#include "camera.h"
#include "api.h"
#include "util.h"
#include "log.h"
#include "demo.h"
#include "ui.h"

static int read_frame(struct v4l2_camera *cam, int (*func)(struct v4l2_camera *, struct buffer))
{
    struct v4l2_buffer buffer_info;
    struct buffer buffer;
    struct time_recorder tr;
    int ret;

    //Count the time of get one frame
    time_recorder_start(&tr);
    ret = camera_dequeue_buffer(cam, &buffer_info);
    if (ret != CAMERA_RETURN_SUCCESS)
        return ret;
    time_recorder_end(&tr);
    time_recorder_print_time(&tr, "Get frame");
    camera_get_buffer(cam, &buffer_info, &buffer);
    ret = func(cam, buffer);
    if (camera_queue_buffer(cam, &buffer_info) != CAMERA_RETURN_SUCCESS) {
        ret = CAMERA_RETURN_FAILURE;
    }
    return ret;
}

static int display_frame(struct v4l2_camera *cam, struct buffer buffer)
{
    return push_frame(buffer.addr, cam->fmt.fmt.pix.width, cam->fmt.fmt.pix.height);
}

static void mainloop(struct v4l2_camera *cam)
{
    int running = 1, ret;
    camera_start_capturing(cam);
    while (running) {
        while((ret = read_frame(cam, display_frame)) == -EAGAIN);
        if (ret != CAMERA_RETURN_SUCCESS)
            break;
    }
    camera_stop_capturing(cam);
}

int main(int argc, char **argv)
{
    int opt;
    struct v4l2_camera *cam = NULL;

    cam = camera_create_object();
    if (!cam) {
        LOGE(DUMP_NONE, "Out of memory\n");
        exit(EXIT_FAILURE);
    }

    LOGI("Parsing command line args:\n");
    while ((opt = getopt(argc, argv, "?vp:w:h:")) != -1) {
        switch(opt){
            case 'v':
                LOGI("Verbose log\n");
                set_log_level(DEBUG);
                break;
            case 'p':
                cam->dev_name = optarg;
                LOGI("Device path: %s\n", cam->dev_name);
                break;
            case 'w':
                cam->fmt.fmt.pix.width = atoi(optarg);
                LOGI("Width: %d\n", cam->fmt.fmt.pix.width);
                break;
            case 'h':
                cam->fmt.fmt.pix.height = atoi(optarg);
                LOGI("Height: %d\n", cam->fmt.fmt.pix.height);
                break;
            case '?':
            default:
                help();
                goto out_free;
        }
    }
    LOGI("Parsing command line args done\n");

    /* Fixed format to YUYV */
    cam->fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
    LOGI("Format: %d\n", cam->fmt.fmt.pix.pixelformat);

    if (camera_open_device(cam))
        goto out_free;
    if (camera_query_cap(cam))
        goto out_close;
    if(!(cam->cap.capabilities & V4L2_CAP_VIDEO_CAPTURE))
    {
        LOGE(DUMP_NONE, "%s is no video capture device\n", cam->dev_name);
        goto out_close;
    }
    if(!(cam->cap.capabilities & V4L2_CAP_STREAMING))
    {
        LOGE(DUMP_NONE, "%s does not support streaming i/o\n", cam->dev_name);
        goto out_close;
    }
    camera_query_support_control(cam);
    camera_query_support_format(cam);

    if (camera_set_output_format(cam))
        goto out_close;

    /* Note VIDIOC_S_FMT may change width and height. */
    camera_get_output_format(cam);

    if (camera_request_and_map_buffer(cam))
        goto out_close;

    if (window_init(cam->fmt.fmt.pix.width, cam->fmt.fmt.pix.height)) {
        LOGE(DUMP_NONE, "Native window init failed.\n");
        goto out_close;
    }
    mainloop(cam);
    window_deinit();

    camera_return_and_unmap_buffer(cam);

out_close:
    camera_close_device(cam);
out_free:
    camera_free_object(cam);
    return CAMERA_RETURN_SUCCESS;
}

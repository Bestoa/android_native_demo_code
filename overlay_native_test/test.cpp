#include <stdio.h>
#include <android/native_window.h>
#include <hardware/gralloc.h>
#include <ui/GraphicBufferMapper.h>
#include <gui/SurfaceComposerClient.h>
#include <gui/SurfaceControl.h>
#include <gui/Surface.h>

using namespace android;

#define HAL_PIXEL_FORMAT_UYVY 0x100

#define GRALLOC_USAGE_VIDEO_OVERLAY 0x00400000
//#define LOCK_USAGE GRALLOC_USAGE_SW_WRITE_OFTEN
#define LOCK_USAGE GRALLOC_USAGE_VIDEO_OVERLAY|GRALLOC_USAGE_PRIVATE_0|GRALLOC_USAGE_SW_WRITE_OFTEN

#define FRAME_W 1280
#define FRAME_H 720

void test()
{
    status_t status = 0;
    GraphicBufferMapper& mapper = GraphicBufferMapper::get();
    sp<SurfaceComposerClient> mSurfaceComposerClient = new SurfaceComposerClient();
    SurfaceComposerClient::Transaction t;

    if(mSurfaceComposerClient == NULL)
    {
        printf("NULL mSurfaceComposerClient!");
    }

    sp<SurfaceControl> mSurfaceControl = mSurfaceComposerClient->createSurface(String8("Test_Surface"), 1280, 720, HAL_PIXEL_FORMAT_UYVY, 0);

    if(mSurfaceControl == NULL)
    {
        printf("NULL mSurfaceControl!");
    }

    sp<Surface> mSurface = mSurfaceControl->getSurface();

    t.setPosition(mSurfaceControl, 100, 100)
        .setLayer(mSurfaceControl, 30000)
        .apply();

    ANativeWindow* window = mSurface.get();

    native_window_api_connect(window, NATIVE_WINDOW_API_CPU);

    int frame = 0;
    FILE *file;
    ANativeWindowBuffer* buf = NULL;

    file = fopen("/data/720p_bigbuckbunny_30f.yuyv", "r");
    if (!file) {
        goto EXIT;
    }

    while(frame++ < 10000000){
        if (feof(file)) {
            fseek(file, 0L, SEEK_SET);
        }
        status = window->dequeueBuffer_DEPRECATED(window, &buf);
        if (status) {
            printf("Deque buffer failed. Err = %d\n", status);
            break;
        }

        void* vaddr;

        mapper.lock(buf->handle, LOCK_USAGE, Rect(FRAME_W, FRAME_H), &vaddr);
        fread(vaddr, 1, FRAME_W*FRAME_H*2, file);
        mapper.unlock(buf->handle);
        status = window->queueBuffer_DEPRECATED(window, buf);
    }

EXIT:
    mSurfaceComposerClient->dispose();
}

int main(void)
{
    pid_t pid;
    pid = fork();
    if(pid==0)
    {
        test();
    }
    return 0;
}

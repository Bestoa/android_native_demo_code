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

int mFrameW = 0;
int mFrameH = 0;

GraphicBufferMapper& mapper = GraphicBufferMapper::get();
sp<SurfaceComposerClient> mSurfaceComposerClient;
sp<SurfaceControl> mSurfaceControl;
sp<Surface> mSurface;
ANativeWindow* mWindow;

extern "C" {
    int window_init(int, int);
    int push_frame(void *, int, int);
    void window_deinit();
}

int window_init(int w, int h)
{
    mSurfaceComposerClient = new SurfaceComposerClient();

    if(mSurfaceComposerClient == nullptr)
    {
        printf("nullptr mSurfaceComposerClient!");
        return 1;
    }

    mFrameW = w;
    mFrameH = h;
    mSurfaceControl = mSurfaceComposerClient->createSurface(String8("Test_Surface"), w, h, HAL_PIXEL_FORMAT_UYVY, 0);

    if(mSurfaceControl == nullptr)
    {
        printf("nullptr mSurfaceControl!");
        return 1;
    }

    mSurface = mSurfaceControl->getSurface();

    mWindow = mSurface.get();
    if (mWindow == nullptr) {
        printf("nullptr mWindow!");
        return 1;
    }
    native_window_api_connect(mWindow, NATIVE_WINDOW_API_CPU);
    return 0;
}

int push_frame(void *addr, int w, int h)
{
    ANativeWindowBuffer* buf = nullptr;
    int status;

    if (w != mFrameW || h != mFrameH) {
        printf("invalid size!\n");
        return 1;
    }

    status = mWindow->dequeueBuffer_DEPRECATED(mWindow, &buf);
    if (status) {
        printf("Deque buffer failed. Err = %d\n", status);
        return 1;
    }

    void* vaddr;

    mapper.lock(buf->handle, LOCK_USAGE, Rect(mFrameW, mFrameH), &vaddr);
    if (!vaddr) {
        printf("lock buffer failed\n");
        return 1;
    }
    memcpy(vaddr, addr, mFrameH * mFrameW * 2);
    mapper.unlock(buf->handle);
    status = mWindow->queueBuffer_DEPRECATED(mWindow, buf);

    return 0;
}

void window_deinit()
{
    native_window_api_disconnect(mWindow, NATIVE_WINDOW_API_CPU);
    mSurfaceComposerClient->dispose();
}

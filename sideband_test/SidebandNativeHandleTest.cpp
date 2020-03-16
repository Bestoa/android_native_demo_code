/*
 * Copyright (C) 2018 Synaptics Incorporated. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */


#include <utils/Log.h>
#include <utils/NativeHandle.h>

#include <system/window.h>
#include <hardware/gralloc.h>

#include <binder/IMemory.h>
#include <gui/ISurfaceComposer.h>
#include <gui/Surface.h>
#include <gui/SurfaceComposerClient.h>
#include <gui/BufferItemConsumer.h>
#include <gui/LayerState.h>
#include <unistd.h>

#include "SidebandNativeHandle.h"

using namespace android;

#define X_FB_SZIE (1920)
#define Y_FB_SZIE (1080)
#define X_WINDOW_SIZE (1280)
#define Y_WINDOW_SIZE (720)
#define ANIMATION_COUNT (60*60*10)

int main(int argc, char** argv)
{
    status_t ret;

    sp<Surface> mSurface;
    sp<SurfaceComposerClient> mComposerClient;
    sp<SurfaceControl> mSurfaceControl;
    SidebandNativeHandle *sb_nativeHandle = NULL;
    sp<NativeHandle> nativeHandle;
    SurfaceComposerClient::Transaction t;

    ANativeWindow* window;
    ANativeWindowBuffer* buf = NULL;

    mComposerClient = new SurfaceComposerClient;
    if (mComposerClient->initCheck()) {
        printf("%s:%i, fail to create mComposerClient\n", __FUNCTION__, __LINE__);
        goto error_exit;
    }

    mSurfaceControl = mComposerClient->createSurface(
                String8("Test Surface"), X_WINDOW_SIZE, Y_WINDOW_SIZE, 0x100, 0);

    if ((mSurfaceControl == NULL) || (!mSurfaceControl->isValid())) {
        printf("%s:%i, fail to create SurfaceControl\n", __FUNCTION__, __LINE__);
        goto error_exit;
    }

    t.setLayer(mSurfaceControl, 0x7fffffff).show(mSurfaceControl).apply();

    mSurface = mSurfaceControl->getSurface();
    if (mSurface == NULL) {
        printf("%s:%i, fail to getSurface\n", __FUNCTION__, __LINE__);
        goto error_exit;
    }
#if 1
    window = mSurface.get();
    native_window_api_connect(window, NATIVE_WINDOW_API_CPU);
    ret = window->dequeueBuffer_DEPRECATED(window, &buf);
    if (ret) {
        printf("Deque buffer failed. Err = %d\n", ret);
        goto error_exit;
    }
    ret = window->queueBuffer_DEPRECATED(window, buf);
    native_window_api_disconnect(window, NATIVE_WINDOW_API_CPU);
#endif

#if 0
    printf("press any number to continue\n");
    scanf("%d", &ret);
    printf("continue\n");
#endif


#if 1
    sb_nativeHandle = new SidebandNativeHandle(true);
    if (!sb_nativeHandle) {
        printf("%s:%i, fail to getSurface\n", __FUNCTION__, __LINE__);
        goto error_exit;
    }
    nativeHandle = NativeHandle::create(sb_nativeHandle, false);

    mSurface->setSidebandStream(nativeHandle);

    for (int i = 0; i < ANIMATION_COUNT; i++)
    {
        usleep(10000);
        t.setPosition(mSurfaceControl, i % (X_FB_SZIE - X_WINDOW_SIZE), i % (Y_FB_SZIE - Y_WINDOW_SIZE))
            .apply();
    }

    mSurface->setSidebandStream(NULL);
#endif

    mComposerClient->dispose();

    goto normal_exit;

error_exit:
    printf("%s:%i, found error, exit...\n", __FUNCTION__, __LINE__);
normal_exit:
    if (sb_nativeHandle)
        delete sb_nativeHandle;

    nativeHandle = NULL;
    sb_nativeHandle = NULL;
    mSurface = NULL;
    mSurfaceControl = NULL;
    mComposerClient = NULL;

    printf("%s:%i, return\n", __FUNCTION__, __LINE__);
    return 0;
}

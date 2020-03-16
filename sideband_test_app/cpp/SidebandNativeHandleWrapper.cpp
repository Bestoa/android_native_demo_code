#include <jni.h>
#include <string>
#include <system/window.h>
#include <android/native_window.h>
#include <android/native_window_jni.h>
#include <utils/Log.h>

#include <utils/NativeHandle.h>
#include "SidebandNativeHandle.h"

using namespace android;

SidebandNativeHandle *sb_nativeHandle = nullptr;

extern "C" JNIEXPORT void JNICALL
Java_com_synaptics_mfang_test_1sideband_MainActivity_setSideband(
        JNIEnv* env,
        jobject obj,
        jobject surface,
        jboolean on)
{
    ANativeWindow* window;

    int ret;

    window = ANativeWindow_fromSurface(env, surface);
    if (NULL == window) {
        ALOGE("++++++++Failed to get native window");
        return;
    }
    ALOGE("+++++++++Succeed to get native window");

    if (!on) {
        native_window_set_sideband_stream(window, nullptr);
        ALOGE("+++++++++exit sideband");
        if (sb_nativeHandle != nullptr) {
            delete sb_nativeHandle;
        }
        return;
    }

    if (sb_nativeHandle == nullptr) {
        sb_nativeHandle = new SidebandNativeHandle(true);
    }

    if (sb_nativeHandle == nullptr) {
        ALOGE("++++++++Failed to create SidebandNativeHandle");
    }
    sp<NativeHandle> nativeHandle = NativeHandle::create(sb_nativeHandle, false);

    ALOGE("+++++++++enter sideband");
    native_window_set_sideband_stream(window, const_cast<native_handle_t *>(nativeHandle->handle()));
}

LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_SRC_FILES:= \
	test.cpp

LOCAL_SHARED_LIBRARIES := \
	libcutils \
	libutils \
	libandroid \
	libgui \
	libui \
	liblog

LOCAL_MODULE:= overlay_native_test

LOCAL_MODULE_TAGS := optional

LOCAL_CFLAGS += \
				-Wno-deprecated-declarations

include $(BUILD_EXECUTABLE)

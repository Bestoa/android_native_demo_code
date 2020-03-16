LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_SRC_FILES:= \
	libcamera_base/camera.c \
	libcamera_base/log.c \
	libcamera_base/util.c \
	cam_main.c \
	ui_main.cpp

LOCAL_SHARED_LIBRARIES := \
	libcutils \
	libutils \
	libandroid \
	libgui \
	libui \
	liblog

LOCAL_MODULE:= camera_native_test

LOCAL_MODULE_TAGS := optional

LOCAL_CFLAGS += \
				-Wno-deprecated-declarations \
				-Wno-unused-parameter

LOCAL_C_INCLUDES += \
					$(LOCAL_PATH)/include

include $(BUILD_EXECUTABLE)

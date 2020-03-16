LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_CPP_EXTENSION := .cpp
LOCAL_MODULE    := libjni_SidebandHandle
LOCAL_SRC_FILES := \
	SidebandNativeHandleWrapper.cpp \
	SidebandNativeHandle.cpp

LOCAL_PRODUCT_MODULE := true

LOCAL_SHARED_LIBRARIES += \
						libnativewindow\
						libutils \
						libcutils \
						liblog \
						libandroid

LOCAL_CFLAGS += \
				-Wno-unused-parameter \
				-Wno-unused-variable

include $(BUILD_SHARED_LIBRARY)


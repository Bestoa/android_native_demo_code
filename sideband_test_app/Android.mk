LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE_TAGS := optional

LOCAL_SRC_FILES := $(call all-java-files-under, java)

LOCAL_PRODUCT_MODULE := true

LOCAL_CERTIFICATE := platform

LOCAL_RESOURCE_DIR += \
	$(LOCAL_PATH)/res

LOCAL_PACKAGE_NAME := SidebandTest

LOCAL_SDK_VERSION := current

LOCAL_JNI_SHARED_LIBRARIES := libjni_SidebandHandle

include $(BUILD_PACKAGE)
include $(call all-makefiles-under, $(LOCAL_PATH))

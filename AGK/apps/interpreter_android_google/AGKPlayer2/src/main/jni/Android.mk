# Copyright (C) 2010 The Android Open Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
# 
LOCAL_PATH := $(call my-dir)

### copy the AGK lib into the objs directory for linking ###
include $(CLEAR_VARS)
LOCAL_MODULE    := AGKBullet
LOCAL_SRC_FILES := ../../../../../../platform/android/jni/$(TARGET_ARCH_ABI)/libAGKBullet.a
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/../../../../../../common/include \
    $(LOCAL_PATH)/../../../../../../bullet \
    $(LOCAL_PATH)/../../../../../../bullet/BulletCollision/CollisionShapes
include $(PREBUILT_STATIC_LIBRARY) 

include $(CLEAR_VARS)
LOCAL_MODULE    := AGKAssimp
LOCAL_SRC_FILES := ../../../../../../platform/android/jni/$(TARGET_ARCH_ABI)/libAGKAssimp.a
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/../../../../../../common/include \
    $(LOCAL_PATH)/../../../../../../bullet \
    $(LOCAL_PATH)/../../../../../../bullet/BulletCollision/CollisionShapes
include $(PREBUILT_STATIC_LIBRARY) 

include $(CLEAR_VARS)
LOCAL_MODULE    := AGKCurl
LOCAL_SRC_FILES := ../../../../../../platform/android/jni/$(TARGET_ARCH_ABI)/libAGKCurl.a
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/../../../../../../common/include \
    $(LOCAL_PATH)/../../../../../../bullet \
    $(LOCAL_PATH)/../../../../../../bullet/BulletCollision/CollisionShapes
include $(PREBUILT_STATIC_LIBRARY) 

include $(CLEAR_VARS)
LOCAL_MODULE    := AGKAndroid
LOCAL_SRC_FILES := ../../../../../../platform/android/jni/$(TARGET_ARCH_ABI)/libAGKAndroid.a
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/../../../../../../common/include \
    $(LOCAL_PATH)/../../../../../../bullet \
    $(LOCAL_PATH)/../../../../../../bullet/BulletCollision/CollisionShapes
LOCAL_STATIC_LIBRARIES := AGKBullet AGKAssimp
include $(PREBUILT_STATIC_LIBRARY) 

### build the app ###
include $(CLEAR_VARS)

# the name of the library referenced from the AndroidManifest.xml file
LOCAL_MODULE    := android_player

# agk includes folder
LOCAL_C_INCLUDES := $(LOCAL_PATH) \
                $(LOCAL_PATH)/../../../../../../common/include \
                $(LOCAL_PATH)/../../../../../interpreter \
                $(LOCAL_PATH)/../../../../../../renderer

# app source files, must be relative to the jni folder
LOCAL_SRC_FILES := ../../../../../android_common_native/main.c \
               ../../../../../android_common_native/Core.cpp \
               ../../../../../interpreter/interpreter.cpp \
               ../../../../../interpreter/ProgramData.cpp

# included system libraries
LOCAL_LDLIBS    := -lm -llog -landroid -lEGL -lGLESv2 -lz -lOpenSLES

# included user libraris
LOCAL_STATIC_LIBRARIES := AGKAndroid AGKBullet AGKAssimp AGKCurl android_native_app_glue

# define IDE_ANDROID (for AGK) and use O2 optimizations
LOCAL_CFLAGS += -DIDE_ANDROID -O2 -fsigned-char
LOCAL_CPPFLAGS += -fexceptions -std=c++11

# use arm instead of thumb instructions
LOCAL_ARM_MODE := arm

include $(BUILD_SHARED_LIBRARY)

### build the native support library ###
$(call import-module,android/native_app_glue)

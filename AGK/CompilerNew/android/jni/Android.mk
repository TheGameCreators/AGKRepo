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

include $(CLEAR_VARS)

LOCAL_MODULE    := AGKCompiler
LOCAL_C_INCLUDES := $(LOCAL_PATH)/../../
LOCAL_SRC_FILES := ../../AGKCompiler2.cpp \
    ../../cFile.cpp \
    ../../DynArray.cpp \
    ../../Parser1.cpp \
    ../../Parser2.cpp \
    ../../ProgramData.cpp \
    ../../../common/Source/uString.cpp \
    ../../AndroidCompiler.cpp

LOCAL_EXPORT_LDLIBS    := -llog -landroid -lz
LOCAL_CFLAGS += -DAGK_ANDROID -DAGK_COMPILER -O3 -DAGK_STATIC_LIB -fsigned-char
LOCAL_CPPFLAGS += -fexceptions
LOCAL_ARM_MODE := arm
LOCAL_SHORT_COMMANDS := false

# hack to avoid compilation output jumping out of the armeabi folder due to ../../../
ifeq ($(NDK_APP_OPTIM),debug)
TARGET_OBJS := $(TARGET_OUT)/objs-debug/objs2
else
TARGET_OBJS := $(TARGET_OUT)/objs/objs2
endif

include $(BUILD_STATIC_LIBRARY)

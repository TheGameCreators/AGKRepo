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

LOCAL_MODULE    := AGKAndroid
LOCAL_C_INCLUDES := $(LOCAL_PATH)/../../../common/include \
    $(LOCAL_PATH)/../../../common \
    $(LOCAL_PATH)/../../../common/Collision \
    $(LOCAL_PATH)/../../../libpng \
    $(LOCAL_PATH)/../../../libjpeg \
    $(LOCAL_PATH)/../../../zlib \
    $(LOCAL_PATH)/../../../zxing \
    $(LOCAL_PATH)/../../../curl \
    $(LOCAL_PATH)/../../../curl/include \
    $(LOCAL_PATH)/../../../giflib \
    $(LOCAL_PATH)/../../../json \
    $(LOCAL_PATH)/../../../assimp \
    $(LOCAL_PATH)/../../../bullet \
    $(LOCAL_PATH)/../../../bullet/BulletCollision/CollisionShapes \
    $(LOCAL_PATH)/../../../libogg/include \
    $(LOCAL_PATH)/../../../FirebaseSDK/include \
    $(LOCAL_PATH)/../../../freetype/include \
    $(LOCAL_PATH)/../../../renderer/OpenGLES2  \
    $(LOCAL_PATH)/../../../platform/android/ARCore/include \
    $(LOCAL_PATH)/../../../renderer/MoltenVK/include \
    $(LOCAL_PATH)/../../../renderer
LOCAL_SRC_FILES := ../../../common/Source/AGKMotionState.cpp \
    ../../../common/Source/AGKToBullet.cpp \
    ../../../common/Source/AppErrors.cpp \
    ../../../common/Source/BaseItem.cpp \
    ../../../common/Source/CollisionShapes.cpp \
    ../../../common/Source/ContactReport.cpp \
    ../../../common/Source/DynamicsWorld.cpp \
    ../../../common/Source/GLDebugDrawer.cpp \
    ../../../common/Source/Joint.cpp \
    ../../../common/Source/Joints.cpp \
    ../../../common/Source/Ray.cpp \
    ../../../common/Source/RigidBodies.cpp \
    ../../../common/Source/RigidBody.cpp \
    ../../../common/Source/StaticPlane.cpp \
    ../../../common/Source/CharacterController.cpp \
    ../../../common/Source/Ragdoll.cpp \
    ../../../common/Source/RagdollBone.cpp \
    ../../../common/Source/Vector.cpp \
    ../../../common/Source/Wrapper.cpp \
    ../../../common/Source/uString.cpp \
    ../../../common/Source/Physics.cpp \
    ../../../common/Source/Particles.cpp \
    ../../../common/Source/Network.cpp \
    ../../../common/Source/Mouse.cpp \
    ../../../common/Source/Joystick.cpp \
    ../../../common/Source/DebugDraw.cpp \
    ../../../common/Source/cTouch.cpp \
    ../../../common/Source/cText.cpp \
    ../../../common/Source/cSpriteMgr.cpp \
    ../../../common/Source/cSprite.cpp \
    ../../../common/Source/cSound.cpp \
    ../../../common/Source/cNode.cpp \
    ../../../common/Source/cImage.cpp \
    ../../../common/Source/cForce.cpp \
    ../../../common/Source/cFile.cpp \
    ../../../common/Source/cEvent.cpp \
    ../../../common/Source/cTextEntry.cpp \
    ../../../common/Source/AGKFont.cpp \
    ../../../common/Source/ZipFile.cpp \
    ../../../common/Source/sha1.cpp \
    ../../../common/Source/sha2.cpp \
    ../../../common/Source/AGO.cpp \
    ../../../common/Source/cMesh.cpp \
    ../../../common/Source/cObject3D.cpp \
    ../../../common/Source/cObjectMgr.cpp \
    ../../../common/Source/3DMath.cpp \
    ../../../common/Source/AGKShader.cpp \
    ../../../common/Source/cCamera.cpp \
    ../../../common/Source/FrameBuffer.cpp \
    ../../../common/Source/JSONElement.cpp \
    ../../../common/Source/Skeleton2D.cpp \
    ../../../common/Source/Skeleton3D.cpp \
    ../../../common/Source/Tween.cpp \
    ../../../common/Source/AGKSkyBox.cpp \
    ../../../common/Source/3DParticles.cpp \
	../../../common/Source/SimplexNoise.cpp \
    ../../../common/Collision/Box.cpp \
    ../../../common/Collision/CollisionBox.cpp \
    ../../../common/Collision/CollisionPolygon.cpp \
    ../../../common/Collision/CollisionResults.cpp \
    ../../../common/Collision/CollisionSphere.cpp \
    ../../../common/Collision/CollisionTree.cpp \
    ../../../common/Collision/Face.cpp \
    ../../../common/Collision/TreeNode.cpp \
    box2d.cpp \
    ../../../platform/android/Source/AndroidCore.cpp \
    ../../../platform/android/Source/AndroidNetwork.cpp \
    ../../../renderer/OpenGLES2/OpenGL_ES2.cpp \
    ../../../renderer/Vulkan/VulkanShaders.cpp \
    ../../../renderer/Vulkan/VulkanVertices.cpp \
    ../../../renderer/Vulkan/VulkanFrameBuffer.cpp \
    ../../../renderer/Vulkan/AGKVulkan.cpp \
    ../../../renderer/Vulkan/GLSLtoSPIRV/glslang/OSDependent/Unix/ossource.cpp \
    ../../../renderer/Vulkan/GLSLtoSPIRV/glslang/GenericCodeGen/CodeGen.cpp \
    ../../../renderer/Vulkan/GLSLtoSPIRV/glslang/GenericCodeGen/Link.cpp \
    ../../../renderer/Vulkan/GLSLtoSPIRV/glslang/MachineIndependent/InfoSink.cpp \
    ../../../renderer/Vulkan/GLSLtoSPIRV/glslang/MachineIndependent/glslang_tab.cpp \
    ../../../renderer/Vulkan/GLSLtoSPIRV/glslang/MachineIndependent/parseConst.cpp \
    ../../../renderer/Vulkan/GLSLtoSPIRV/glslang/MachineIndependent/propagateNoContraction.cpp \
    ../../../renderer/Vulkan/GLSLtoSPIRV/glslang/MachineIndependent/IntermTraverse.cpp \
    ../../../renderer/Vulkan/GLSLtoSPIRV/glslang/MachineIndependent/ParseHelper.cpp \
    ../../../renderer/Vulkan/GLSLtoSPIRV/glslang/MachineIndependent/reflection.cpp \
    ../../../renderer/Vulkan/GLSLtoSPIRV/glslang/MachineIndependent/linkValidate.cpp \
    ../../../renderer/Vulkan/GLSLtoSPIRV/glslang/MachineIndependent/ParseContextBase.cpp \
    ../../../renderer/Vulkan/GLSLtoSPIRV/glslang/MachineIndependent/Scan.cpp \
    ../../../renderer/Vulkan/GLSLtoSPIRV/glslang/MachineIndependent/Versions.cpp \
    ../../../renderer/Vulkan/GLSLtoSPIRV/glslang/MachineIndependent/Intermediate.cpp \
    ../../../renderer/Vulkan/GLSLtoSPIRV/glslang/MachineIndependent/pch.cpp \
    ../../../renderer/Vulkan/GLSLtoSPIRV/glslang/MachineIndependent/attribute.cpp \
    ../../../renderer/Vulkan/GLSLtoSPIRV/glslang/MachineIndependent/Initialize.cpp \
    ../../../renderer/Vulkan/GLSLtoSPIRV/glslang/MachineIndependent/SymbolTable.cpp \
    ../../../renderer/Vulkan/GLSLtoSPIRV/glslang/MachineIndependent/limits.cpp \
    ../../../renderer/Vulkan/GLSLtoSPIRV/glslang/MachineIndependent/ShaderLang.cpp \
    ../../../renderer/Vulkan/GLSLtoSPIRV/glslang/MachineIndependent/intermOut.cpp \
    ../../../renderer/Vulkan/GLSLtoSPIRV/glslang/MachineIndependent/PoolAlloc.cpp \
    ../../../renderer/Vulkan/GLSLtoSPIRV/glslang/MachineIndependent/RemoveTree.cpp \
    ../../../renderer/Vulkan/GLSLtoSPIRV/glslang/MachineIndependent/iomapper.cpp \
    ../../../renderer/Vulkan/GLSLtoSPIRV/glslang/MachineIndependent/Constant.cpp \
    ../../../renderer/Vulkan/GLSLtoSPIRV/glslang/MachineIndependent/preprocessor/Pp.cpp \
    ../../../renderer/Vulkan/GLSLtoSPIRV/glslang/MachineIndependent/preprocessor/PpTokens.cpp \
    ../../../renderer/Vulkan/GLSLtoSPIRV/glslang/MachineIndependent/preprocessor/PpAtom.cpp \
    ../../../renderer/Vulkan/GLSLtoSPIRV/glslang/MachineIndependent/preprocessor/PpContext.cpp \
    ../../../renderer/Vulkan/GLSLtoSPIRV/glslang/MachineIndependent/preprocessor/PpScanner.cpp \
    ../../../renderer/Vulkan/GLSLtoSPIRV/SPIRV/SpvBuilder.cpp \
    ../../../renderer/Vulkan/GLSLtoSPIRV/SPIRV/disassemble.cpp \
    ../../../renderer/Vulkan/GLSLtoSPIRV/SPIRV/GlslangToSpv.cpp \
    ../../../renderer/Vulkan/GLSLtoSPIRV/SPIRV/SpvPostProcess.cpp \
    ../../../renderer/Vulkan/GLSLtoSPIRV/SPIRV/Logger.cpp \
    ../../../renderer/Vulkan/GLSLtoSPIRV/SPIRV/SPVRemapper.cpp \
    ../../../renderer/Vulkan/GLSLtoSPIRV/SPIRV/InReadableOrder.cpp \
    ../../../renderer/Vulkan/GLSLtoSPIRV/SPIRV/doc.cpp \
    ../../../renderer/Vulkan/GLSLtoSPIRV/SPIRV/SpvTools.cpp \
    ../../../renderer/Vulkan/GLSLtoSPIRV/OGLCompilersDLL/InitializeDll.cpp \
    ../../../renderer/Vulkan/GLSLtoSPIRV/glslang_main/ResourceLimits.cpp \
    ../../../renderer/Vulkan/GLSLtoSPIRV/glslang_main/glslang_main.cpp \
    ../../../renderer/Vulkan/VulkanImage.cpp \
    ../../../renderer/Vulkan/GLSLParser.cpp \
    ../../../renderer/Vulkan/spirv_reflect.c \
    ../../../renderer/Vulkan/VulkanFunctions.cpp \
    ../../../renderer/Vulkan/VulkanPipeline.cpp \
    ../../../zlib/adler32.c \
    ../../../zlib/compress.c \
    ../../../zlib/crc32.c \
    ../../../zlib/deflate.c \
    ../../../zlib/gzclose.c \
    ../../../zlib/gzlib.c \
    ../../../zlib/gzread.c \
    ../../../zlib/gzwrite.c \
    ../../../zlib/infback.c \
    ../../../zlib/inffast.c \
    ../../../zlib/inflate.c \
    ../../../zlib/inftrees.c \
    ../../../zlib/ioapi.c \
    ../../../zlib/trees.c \
    ../../../zlib/uncompr.c \
    ../../../zlib/unzip.c \
    ../../../zlib/zip.c \
    ../../../zlib/zutil.c \
    zxing.cpp \
    ../../../zxing/datamatrix/DataMatrixReader.cpp \
    ../../../zxing/datamatrix/Version2.cpp \
    ../../../zxing/datamatrix/decoder/BitMatrixParser2.cpp \
    ../../../zxing/datamatrix/decoder/DataBlock2.cpp \
    ../../../zxing/datamatrix/decoder/DecodedBitStreamParser2.cpp \
    ../../../zxing/datamatrix/decoder/Decoder2.cpp \
    ../../../zxing/datamatrix/detector/CornerPoint.cpp \
    ../../../zxing/datamatrix/detector/Detector2.cpp \
    ../../../zxing/datamatrix/detector/DetectorException.cpp \
    ../../../zxing/datamatrix/detector/MonochromeRectangleDetector2.cpp \
    libpng.c \
    ../../../libjpeg/jdatasrc.c \
    ../../../libjpeg/jdapimin.c \
    ../../../libjpeg/jdmarker.c \
    ../../../libjpeg/jerror.c \
    ../../../libjpeg/jdinput.c \
    ../../../libjpeg/jdapistd.c \
    ../../../libjpeg/jcomapi.c \
    ../../../libjpeg/jmemmgr.c \
    ../../../libjpeg/jutils.c \
    ../../../libjpeg/jdmaster.c \
    ../../../libjpeg/jmemansi.c \
    ../../../libjpeg/jquant1.c \
    ../../../libjpeg/jquant2.c \
    ../../../libjpeg/jddctmgr.c \
    ../../../libjpeg/jdarith.c \
    ../../../libjpeg/jdcoefct.c \
    ../../../libjpeg/jdcolor.c \
    ../../../libjpeg/jdsample.c \
    ../../../libjpeg/jdpostct.c \
    ../../../libjpeg/jdhuff.c \
    ../../../libjpeg/jdmerge.c \
    ../../../libjpeg/jdmainct.c \
    ../../../libjpeg/jidctint.c \
    ../../../libjpeg/jaricom.c \
    ../../../libjpeg/jidctfst.c \
    ../../../libjpeg/jidctflt.c \
    ../../../libjpeg/jcapimin.c \
    ../../../libjpeg/jdatadst.c \
    ../../../libjpeg/jcparam.c \
    ../../../libjpeg/jcapistd.c \
    ../../../libjpeg/jcmarker.c \
    ../../../libjpeg/jcinit.c \
    ../../../libjpeg/jcmaster.c \
    ../../../libjpeg/jcmaster.c \
    ../../../libjpeg/jcdctmgr.c \
    ../../../libjpeg/jcarith.c \
    ../../../libjpeg/jccoefct.c \
    ../../../libjpeg/jcmainct.c \
    ../../../libjpeg/jchuff.c \
    ../../../libjpeg/jccolor.c \
    ../../../libjpeg/jcsample.c \
    ../../../libjpeg/jcprepct.c \
    ../../../libjpeg/jfdctint.c \
    ../../../libjpeg/jfdctfst.c \
    ../../../libjpeg/jfdctflt.c \
    ../../../giflib/dev2gif.c \
    ../../../giflib/dgif_lib.c \
    ../../../giflib/egif_lib.c \
    ../../../giflib/getarg.c \
    ../../../giflib/gif_err.c \
    ../../../giflib/gif_font.c \
    ../../../giflib/gif_hash.c \
    ../../../giflib/gifalloc.c \
    ../../../giflib/qprintf.c \
    ../../../giflib/quantize.c \
    ../../../json/dump.c \
    ../../../json/error.c \
    ../../../json/hashtable.c \
    ../../../json/load.c \
    ../../../json/memory.c \
    ../../../json/pack_unpack.c \
    ../../../json/strbuffer.c \
    ../../../json/strconv.c \
    ../../../json/utf.c \
    ../../../json/value.c \
    ../../../libogg/src/mdct.c \
    ../../../libogg/src/psy.c \
    ../../../libogg/src/registry.c \
    ../../../libogg/src/res0.c \
    ../../../libogg/src/sharedbook.c \
    ../../../libogg/src/smallft.c \
    ../../../libogg/src/synthesis.c \
    ../../../libogg/src/vorbisenc.c \
    ../../../libogg/src/vorbisfile.c \
    ../../../libogg/src/window.c \
    ../../../libogg/src/analysis.c \
    ../../../libogg/src/bitrate.c \
    ../../../libogg/src/bitwise.c \
    ../../../libogg/src/block.c \
    ../../../libogg/src/codebook.c \
    ../../../libogg/src/envelope.c \
    ../../../libogg/src/floor0.c \
    ../../../libogg/src/floor1.c \
    ../../../libogg/src/framing.c \
    ../../../libogg/src/info.c \
    ../../../libogg/src/lookup.c \
    ../../../libogg/src/lpc.c \
    ../../../libogg/src/lsp.c \
    ../../../libogg/src/mapping0.c \
    ../../../freetype/src/autofit/autofit.c \
    ../../../freetype/src/bdf/bdf.c \
    ../../../freetype/src/cff/cff.c \
    ../../../freetype/src/base/ftbase.c \
    ../../../freetype/src/base/ftbitmap.c \
    ../../../freetype/src/cache/ftcache.c \
    ../../../freetype/src/base/ftdebug.c \
    ../../../freetype/src/base/ftfstype.c \
    ../../../freetype/src/base/ftgasp.c \
    ../../../freetype/src/base/ftglyph.c \
    ../../../freetype/src/gzip/ftgzip.c \
    ../../../freetype/src/base/ftinit.c \
    ../../../freetype/src/lzw/ftlzw.c \
    ../../../freetype/src/base/ftstroke.c \
    ../../../freetype/src/base/ftsystem.c \
    ../../../freetype/src/smooth/smooth.c \
    ../../../freetype/src/base/ftbbox.c \
    ../../../freetype/src/base/ftfntfmt.c \
    ../../../freetype/src/base/ftgxval.c \
    ../../../freetype/src/base/ftlcdfil.c \
    ../../../freetype/src/base/ftmm.c \
    ../../../freetype/src/base/ftotval.c \
    ../../../freetype/src/base/ftpatent.c \
    ../../../freetype/src/base/ftpfr.c \
    ../../../freetype/src/base/ftsynth.c \
    ../../../freetype/src/base/fttype1.c \
    ../../../freetype/src/base/ftwinfnt.c \
    ../../../freetype/src/pcf/pcf.c \
    ../../../freetype/src/pfr/pfr.c \
    ../../../freetype/src/psaux/psaux.c \
    ../../../freetype/src/pshinter/pshinter.c \
    ../../../freetype/src/psnames/psmodule.c \
    ../../../freetype/src/raster/raster.c \
    ../../../freetype/src/sfnt/sfnt.c \
    ../../../freetype/src/truetype/truetype.c \
    ../../../freetype/src/type1/type1.c \
    ../../../freetype/src/cid/type1cid.c \
    ../../../freetype/src/type42/type42.c \
    ../../../freetype/src/winfonts/winfnt.c

LOCAL_EXPORT_LDLIBS    := -llog -landroid -lEGL -lGLESv1_CM -lz -lOpenSLES 
LOCAL_CFLAGS += -DIDE_ANDROID -O2 -DUSE_MBEDTLS -DHAVE_CONFIG_H -fsigned-char
LOCAL_CPPFLAGS += -fexceptions -std=c++11
LOCAL_ARM_MODE := arm
LOCAL_SHORT_COMMANDS := false

# hack to avoid compilation output jumping out of the armeabi folder due to ../../../
ifeq ($(NDK_APP_OPTIM),debug)
TARGET_OBJS := $(TARGET_OUT)/objs-debug/objs2
else
TARGET_OBJS := $(TARGET_OUT)/objs/objs2
endif

include $(BUILD_STATIC_LIBRARY)

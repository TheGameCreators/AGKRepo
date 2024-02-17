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

LOCAL_MODULE    := AGKAssimp
LOCAL_C_INCLUDES := $(LOCAL_PATH)/../../../common/include \
    $(LOCAL_PATH)/../../../common \
    $(LOCAL_PATH)/../../../common/Collision \
    $(LOCAL_PATH)/libpng \
    $(LOCAL_PATH)/libjpeg \
    $(LOCAL_PATH)/../../../zlib \
    $(LOCAL_PATH)/../../../zxing \
    $(LOCAL_PATH)/../../../curl \
    $(LOCAL_PATH)/../../../curl/include \
    $(LOCAL_PATH)/../../../curl/axTLS \
    $(LOCAL_PATH)/../../../giflib \
    $(LOCAL_PATH)/../../../json \
    $(LOCAL_PATH)/../../../assimp \
    $(LOCAL_PATH)/../../../bullet \
    $(LOCAL_PATH)/../../../bullet/BulletCollision/CollisionShapes
LOCAL_SRC_FILES := ../../../assimp/3DSConverter.cpp \
    ../../../assimp/3DSLoader.cpp \
    ../../../assimp/ACLoader.cpp \
    ../../../assimp/ASELoader.cpp \
    ../../../assimp/ASEParser.cpp \
    ../../../assimp/Assimp.cpp \
    ../../../assimp/AssimpCExport.cpp \
    ../../../assimp/AssimpPCH.cpp \
    ../../../assimp/B3DImporter.cpp \
    ../../../assimp/BaseImporter.cpp \
    ../../../assimp/BaseProcess.cpp \
    ../../../assimp/BlenderDNA.cpp \
    ../../../assimp/BlenderLoader.cpp \
    ../../../assimp/BlenderModifier.cpp \
    ../../../assimp/BlenderScene.cpp \
    ../../../assimp/BVHLoader.cpp \
    ../../../assimp/CalcTangentsProcess.cpp \
    ../../../assimp/COBLoader.cpp \
    ../../../assimp/ColladaExporter.cpp \
    ../../../assimp/ColladaLoader.cpp \
    ../../../assimp/ColladaParser.cpp \
    ../../../assimp/ComputeUVMappingProcess.cpp \
    ../../../assimp/ConvertToLHProcess.cpp \
    ../../../assimp/CSMLoader.cpp \
    ../../../assimp/DeboneProcess.cpp \
    ../../../assimp/DefaultIOStream.cpp \
    ../../../assimp/DefaultIOSystem.cpp \
    ../../../assimp/DefaultLogger.cpp \
    ../../../assimp/DXFLoader.cpp \
    ../../../assimp/Exporter.cpp \
    ../../../assimp/FindDegenerates.cpp \
    ../../../assimp/FindInstancesProcess.cpp \
    ../../../assimp/FindInvalidDataProcess.cpp \
    ../../../assimp/FixNormalsStep.cpp \
    ../../../assimp/GenFaceNormalsProcess.cpp \
    ../../../assimp/GenVertexNormalsProcess.cpp \
    ../../../assimp/HMPLoader.cpp \
    ../../../assimp/IFCCurve.cpp \
    ../../../assimp/IFCGeometry.cpp \
    ../../../assimp/IFCLoader.cpp \
    ../../../assimp/IFCMaterial.cpp \
    ../../../assimp/IFCProfile.cpp \
    ../../../assimp/IFCReaderGen.cpp \
    ../../../assimp/IFCUtil.cpp \
    ../../../assimp/Importer.cpp \
    ../../../assimp/ImporterRegistry.cpp \
    ../../../assimp/ImproveCacheLocality.cpp \
    ../../../assimp/IRRLoader.cpp \
    ../../../assimp/IRRMeshLoader.cpp \
    ../../../assimp/IRRShared.cpp \
    ../../../assimp/JoinVerticesProcess.cpp \
    ../../../assimp/LimitBoneWeightsProcess.cpp \
    ../../../assimp/LWOAnimation.cpp \
    ../../../assimp/LWOBLoader.cpp \
    ../../../assimp/LWOLoader.cpp \
    ../../../assimp/LWOMaterial.cpp \
    ../../../assimp/LWSLoader.cpp \
    ../../../assimp/MakeVerboseFormat.cpp \
    ../../../assimp/MaterialSystem.cpp \
    ../../../assimp/MD2Loader.cpp \
    ../../../assimp/MD3Loader.cpp \
    ../../../assimp/MD5Loader.cpp \
    ../../../assimp/MD5Parser.cpp \
    ../../../assimp/MDCLoader.cpp \
    ../../../assimp/MDLLoader.cpp \
    ../../../assimp/MDLMaterialLoader.cpp \
    ../../../assimp/MS3DLoader.cpp \
    ../../../assimp/NDOLoader.cpp \
    ../../../assimp/NFFLoader.cpp \
    ../../../assimp/ObjExporter.cpp \
    ../../../assimp/ObjFileImporter.cpp \
    ../../../assimp/ObjFileMtlImporter.cpp \
    ../../../assimp/ObjFileParser.cpp \
    ../../../assimp/OFFLoader.cpp \
    ../../../assimp/OgreImporter.cpp \
    ../../../assimp/OgreMaterial.cpp \
    ../../../assimp/OptimizeGraph.cpp \
    ../../../assimp/OptimizeMeshes.cpp \
    ../../../assimp/PlyExporter.cpp \
    ../../../assimp/PlyLoader.cpp \
    ../../../assimp/PlyParser.cpp \
    ../../../assimp/PostStepRegistry.cpp \
    ../../../assimp/PretransformVertices.cpp \
    ../../../assimp/ProcessHelper.cpp \
    ../../../assimp/Q3BSPFileImporter.cpp \
    ../../../assimp/Q3BSPFileParser.cpp \
    ../../../assimp/Q3BSPZipArchive.cpp \
    ../../../assimp/Q3DLoader.cpp \
    ../../../assimp/RawLoader.cpp \
    ../../../assimp/RemoveComments.cpp \
    ../../../assimp/RemoveRedundantMaterials.cpp \
    ../../../assimp/RemoveVCProcess.cpp \
    ../../../assimp/SceneCombiner.cpp \
    ../../../assimp/ScenePreprocessor.cpp \
    ../../../assimp/SGSpatialSort.cpp \
    ../../../assimp/SkeletonMeshBuilder.cpp \
    ../../../assimp/SMDLoader.cpp \
    ../../../assimp/SortByPTypeProcess.cpp \
    ../../../assimp/SpatialSort.cpp \
    ../../../assimp/SplitByBoneCountProcess.cpp \
    ../../../assimp/SplitLargeMeshes.cpp \
    ../../../assimp/StandardShapes.cpp \
    ../../../assimp/STEPFileReader.cpp \
    ../../../assimp/STLExporter.cpp \
    ../../../assimp/STLLoader.cpp \
    ../../../assimp/Subdivision.cpp \
    ../../../assimp/TargetAnimation.cpp \
    ../../../assimp/TerragenLoader.cpp \
    ../../../assimp/TextureTransform.cpp \
    ../../../assimp/TriangulateProcess.cpp \
    ../../../assimp/UnrealLoader.cpp \
    ../../../assimp/ValidateDataStructure.cpp \
    ../../../assimp/VertexTriangleAdjacency.cpp \
    ../../../assimp/XFileImporter.cpp \
    ../../../assimp/XFileParser.cpp \
    ../../../assimp/XGLLoader.cpp \
    ../../../assimp/include/ConvertUTF/ConvertUTF.c \
    ../../../assimp/include/irrXML/irrXML.cpp \
    ../../../assimp/include/poly2tri/common/shapes.cc \
    ../../../assimp/include/poly2tri/sweep/advancing_front.cc \
    ../../../assimp/include/poly2tri/sweep/cdt.cc \
    ../../../assimp/include/poly2tri/sweep/sweep.cc \
    ../../../assimp/include/poly2tri/sweep/sweep_context.cc \
    ../../../assimp/Bitmap.cpp \
    ../../../assimp/BlenderBMesh.cpp \
    ../../../assimp/BlenderTessellator.cpp \
    ../../../assimp/FBXAnimation.cpp \
    ../../../assimp/FBXBinaryTokenizer.cpp \
    ../../../assimp/FBXConverter.cpp \
    ../../../assimp/FBXDeformer.cpp \
    ../../../assimp/FBXDocument.cpp \
    ../../../assimp/FBXDocumentUtil.cpp \
    ../../../assimp/FBXImporter.cpp \
    ../../../assimp/FBXMaterial.cpp \
    ../../../assimp/FBXMeshGeometry.cpp \
    ../../../assimp/FBXModel.cpp \
    ../../../assimp/FBXNodeAttribute.cpp \
    ../../../assimp/FBXParser.cpp \
    ../../../assimp/FBXProperties.cpp \
    ../../../assimp/FBXTokenizer.cpp \
    ../../../assimp/FBXUtil.cpp \
    ../../../assimp/IFCBoolean.cpp \
    ../../../assimp/IFCOpenings.cpp \
    ../../../assimp/OgreBinarySerializer.cpp \
    ../../../assimp/OgreStructs.cpp \
    ../../../assimp/OgreXmlSerializer.cpp \
    ../../../assimp/STEPFileEncoding.cpp

LOCAL_EXPORT_LDLIBS    := -llog -landroid -lEGL -lGLESv1_CM -lz -lOpenSLES 
LOCAL_CFLAGS += -DIDE_ANDROID -O3 -DUSE_AXTLS -DHAVE_CONFIG_H -fsigned-char
LOCAL_CPPFLAGS += -fexceptions -std=c++11 -frtti
LOCAL_ARM_MODE := arm
LOCAL_SHORT_COMMANDS := false

# hack to avoid compilation output jumping out of the armeabi folder due to ../../../
ifeq ($(NDK_APP_OPTIM),debug)
TARGET_OBJS := $(TARGET_OUT)/objs-debug/objs2
else
TARGET_OBJS := $(TARGET_OUT)/objs/objs2
endif

include $(BUILD_STATIC_LIBRARY)

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

LOCAL_MODULE    := AGKBullet
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
LOCAL_SRC_FILES := ../../../bullet/BulletCollision/BroadphaseCollision/btAxisSweep3.cpp \
    ../../../bullet/BulletCollision/BroadphaseCollision/btBroadphaseProxy.cpp \
    ../../../bullet/BulletCollision/BroadphaseCollision/btCollisionAlgorithm.cpp \
    ../../../bullet/BulletCollision/BroadphaseCollision/btDbvt.cpp \
    ../../../bullet/BulletCollision/BroadphaseCollision/btDbvtBroadphase.cpp \
    ../../../bullet/BulletCollision/BroadphaseCollision/btDispatcher.cpp \
    ../../../bullet/BulletCollision/BroadphaseCollision/btMultiSapBroadphase.cpp \
    ../../../bullet/BulletCollision/BroadphaseCollision/btOverlappingPairCache.cpp \
    ../../../bullet/BulletCollision/BroadphaseCollision/btQuantizedBvh.cpp \
    ../../../bullet/BulletCollision/BroadphaseCollision/btSimpleBroadphase.cpp \
    ../../../bullet/BulletCollision/CollisionDispatch/btActivatingCollisionAlgorithm.cpp \
    ../../../bullet/BulletCollision/CollisionDispatch/btBox2dBox2dCollisionAlgorithm.cpp \
    ../../../bullet/BulletCollision/CollisionDispatch/btBoxBoxCollisionAlgorithm.cpp \
    ../../../bullet/BulletCollision/CollisionDispatch/btBoxBoxDetector.cpp \
    ../../../bullet/BulletCollision/CollisionDispatch/btCollisionDispatcher.cpp \
    ../../../bullet/BulletCollision/CollisionDispatch/btCollisionObject.cpp \
    ../../../bullet/BulletCollision/CollisionDispatch/btCollisionWorld.cpp \
    ../../../bullet/BulletCollision/CollisionDispatch/btCompoundCollisionAlgorithm.cpp \
    ../../../bullet/BulletCollision/CollisionDispatch/btCompoundCompoundCollisionAlgorithm.cpp \
    ../../../bullet/BulletCollision/CollisionDispatch/btConvex2dConvex2dAlgorithm.cpp \
    ../../../bullet/BulletCollision/CollisionDispatch/btConvexConcaveCollisionAlgorithm.cpp \
    ../../../bullet/BulletCollision/CollisionDispatch/btConvexConvexAlgorithm.cpp \
    ../../../bullet/BulletCollision/CollisionDispatch/btConvexPlaneCollisionAlgorithm.cpp \
    ../../../bullet/BulletCollision/CollisionDispatch/btDefaultCollisionConfiguration.cpp \
    ../../../bullet/BulletCollision/CollisionDispatch/btEmptyCollisionAlgorithm.cpp \
    ../../../bullet/BulletCollision/CollisionDispatch/btGhostObject.cpp \
    ../../../bullet/BulletCollision/CollisionDispatch/btHashedSimplePairCache.cpp \
    ../../../bullet/BulletCollision/CollisionDispatch/btInternalEdgeUtility.cpp \
    ../../../bullet/BulletCollision/CollisionDispatch/btManifoldResult.cpp \
    ../../../bullet/BulletCollision/CollisionDispatch/btSimulationIslandManager.cpp \
    ../../../bullet/BulletCollision/CollisionDispatch/btSphereBoxCollisionAlgorithm.cpp \
    ../../../bullet/BulletCollision/CollisionDispatch/btSphereSphereCollisionAlgorithm.cpp \
    ../../../bullet/BulletCollision/CollisionDispatch/btSphereTriangleCollisionAlgorithm.cpp \
    ../../../bullet/BulletCollision/CollisionDispatch/btUnionFind.cpp \
    ../../../bullet/BulletCollision/CollisionDispatch/SphereTriangleDetector.cpp \
    ../../../bullet/BulletCollision/CollisionShapes/btBox2dShape.cpp \
    ../../../bullet/BulletCollision/CollisionShapes/btBoxShape.cpp \
    ../../../bullet/BulletCollision/CollisionShapes/btBvhTriangleMeshShape.cpp \
    ../../../bullet/BulletCollision/CollisionShapes/btCapsuleShape.cpp \
    ../../../bullet/BulletCollision/CollisionShapes/btCollisionShape.cpp \
    ../../../bullet/BulletCollision/CollisionShapes/btCompoundShape.cpp \
    ../../../bullet/BulletCollision/CollisionShapes/btConcaveShape.cpp \
    ../../../bullet/BulletCollision/CollisionShapes/btConeShape.cpp \
    ../../../bullet/BulletCollision/CollisionShapes/btConvex2dShape.cpp \
    ../../../bullet/BulletCollision/CollisionShapes/btConvexHullShape.cpp \
    ../../../bullet/BulletCollision/CollisionShapes/btConvexInternalShape.cpp \
    ../../../bullet/BulletCollision/CollisionShapes/btConvexPointCloudShape.cpp \
    ../../../bullet/BulletCollision/CollisionShapes/btConvexPolyhedron.cpp \
    ../../../bullet/BulletCollision/CollisionShapes/btConvexShape.cpp \
    ../../../bullet/BulletCollision/CollisionShapes/btConvexTriangleMeshShape.cpp \
    ../../../bullet/BulletCollision/CollisionShapes/btCylinderShape.cpp \
    ../../../bullet/BulletCollision/CollisionShapes/btEmptyShape.cpp \
    ../../../bullet/BulletCollision/CollisionShapes/btHeightfieldTerrainShape.cpp \
    ../../../bullet/BulletCollision/CollisionShapes/btMinkowskiSumShape.cpp \
    ../../../bullet/BulletCollision/CollisionShapes/btMultimaterialTriangleMeshShape.cpp \
    ../../../bullet/BulletCollision/CollisionShapes/btMultiSphereShape.cpp \
    ../../../bullet/BulletCollision/CollisionShapes/btOptimizedBvh.cpp \
    ../../../bullet/BulletCollision/CollisionShapes/btPolyhedralConvexShape.cpp \
    ../../../bullet/BulletCollision/CollisionShapes/btScaledBvhTriangleMeshShape.cpp \
    ../../../bullet/BulletCollision/CollisionShapes/btShapeHull.cpp \
    ../../../bullet/BulletCollision/CollisionShapes/btSphereShape.cpp \
    ../../../bullet/BulletCollision/CollisionShapes/btStaticPlaneShape.cpp \
    ../../../bullet/BulletCollision/CollisionShapes/btStridingMeshInterface.cpp \
    ../../../bullet/BulletCollision/CollisionShapes/btTetrahedronShape.cpp \
    ../../../bullet/BulletCollision/CollisionShapes/btTriangleBuffer.cpp \
    ../../../bullet/BulletCollision/CollisionShapes/btTriangleCallback.cpp \
    ../../../bullet/BulletCollision/CollisionShapes/btTriangleIndexVertexArray.cpp \
    ../../../bullet/BulletCollision/CollisionShapes/btTriangleIndexVertexMaterialArray.cpp \
    ../../../bullet/BulletCollision/CollisionShapes/btTriangleMesh.cpp \
    ../../../bullet/BulletCollision/CollisionShapes/btTriangleMeshShape.cpp \
    ../../../bullet/BulletCollision/CollisionShapes/btUniformScalingShape.cpp \
    ../../../bullet/BulletCollision/Gimpact/btContactProcessing.cpp \
    ../../../bullet/BulletCollision/Gimpact/btGenericPoolAllocator.cpp \
    ../../../bullet/BulletCollision/Gimpact/btGImpactBvh.cpp \
    ../../../bullet/BulletCollision/Gimpact/btGImpactCollisionAlgorithm.cpp \
    ../../../bullet/BulletCollision/Gimpact/btGImpactQuantizedBvh.cpp \
    ../../../bullet/BulletCollision/Gimpact/btGImpactShape.cpp \
    ../../../bullet/BulletCollision/Gimpact/btTriangleShapeEx.cpp \
    ../../../bullet/BulletCollision/Gimpact/gim_box_set.cpp \
    ../../../bullet/BulletCollision/Gimpact/gim_contact.cpp \
    ../../../bullet/BulletCollision/Gimpact/gim_memory.cpp \
    ../../../bullet/BulletCollision/Gimpact/gim_tri_collision.cpp \
    ../../../bullet/BulletCollision/NarrowPhaseCollision/btContinuousConvexCollision.cpp \
    ../../../bullet/BulletCollision/NarrowPhaseCollision/btConvexCast.cpp \
    ../../../bullet/BulletCollision/NarrowPhaseCollision/btGjkConvexCast.cpp \
    ../../../bullet/BulletCollision/NarrowPhaseCollision/btGjkEpa2.cpp \
    ../../../bullet/BulletCollision/NarrowPhaseCollision/btGjkEpaPenetrationDepthSolver.cpp \
    ../../../bullet/BulletCollision/NarrowPhaseCollision/btGjkPairDetector.cpp \
    ../../../bullet/BulletCollision/NarrowPhaseCollision/btMinkowskiPenetrationDepthSolver.cpp \
    ../../../bullet/BulletCollision/NarrowPhaseCollision/btPersistentManifold.cpp \
    ../../../bullet/BulletCollision/NarrowPhaseCollision/btPolyhedralContactClipping.cpp \
    ../../../bullet/BulletCollision/NarrowPhaseCollision/btRaycastCallback.cpp \
    ../../../bullet/BulletCollision/NarrowPhaseCollision/btSubSimplexConvexCast.cpp \
    ../../../bullet/BulletCollision/NarrowPhaseCollision/btVoronoiSimplexSolver.cpp \
    ../../../bullet/BulletDynamics/Character/btKinematicCharacterController.cpp \
    ../../../bullet/BulletDynamics/ConstraintSolver/btConeTwistConstraint.cpp \
    ../../../bullet/BulletDynamics/ConstraintSolver/btContactConstraint.cpp \
    ../../../bullet/BulletDynamics/ConstraintSolver/btFixedConstraint.cpp \
    ../../../bullet/BulletDynamics/ConstraintSolver/btGearConstraint.cpp \
    ../../../bullet/BulletDynamics/ConstraintSolver/btGeneric6DofConstraint.cpp \
    ../../../bullet/BulletDynamics/ConstraintSolver/btGeneric6DofSpringConstraint.cpp \
    ../../../bullet/BulletDynamics/ConstraintSolver/btHinge2Constraint.cpp \
    ../../../bullet/BulletDynamics/ConstraintSolver/btHingeConstraint.cpp \
    ../../../bullet/BulletDynamics/ConstraintSolver/btPoint2PointConstraint.cpp \
    ../../../bullet/BulletDynamics/ConstraintSolver/btSequentialImpulseConstraintSolver.cpp \
    ../../../bullet/BulletDynamics/ConstraintSolver/btSliderConstraint.cpp \
    ../../../bullet/BulletDynamics/ConstraintSolver/btSolve2LinearConstraint.cpp \
    ../../../bullet/BulletDynamics/ConstraintSolver/btTypedConstraint.cpp \
    ../../../bullet/BulletDynamics/ConstraintSolver/btUniversalConstraint.cpp \
    ../../../bullet/BulletDynamics/Dynamics/btDiscreteDynamicsWorld.cpp \
    ../../../bullet/BulletDynamics/Dynamics/btRigidBody.cpp \
    ../../../bullet/BulletDynamics/Dynamics/btSimpleDynamicsWorld.cpp \
    ../../../bullet/BulletDynamics/Dynamics/Bullet-C-API.cpp \
    ../../../bullet/BulletDynamics/Featherstone/btMultiBody.cpp \
    ../../../bullet/BulletDynamics/Featherstone/btMultiBodyConstraint.cpp \
    ../../../bullet/BulletDynamics/Featherstone/btMultiBodyConstraintSolver.cpp \
    ../../../bullet/BulletDynamics/Featherstone/btMultiBodyDynamicsWorld.cpp \
    ../../../bullet/BulletDynamics/Featherstone/btMultiBodyJointLimitConstraint.cpp \
    ../../../bullet/BulletDynamics/Featherstone/btMultiBodyJointMotor.cpp \
    ../../../bullet/BulletDynamics/Featherstone/btMultiBodyPoint2Point.cpp \
    ../../../bullet/BulletDynamics/MLCPSolvers/btDantzigLCP.cpp \
    ../../../bullet/BulletDynamics/MLCPSolvers/btMLCPSolver.cpp \
    ../../../bullet/BulletDynamics/Vehicle/btRaycastVehicle.cpp \
    ../../../bullet/BulletDynamics/Vehicle/btWheelInfo.cpp \
    ../../../bullet/BulletFileLoader/bChunk.cpp \
    ../../../bullet/BulletFileLoader/bDNA.cpp \
    ../../../bullet/BulletFileLoader/bFile.cpp \
    ../../../bullet/BulletFileLoader/btBulletFile.cpp \
    ../../../bullet/BulletWorldImporter/btBulletWorldImporter.cpp \
    ../../../bullet/BulletWorldImporter/btWorldImporter.cpp \
    ../../../bullet/LinearMath/btAlignedAllocator.cpp \
    ../../../bullet/LinearMath/btConvexHull.cpp \
    ../../../bullet/LinearMath/btConvexHullComputer.cpp \
    ../../../bullet/LinearMath/btGeometryUtil.cpp \
    ../../../bullet/LinearMath/btPolarDecomposition.cpp \
    ../../../bullet/LinearMath/btQuickprof.cpp \
    ../../../bullet/LinearMath/btSerializer.cpp \
    ../../../bullet/LinearMath/btVector3.cpp

LOCAL_EXPORT_LDLIBS    := -llog -landroid -lEGL -lGLESv1_CM -lz -lOpenSLES 
LOCAL_CFLAGS += -DIDE_ANDROID -O3 -DUSE_AXTLS -DHAVE_CONFIG_H -fsigned-char
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


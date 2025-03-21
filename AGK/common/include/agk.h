// Include all GDK command libraries

#ifndef _H_AGK_
#define _H_AGK_

	#include "Globals.h"
	#include "AppErrors.h"
	#include "Common.h"

	#include "sha1.h"
	#include "sha2.h"
	#include "agkKeys.h"
	#include "cHashedList.h"
	#include "uString.h"
	#include "cImage.h"
	#include "cSprite.h"
	#include "cSpriteMgr.h"
	#include "cTouch.h"
	#include "cEvent.h"
	#include "cSound.h"
	#include "cText.h"
	#include "AGKFont.h"
	#include "Physics.h"
	#include "cForce.h"
	#include "Thread.h"
	#include "Network.h"
	#include "NetworkCommon.h"
	#include "cFile.h"
	#include "Particles.h"
	#include "Joystick.h"
	#include "Mouse.h"
	#include "cTextEntry.h"
	#include "ZipFile.h"

	#include "AGKArrays.h"
	#include "AGO.h"
	#include "3DMath.h"
	#include "AGKShader.h"
	#include "cNode.h"
	#include "cMesh.h"
	#include "cObject3D.h"
	#include "Skeleton3D.h"
	#include "cCamera.h"
	#include "AGKLights.h"
	#include "../Collision/CollisionIncludes.h"
	#include "cObjectMgr.h"
	#include "FrameBuffer.h"
	#include "JSONElement.h"
	#include "Skeleton2D.h"
	#include "Tween.h"
	#include "AGKSkyBox.h"
	#include "3DParticles.h"

	#include "Renderer.h"

	//Bullet Library
	#include "../../bullet/btBulletDynamicsCommon.h"
	#include "../../bullet/btBulletCollisionCommon.h"
	#include "../../bullet/BulletCollision/CollisionDispatch/btInternalEdgeUtility.h"
	#include "../../bullet/BulletCollision/CollisionShapes/btShapeHull.h"
	#include "../../bullet/BulletWorldImporter/btBulletWorldImporter.h"
	#include "../../bullet/BulletCollision/NarrowPhaseCollision/btRaycastCallback.h"
	#include "../../bullet/BulletCollision/CollisionDispatch/btGhostObject.h"
	#include "../../bullet/BulletDynamics/Character/btCharacterControllerInterface.h"
	#include "../../bullet/BulletDynamics/Character/btKinematicCharacterController.h"

	//3D Physics 
    #include "ContactReport.h"
	#include "BaseItem.h"
	#include "Joint.h"
	#include "Joints.h"
	#include "StaticPlane.h"
	#include "Vector.h"
	#include "RigidBody.h"
	#include "RagdollBone.h"
	#include "RagDoll.h"
	#include "CharacterController.h"

	#include "Ray.h"
	#include "RigidBodies.h"
    #include "AGKToBullet.h"
	#include "GLDebugDrawer.h"
	#include "DynamicsWorld.h"
	#include "AGKMotionState.h"
	#include "CollisionShapes.h"

	#include "Wrapper.h"

	using namespace AGK;

#endif


// Bullet Physics Wrapper for App Game Kit
//Stab In The Dark Software 
//Copyright (c) 2002-2015
//http://gonefishing@stabinthedarksoftware.com 

#include "agk.h"
#include <math.h>



CharacterController::CharacterController(int objectID, int debugObjectID, int debugObjectID_Duck, btPairCachingGhostObject* ghostObject,btConvexShape* convexShape, btConvexShape* convexShapeDuck,btScalar stepHeight, const btVector3& objectOffset, int upAxis /*= 1*/)
	: btKinematicCharacterController(ghostObject, convexShape, stepHeight, upAxis), BaseItem( objectID )
{
	m_currentPosition = btVector3(0,0,0);
	m_addedMargin = 0.02f;
	m_debugObjectID = debugObjectID;
	m_debugObjectID_Crouch = debugObjectID_Duck;
	m_walkVelocity = ((btScalar)1.0) / GetCurrentDynamicsWorld()->m_scaleFactor;
	m_mass = 45.0;
	m_standingConvexShape = convexShape;
	m_crouchConvexShape = convexShapeDuck;
	m_objectOffset = objectOffset;
	btVector3 objSizeVec = AGKToBullet::GetObjectSize( debugObjectID ) / 2.0 / GetCurrentDynamicsWorld()->m_scaleFactor;
	m_halfHeight = objSizeVec.getY();
	m_yAngleRotation.setIdentity();
	m_isStanding = true;
	m_isDebugMode = false;
}

CharacterController::~CharacterController()
{
	agk::DeleteObject( m_debugObjectID );
	agk::DeleteObject( m_debugObjectID_Crouch );
	//http://www.bulletphysics.org/Bullet/phpBB3/viewtopic.php?f=9&t=4611
	m_ghostObject->getOverlappingPairCache()->cleanProxyFromPairs(m_ghostObject->getBroadphaseHandle(),GetCurrentDynamicsWorld()->m_dynamicsWorld->getDispatcher());
	//Note: Reset needs to be called before removing or deleteing the ghost object.
	reset( GetCurrentDynamicsWorld()->m_dynamicsWorld );
	GetCurrentDynamicsWorld()->m_dynamicsWorld->removeCollisionObject(m_ghostObject);
	delete m_ghostObject;
	delete m_standingConvexShape ;
	delete m_crouchConvexShape ;
	GetCurrentDynamicsWorld()->m_dynamicsWorld->removeAction(this);
}

void CharacterController::Create( int objectID, int axis, const btVector3& objectOffset, const btVector3& objOrientation, btScalar crouchScale )
{
	btScalar scaleFactor = GetCurrentDynamicsWorld()->m_scaleFactor;
	btConvexShape* convexShape;
	btConvexShape* convexShapeDuck;
	
	btVector3 objSizeVec = AGKToBullet::GetObjectSize( objectID );
	btScalar width = objSizeVec.getX();
	btScalar height = objSizeVec.getY();
	btScalar depth = objSizeVec.getZ();
	btScalar diameter = width;
	diameter = width > depth ? width : depth;

	int debugObjectID = agk::CreateObjectCapsule( diameter, height, axis );
	agk::SetObjectVisible( debugObjectID, false );
	int debugObjectID_Crouch = agk::CreateObjectCapsule( diameter, height * crouchScale, axis );
	agk::SetObjectVisible(debugObjectID_Crouch, false );

	agk::SetObjectPosition( debugObjectID, agk::GetObjectX( objectID ), agk::GetObjectY( objectID ), agk::GetObjectZ( objectID ) );
	agk::SetObjectPosition(debugObjectID_Crouch, agk::GetObjectX(objectID), agk::GetObjectY(objectID), agk::GetObjectZ(objectID));

	//Rotate the Model to point down the positive z axis.
	if ( objOrientation.getZ() == -1.0 )
		agk::SetObjectRotation( objectID, 0.0, 180.0, 0.0 );
	if ( objOrientation.getX() == -1.0 )	
		agk::SetObjectRotation( objectID, 0.0, 90.0, 0.0 );
	if ( objOrientation.getX() == 1.0 )
		agk::SetObjectRotation( objectID, 0.0, -90.0, 0.0 );
	
	btTransform debugObjTrans = AGKToBullet::GetTransform( debugObjectID, scaleFactor );

	if ( axis == Z_AXIS )
	{
		convexShape = new btCapsuleShapeZ(btScalar(diameter/scaleFactor/2.0),btScalar((height - diameter)/scaleFactor));
		convexShapeDuck = new btCapsuleShapeZ(btScalar(diameter/scaleFactor/2.0),btScalar((height * crouchScale - diameter)/scaleFactor));
	}
	if ( axis == X_AXIS )
	{
		convexShape = new btCapsuleShapeX(btScalar(diameter/scaleFactor/2.0),btScalar((height - diameter)/scaleFactor));
		convexShapeDuck = new btCapsuleShapeX(btScalar(diameter/scaleFactor/2.0),btScalar((height * crouchScale - diameter)/scaleFactor));
	}
	if ( axis == Y_AXIS )
	{
		convexShape = new btCapsuleShape(btScalar(diameter/scaleFactor/2.0),btScalar((height - diameter)/scaleFactor));
		convexShapeDuck = new btCapsuleShape(btScalar(diameter/scaleFactor/2.0),btScalar((height * crouchScale - diameter)/scaleFactor));
	}

	btPairCachingGhostObject* ghostObj = new btPairCachingGhostObject();
    ghostObj->setWorldTransform( debugObjTrans );
    ghostObj->setCollisionShape ( convexShape );
	ghostObj->setCollisionFlags ( btCollisionObject::CF_CHARACTER_OBJECT );
    ghostObj->setFriction(0.0);
    ghostObj->setRestitution(0.0);
	
	CharacterController* controller = new CharacterController( objectID, debugObjectID, debugObjectID_Crouch, 
																								ghostObj, convexShape, convexShapeDuck,
																									(btScalar)2.0/GetCurrentDynamicsWorld()->m_scaleFactor, objectOffset);
	btVector3 initialRot = btVector3( agk::GetObjectWorldQuatX(objectID) , 
																agk::GetObjectWorldQuatY(objectID) , agk::GetObjectWorldQuatZ(objectID) );
	initialRot.setW(agk::GetObjectWorldQuatW(objectID) );
	controller->SetObjectInitialRotation( initialRot );
	controller->setMaxJumpHeight(height/scaleFactor);
	controller->SetCrouchScale(crouchScale);
	GetCurrentDynamicsWorld()->m_dynamicsWorld->addCollisionObject( ghostObj, 
																btBroadphaseProxy::CharacterFilter | btBroadphaseProxy::StaticFilter | btBroadphaseProxy::KinematicFilter, 
																	btBroadphaseProxy::AllFilter);
	GetCurrentDynamicsWorld()->m_dynamicsWorld->addAction( controller );
	characterControllerManager.AddItem( controller, objectID );
}

int CharacterController::GetDebugObjectID()
{
	return m_debugObjectID;
}

btScalar CharacterController::GetInvMass()
{
	return ((btScalar)1.0) / m_mass;
}

void CharacterController::SetCrouchScale(btScalar crouchScale)
{
	m_crouchScale = crouchScale;
}

void CharacterController::Update(btScalar elapsedTime)
{
	if ( GetCurrentDynamicsWorld()->m_dynamicsWorld && elapsedTime != 0 )
	{
		setVelocityForTimeInterval( m_walkDirection, elapsedTime / 1000.0f );
		AGKMotionState::setWorldTransform( m_debugObjectID_Crouch, getGhostObject()->getWorldTransform() );
		AGKMotionState::setWorldTransform( m_debugObjectID, getGhostObject()->getWorldTransform() );
		btTransform xForm;
		xForm.setIdentity();
		xForm.setBasis(getGhostObject()->getWorldTransform().getBasis());
		if(m_isCrouching == true)
		{
			xForm.setOrigin(getGhostObject()->getWorldTransform().getOrigin() - m_objectOffset * m_crouchScale/ GetCurrentDynamicsWorld()->m_scaleFactor);
		}
		else if(m_isStanding == true)
		{
			xForm.setOrigin(getGhostObject()->getWorldTransform().getOrigin() - m_objectOffset / GetCurrentDynamicsWorld()->m_scaleFactor);
		}
		AGKMotionState::setWorldTransform( m_id, xForm, m_objectInitialRotation );
	}
}

void CharacterController::SetWalkVelocity(btScalar walkVelocity)
{
	m_walkVelocity = walkVelocity;
}

void CharacterController::setWalkDirection(const btVector3& walkDirection)
{
	m_walkDirection = (walkDirection * m_walkVelocity) * m_yAngleRotation.inverse();
}

void CharacterController::playerStep(btCollisionWorld * collisionWorld, btScalar dt)
{
	if (!m_useWalkDirection &&  m_velocityTimeInterval <= 0.0)
		return;
	m_wasOnGround = onGround();

	m_verticalVelocity -= m_gravity * dt;

	if (m_verticalVelocity > 0 && m_verticalVelocity > m_jumpSpeed)
		m_verticalVelocity = m_jumpSpeed;

	if (m_verticalVelocity < 0 && btFabs(m_verticalVelocity) > btFabs(m_fallSpeed))
		m_verticalVelocity = -btFabs(m_fallSpeed);

	m_verticalOffset = m_verticalVelocity * dt;

	btTransform xform;
	xform = m_ghostObject->getWorldTransform();

	stepUp(collisionWorld);

	if ( m_useWalkDirection )
		stepForwardAndStrafe(collisionWorld, m_walkDirection);
	else{
		btScalar dtMoving = (dt < m_velocityTimeInterval) ? dt : m_velocityTimeInterval;
		m_velocityTimeInterval -= dt;
		btVector3 move = m_walkDirection * dtMoving;
		stepForwardAndStrafe(collisionWorld, move);
	}
	stepDown(collisionWorld, dt);

	xform.setOrigin(m_currentPosition);
	m_ghostObject->setWorldTransform(xform);

	if (!m_canStand)
		Stand(); 
}

void CharacterController::SetStepHeight(btScalar stepHeight)
{
	m_stepHeight = stepHeight;
}

void CharacterController::SetCrouchConvexShape(btConvexShape * shape)
{
	m_crouchConvexShape = shape;
}

void CharacterController::Crouch()
{
	m_convexShape = m_crouchConvexShape;
	m_ghostObject->setCollisionShape(m_convexShape);
	if (m_isDebugMode )
	{
		agk::SetObjectVisible( m_debugObjectID, false );
		agk::SetObjectVisible(m_debugObjectID_Crouch, true );
	}
	else{
		agk::SetObjectVisible( m_debugObjectID_Crouch , false);
		agk::SetObjectVisible( m_debugObjectID, false );
	}
    if (onGround() && m_isStanding)
	{
		btTransform xform;
		xform.setIdentity();
		xform.setOrigin(m_currentPosition - btVector3(0.0, m_halfHeight - ( m_halfHeight * m_crouchScale), 0.0));
		m_ghostObject->setWorldTransform(xform);
	}
	m_isCrouching = true;
	m_isStanding = false;
}

void CharacterController::Stand()
{
	if (!CanStand())
		return;
	if (m_isDebugMode )
	{
		agk::SetObjectVisible( m_debugObjectID, true );
		agk::SetObjectVisible( m_debugObjectID_Crouch , false);
	}
	else{
		agk::SetObjectVisible( m_debugObjectID_Crouch , false);
		agk::SetObjectVisible( m_debugObjectID, false );
	}

	m_convexShape = m_standingConvexShape;
	m_ghostObject->setCollisionShape(m_convexShape);

    if (onGround() && m_isCrouching)
	{
		btTransform xform;
		xform.setIdentity();
		xform.setOrigin(m_currentPosition + btVector3( 0.0, m_halfHeight - ( m_halfHeight * m_crouchScale), 0.0 ) );
		m_ghostObject->setWorldTransform( xform );
	}
	m_isStanding = true;
	m_isCrouching = false;
}

bool CharacterController::CanStand()
{
	btTransform start, end;
	start.setIdentity();
	end.setIdentity();
	start.setOrigin(m_currentPosition);
	end.setOrigin(m_currentPosition + btVector3(0, static_cast<btCapsuleShape*>(m_standingConvexShape)->getHalfHeight() * 2 - static_cast<btCapsuleShape*>(m_crouchConvexShape)->getHalfHeight() * 2, 0));
	btKinematicClosestNotMeConvexResultCallback callback(m_ghostObject, -getUpAxisDirections()[m_upAxis], 0);
	callback.m_collisionFilterGroup = getGhostObject()->getBroadphaseHandle()->m_collisionFilterGroup;
	callback.m_collisionFilterMask = getGhostObject()->getBroadphaseHandle()->m_collisionFilterMask;

	if (m_useGhostObjectSweepTest)
		m_ghostObject->convexSweepTest(m_convexShape, start, end, callback, GetCurrentDynamicsWorld()->m_dynamicsWorld->getDispatchInfo().m_allowedCcdPenetration);
	else
		GetCurrentDynamicsWorld()->m_dynamicsWorld->convexSweepTest(m_convexShape, start, end, callback);

	if (callback.hasHit() )
	{
		m_canStand = false;
		return false;
	}
	else
	{
		m_canStand = true;
		return true;
	}
}

void CharacterController::SetObjectInitialRotation( const btVector3& rotation )
{
	m_objectInitialRotation = rotation;
}

void CharacterController::SetRotation( btMatrix3x3& rotation )
{
	m_yAngleRotation = rotation;
	m_ghostObject->getWorldTransform().setBasis( rotation );
}

void CharacterController::SetDebugMode( bool isDebug )
{
	m_isDebugMode = isDebug;
}

bool CharacterController::GetDebugMode()
{
	return m_isDebugMode;
}

void CharacterController::SetPosition( const btVector3& pos )
{
	m_ghostObject->getWorldTransform().setOrigin( pos );
}


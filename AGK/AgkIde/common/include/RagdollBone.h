
// Bullet Physics Wrapper for App Game Kit
//Stab In The Dark Software 
//Copyright (c) 2002-2015
//http://gonefishing@stabinthedarksoftware.com 

#pragma once

class RagDollBone
{
public:
	RagDollBone(int dbproObjectID, int dbproStartLimbID, int dbproEndLimbID, btScalar diameter, int collisionGroup, int collisionMask);
	~RagDollBone(void);
	btRigidBody* GetRigidBody();
	int GetEndLimbID();
	int GetStartLimbID();
	int GetObjectID();
	int GetRagDollBoneID();
	btVector3 GetNormilizedVector();
	void AddJointConstraint(int jointID, int jointType);
	void AddLimbID(int dbproLimbID);

	btMatrix3x3 initialRotation;
	btAlignedObjectArray <int> m_limbIDs;
	btAlignedObjectArray <btVector3> m_limbOffsets;
	btAlignedObjectArray <btMatrix3x3> m_limbInitalRotation;
	btAlignedObjectArray <btVector3> m_centerOfObjectOffsets;
	btScalar m_boneVolume;

private:
	int m_objectID;
	int m_startLimbID;
	int m_endLimbID;
	int m_ragDollBoneID;
	btScalar m_diameter;
	int m_collisionGroup;
	int m_collisionMask;
	btVector3 m_boneNormVec;  
	btRigidBody* m_rigidBody;
	btTypedConstraint* m_jointConstraint;
	btCollisionShape* m_collisionShape;

private:
	void CreateBone();
	btRigidBody* localCreateRigidBody (btScalar mass, const btTransform& startTransform, btCollisionShape* shape, int objID, int collisionGroup, int collisionMask);
};
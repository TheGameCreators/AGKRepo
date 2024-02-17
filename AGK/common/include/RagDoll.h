
// Bullet Physics Wrapper for App Game Kit
//Stab In The Dark Software 
//Copyright (c) 2002-2015
//http://gonefishing@stabinthedarksoftware.com 

#pragma once

class RagDoll : public BaseItem
{
public:
	RagDoll(int ragdollID);
	virtual ~RagDoll();
	btRigidBody* localCreateRigidBody (btScalar mass, const btTransform& startTransform, btCollisionShape* shape, int objID, int collisionGroup, int collisionMask);
	int AddBone(int objID, int limbID1, int LimbID2, btScalar diameter, int collisionGroup, int collisionMask);
	void AddJoint( int boneIndex1, int boneIndex2, int jointType, int limbID);
	void AddHingeJoint(int boneIndex1, int boneIndex2, int limbID, const btVector3& jointRotation, btScalar minRotation, btScalar maxRotation);
	void AddTwistConstraint(int boneIndex1, int boneIndex2, int limbID, const btVector3& jointRotation, const btVector3& jointlimits);
	void Finalize();
	void Update();
	void HideBones();
	void DisableDeactivation(int isDisabled);
	void ShowBones();
	int GetBoneObjID(int boneIndex);
	void AssignLimbIDToBone(int boneIndex, int agkBoneID );
	void SetDamping(btScalar linear, btScalar angular);
	void SetSleepingThresholds(btScalar linear, btScalar angular);
	void SetDeactivationTime(btScalar time); 
	void ResetObjectParametersForAnimation();
	bool IsSleeping();
	void SetStatic(bool bStatic);
	void SetTotalWeight(btScalar totalWeight); 
	bool IsStatic(); 
	bool IsBoneObject(int objectID);
	static bool AssertRagdollExist( int ragdollID, const char* message, bool bExist /*= true*/ );
	static int GetIDFromBoneObject(int objID);

private:
	btAlignedObjectArray<int> m_joints;
	btAlignedObjectArray<RagDollBone*> m_ragDollBoneArray;

	btScalar m_modelTotalWeight;
	btScalar m_modelTotalVolume;
	btScalar m_modelTotalDensity;
	btScalar m_linearDamping;
	btScalar m_angularDamping;
	btScalar m_linearSleepingThresholds;
	btScalar m_angularSleepingThresholds;
	btScalar m_deactivationTime; 
	bool m_bIsStatic;
	float m_fRadius;
	float m_fScaledRadius;
	float m_fScaledLargestRadius;
	bool m_bNeedsUpdateForCulling;
};

extern RagDoll* currentRagDoll;

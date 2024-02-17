
#include "RagdollUtil.h"
#include "DemoUtil.h"

struct TwistJoint
{
	int rotationVec3;
	int limitsVec3;
} neckJoint, L_ShoulderJoint, R_ShoulderJoint, hipJoint;

struct HingeJoint
{
	int rotationVec3;
	float minLimit;
	float maxLimit;
} waistJoint, kneeJoint, rightElbowJoint, leftElbowJoint, rightWristJoint, leftWristJoint;

void RagdollUtil::CreateRagdollVectors()
{
	kneeJoint.rotationVec3 = agk::CreateVector3( 90.0, 0.0, 0.0 );
	kneeJoint.minLimit = 0.0;
	kneeJoint.maxLimit = 120.0;

	rightElbowJoint.rotationVec3 = agk::CreateVector3( -120.0, 0.0, 0.0 );
	rightElbowJoint.minLimit = 0.0; 
	rightElbowJoint.maxLimit = 130.0;

	leftElbowJoint.rotationVec3 = agk::CreateVector3( 270.0, 0.0, 0.0 );
	leftElbowJoint.minLimit = 0.0;
	leftElbowJoint.maxLimit = 130.0;

	waistJoint.rotationVec3 = agk::CreateVector3( 90.0, 0.0, 0.0 );
	waistJoint.minLimit = 0.0;
	waistJoint.maxLimit = 160.0;

	hipJoint.rotationVec3 = agk::CreateVector3( 45.0, 45.0, 45.0 );
	hipJoint.limitsVec3 = agk::CreateVector3( 25.0, 25.0, 25.0 );

	R_ShoulderJoint.rotationVec3 = agk::CreateVector3( 0.0, 0.0, 0.0 );
	R_ShoulderJoint.limitsVec3 = agk::CreateVector3( 45.0,45.0,90.0 );

	L_ShoulderJoint.rotationVec3 = agk::CreateVector3( 0.0, 0.0, 0.0 );
	L_ShoulderJoint.limitsVec3 = agk::CreateVector3( 45.0,45.0,90.0 );

	neckJoint.rotationVec3 = agk::CreateVector3( -90.0, 0.0, 0.0 );
	neckJoint.limitsVec3 = agk::CreateVector3( 24.0, 11.5, 8.0 );
}


void RagdollUtil::CreateRagdollSkelton( int objID )
{
	//**************** Create Ragdoll ***************************
	//Create the ragdoll bones using the original objects bone positions then add all the original 
	//objects bones that will be moved by the ragdoll bone.Set collisions groups so bones won't collide with other bones.

	agk::StopObjectAnimation( objID );
	agk::Create3DPhysicsRagDoll( objID, 175.0 );

	int pelvis = agk::Add3DPhysicsRagDollBone( agk::GetObjectBoneByName( objID, "Crotch" ), 
																		agk::GetObjectBoneByName( objID, "Bip01_Spine" ), 10,
																			group0, mask0 | mask1 | mask2 );
	agk::AssignTo3DPhysicsRagDollBoneObjectBone( pelvis, agk::GetObjectBoneByName( objID, "Crotch" ) );
	agk::AssignTo3DPhysicsRagDollBoneObjectBone( pelvis, agk::GetObjectBoneByName( objID, "Bip01_Pelvis" ) );

	int torso = agk::Add3DPhysicsRagDollBone( agk::GetObjectBoneByName( objID, "Bip01_Spine" ), 
																		agk::GetObjectBoneByName( objID, "Bip01_Head" ), 12,
																			group0, mask0 | mask1 | mask2 );
	agk::AssignTo3DPhysicsRagDollBoneObjectBone( torso, agk::GetObjectBoneByName( objID, "Bip01_Spine" ) );
	agk::AssignTo3DPhysicsRagDollBoneObjectBone( torso, agk::GetObjectBoneByName( objID, "Bip01_Spine1" ) );
	agk::AssignTo3DPhysicsRagDollBoneObjectBone( torso, agk::GetObjectBoneByName( objID, "Bip01_Spine2" ) );
	agk::AssignTo3DPhysicsRagDollBoneObjectBone( torso, agk::GetObjectBoneByName( objID, "Bip01_Neck" ) );
	agk::AssignTo3DPhysicsRagDollBoneObjectBone( torso, agk::GetObjectBoneByName( objID, "Bip01_L_Clavicle" ) );
	agk::AssignTo3DPhysicsRagDollBoneObjectBone( torso, agk::GetObjectBoneByName( objID, "Bip01_R_Clavicle" ) );

	int head = agk::Add3DPhysicsRagDollBone( agk::GetObjectBoneByName( objID, "Bip01_Head" ), 
																		agk::GetObjectBoneByName( objID, "HeadTop" ), 8,
																			group0, mask0 | mask1 | mask2 );
	agk::AssignTo3DPhysicsRagDollBoneObjectBone( head, agk::GetObjectBoneByName( objID, "Bip01_Head" ) );
	agk::AssignTo3DPhysicsRagDollBoneObjectBone( head, agk::GetObjectBoneByName( objID, "HeadTop" ) );

	int L_UpperArm = agk::Add3DPhysicsRagDollBone( agk::GetObjectBoneByName( objID, "Bip01_L_UpperArm" ), 
																					agk::GetObjectBoneByName( objID, "Bip01_L_Forearm" ), 6,
																						group0, mask0 | mask1 | mask2 );
	agk::AssignTo3DPhysicsRagDollBoneObjectBone( L_UpperArm, agk::GetObjectBoneByName( objID, "Bip01_L_UpperArm" ) );

	int L_ForeArm = agk::Add3DPhysicsRagDollBone( agk::GetObjectBoneByName( objID, "Bip01_L_Forearm" ), 
																					agk::GetObjectBoneByName( objID, "Bip01_L_Hand" ), 5,
																						group0, mask0 | mask1 | mask2 );
	agk::AssignTo3DPhysicsRagDollBoneObjectBone( L_ForeArm, agk::GetObjectBoneByName( objID, "Bip01_L_Forearm" ) );

	int R_UpperArm = agk::Add3DPhysicsRagDollBone( agk::GetObjectBoneByName( objID, "Bip01_R_UpperArm" ), 
																					agk::GetObjectBoneByName( objID, "Bip01_R_Forearm" ), 6,
																						group0, mask0 | mask1 | mask2 );
	agk::AssignTo3DPhysicsRagDollBoneObjectBone( R_UpperArm, agk::GetObjectBoneByName( objID, "Bip01_R_UpperArm" ) );

	int R_ForeArm = agk::Add3DPhysicsRagDollBone( agk::GetObjectBoneByName( objID, "Bip01_R_Forearm" ), 
																					agk::GetObjectBoneByName( objID, "Bip01_R_Hand" ), 5,
																						group0, mask0 | mask1 | mask2 );
	agk::AssignTo3DPhysicsRagDollBoneObjectBone( R_ForeArm, agk::GetObjectBoneByName( objID, "Bip01_R_Forearm" ) );


	int L_Thigh = agk::Add3DPhysicsRagDollBone( agk::GetObjectBoneByName( objID, "Bip01_L_Thigh" ), 
																					agk::GetObjectBoneByName( objID, "Bip01_L_Calf" ), 7,
																						group0, mask0 | mask1 | mask2 );
	agk::AssignTo3DPhysicsRagDollBoneObjectBone( L_Thigh, agk::GetObjectBoneByName( objID, "Bip01_L_Thigh" ) );

	int L_Calf = agk::Add3DPhysicsRagDollBone( agk::GetObjectBoneByName( objID, "Bip01_L_Calf" ), 
																					agk::GetObjectBoneByName( objID, "Bip01_L_Foot" ), 5,
																						group0, mask0 | mask1 | mask2 );
	agk::AssignTo3DPhysicsRagDollBoneObjectBone( L_Calf, agk::GetObjectBoneByName( objID, "Bip01_L_Calf" ) );

	int R_Thigh = agk::Add3DPhysicsRagDollBone( agk::GetObjectBoneByName( objID, "Bip01_R_Thigh" ), 
																					agk::GetObjectBoneByName( objID, "Bip01_R_Calf" ), 7,
																						group0, mask0 | mask1 | mask2 );
	agk::AssignTo3DPhysicsRagDollBoneObjectBone( R_Thigh, agk::GetObjectBoneByName( objID, "Bip01_R_Thigh" ) );

	int R_Calf = agk::Add3DPhysicsRagDollBone( agk::GetObjectBoneByName( objID, "Bip01_R_Calf" ), 
																					agk::GetObjectBoneByName( objID, "Bip01_R_Foot" ), 5,
																						group0, mask0 | mask1 | mask2 );
	agk::AssignTo3DPhysicsRagDollBoneObjectBone( R_Calf, agk::GetObjectBoneByName( objID, "Bip01_R_Calf" ) );

	//Now Create Joints between ragdoll bones

	agk::Add3DPhysicsRagDollTwistJoint( torso, head, agk::GetObjectBoneByName( objID, "Bip01_Head" ),
																agk::CreateVector3( 90.0, 0.0, 90.0 ), agk::CreateVector3( 24.0, 11.5, 8.0 ) );

	agk::Add3DPhysicsRagDollHingeJoint( pelvis, torso, agk::GetObjectBoneByName( objID, "Bip01_Spine" ),
																waistJoint.rotationVec3, waistJoint.minLimit, waistJoint.maxLimit );

	agk::Add3DPhysicsRagDollTwistJoint( torso, L_UpperArm, agk::GetObjectBoneByName( objID, "Bip01_L_UpperArm" ),
																L_ShoulderJoint.rotationVec3, L_ShoulderJoint.limitsVec3 );
	agk::Add3DPhysicsRagDollTwistJoint( torso, R_UpperArm, agk::GetObjectBoneByName( objID, "Bip01_R_UpperArm" ),
																R_ShoulderJoint.rotationVec3, R_ShoulderJoint.limitsVec3 );

	agk::Add3DPhysicsRagDollHingeJoint( L_UpperArm, L_ForeArm, agk::GetObjectBoneByName( objID, "Bip01_L_Forearm" ),
																leftElbowJoint.rotationVec3, leftElbowJoint.minLimit, leftElbowJoint.maxLimit );
	 
	agk::Add3DPhysicsRagDollHingeJoint( R_UpperArm, R_ForeArm, agk::GetObjectBoneByName( objID, "Bip01_R_Forearm" ),
																rightElbowJoint.rotationVec3, rightElbowJoint.minLimit, rightElbowJoint.maxLimit );

	agk::Add3DPhysicsRagDollTwistJoint( pelvis, L_Thigh, agk::GetObjectBoneByName( objID, "Bip01_L_Thigh" ),
																hipJoint.rotationVec3, hipJoint.limitsVec3 );
	agk::Add3DPhysicsRagDollTwistJoint( pelvis, R_Thigh, agk::GetObjectBoneByName( objID, "Bip01_R_Thigh" ),
																hipJoint.rotationVec3, hipJoint.limitsVec3 );

	agk::Add3DPhysicsRagDollHingeJoint( L_Thigh, L_Calf, agk::GetObjectBoneByName( objID, "Bip01_L_Calf" ),
																kneeJoint.rotationVec3, kneeJoint.minLimit, kneeJoint.maxLimit );
	agk::Add3DPhysicsRagDollHingeJoint( R_Thigh, R_Calf, agk::GetObjectBoneByName( objID, "Bip01_R_Calf" ),
																kneeJoint.rotationVec3, kneeJoint.minLimit, kneeJoint.maxLimit );

	agk::Finalize3DPhysicsRagDoll();

}




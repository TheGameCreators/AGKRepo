
#pragma once
#include "AGK.h"


struct Vertex
{
	float x;
    float y;
    float z;
    float u;
    float v;
    int vertexNumber;
};
enum CollisionGroups
{
	group0 = 1,
	group1 = 2,
	group2 = 4,
	group3 = 8,
	group4 = 16,
	group5 = 32
};

enum CollisionMasks
{
	mask0 = 1,
	mask1 = 2,
	mask2 = 4,
	mask3 = 8,
	mask4 = 16,
	mask5 = 32
};


extern int floorTex;
extern int ballTex;
extern int crateTex; 
extern int concreteTex;
extern int dumpsterTex;

extern int testBall;
extern int HitObjID;
extern int pickJoint;
extern int PickVec;

extern float WalkVelocity;
extern float RunVelocity;
extern float rotationInc;
extern float finalRotation;


extern float rawMouseX;
extern float rawMouseY;
extern int lockMouse;
extern int mouseSet;
extern float cameraAngleX;
extern float cameraAngleY;
extern float oldCamAngleX;
extern float oldCamAngleY;

class DemoUtil
{

public:
	static void Setup();
	static void CreateBoxStack(float boxSize, int rows, int columns);
	static void ThrowBall( float initialSpeed, float mass );
	static void DropCylinder();
	static void DropCone();
	static void DropCapsule();
	static void DropConvexHull();
	static int loadTerrain( char heightMap[128], char sdTexture[128], char hdTexture[128], float detailX, float detailY, float MaxHeight,int stepSize );
	static void PickJointCreateAndUpdate();
	static void ControlKinematicObjectWithArrowKeys( int objID );
	static void CreateCharacterController( int objID );
    static void CharacterControllerInput( int objID );
	static void ControlCameraWithArrowKeys( int cameraID );
	static void RotateCameraWithMouse( int cameraID, float speed );
	static float WrapAngle( float angle );
	static float CurveValue(float a, float da, float sp);
	static float CurveAngle( float destination, float current, float speed );
	

};


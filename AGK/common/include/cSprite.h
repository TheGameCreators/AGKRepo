// Sprite Header
#ifndef _H_SPRITE_
#define _H_SPRITE_

// Common includes
#include "Common.h"
#include "uString.h"
#include "Renderer.h"
#include "cMesh.h"

#define MAX_SPRITES 0x7fffffff

// Sprite status flags
#define AGK_SPRITE_VISIBLE						0x00000001
#define AGK_SPRITE_ACTIVE						0x00000002
#define AGK_SPRITE_WIDTHCALC					0x00000004
#define AGK_SPRITE_HEIGHTCALC					0x00000008
#define AGK_SPRITE_SCISSOR_ON					0x00000010
#define AGK_SPRITE_VERTEX_LAYOUT_NEEDS_UPDATE	0x00000020
#define AGK_SPRITE_VERTEX_COLOR_NEEDS_UPDATE	0x00000040
#define AGK_SPRITE_MANUALOFFSET					0x00000080
#define AGK_SPRITE_LOOP							0x00000100
#define AGK_SPRITE_PLAYING						0x00000200
#define AGK_SPRITE_SHAREDIMAGE					0x00000400
#define AGK_SPRITE_CUSTOM_SHADER				0x00000800 
#define AGK_SPRITE_POSTOPLEFT					0x00001000
#define AGK_SPRITE_SCROLL						0x00002000
#define AGK_SPRITE_FLIPH						0x00004000
#define AGK_SPRITE_FLIPV						0x00008000
#define AGK_SPRITE_SNAP							0x00010000
#define AGK_SPRITE_MANAGE_IMAGES				0x00020000
#define AGK_SPRITE_FORCE_TRANSPARENCY			0x00040000
#define AGK_SPRITE_MANUAL_SCISSOR				0x00080000

// Namespace
namespace AGK
{
	class cImage;
	class AGKShader;
	class Bone2D;
	class Skeleton2D;
	class AGKFontImage;

	// class for holding animation UV coordinates
	class _EXPORT_ cSpriteFrame
	{
		public:
			// top left corner
			float		m_fU1 = 0;
			float		m_fV1 = 0;

			// bottom right corner
			float		m_fU2 = 0;
			float		m_fV2 = 0;

			int			m_iWidth = 0;
			int			m_iHeight = 0;

			cImage		*m_pFrameImage = 0;

			cSpriteFrame() {}
			~cSpriteFrame() {}
	};

	class _EXPORT_ cSprite
	{
		protected:

			static cSprite* g_pAllSprites;
			static cSprite* g_pLastSprite;
			static uint32_t g_iCreated;

			friend class agk;
			friend class cSpriteMgr;
			friend class cSpriteMgrEx;

			static void *m_pInternalVertices;
			void *m_pInternalPipeline = 0;
			void *m_pInternalShaderConstants = 0;
			AGKRenderState m_renderState;
			AGKVertexLayout m_vertexLayout;
			float *m_pVertexData = 0;
			AGKDynamicArray<cImage*> m_shaderImages;
			AGKShaderConstantValue *m_pShaderConstantPosition = 0;
			AGKShaderConstantValue *m_pShaderConstantSize = 0;

			// general variables
			uString     m_sName; // used for bone attachments
			uint32_t	m_iCreated = 0;
			uint32_t	m_iID = 0;
			float		m_fX = 0;
			float		m_fY = 0;
			float		m_fZ = 0.001f; // m_iDepth / 10000
			int			m_iDepth = 10;
			float		m_fWidth = 1;
			float		m_fHeight = 1;
			float		m_fOffsetX = 0;
			float		m_fOffsetY = 0;
			float		m_fAngle = 0;
			float		m_fOrigWidth = 1;
			float		m_fOrigHeight = 1;
			float		m_fOrigRadius = 0;
			uint32_t	m_iFlags = AGK_SPRITE_VISIBLE | AGK_SPRITE_ACTIVE | AGK_SPRITE_LOOP | AGK_SPRITE_SCROLL | AGK_SPRITE_MANAGE_IMAGES | AGK_SPRITE_VERTEX_LAYOUT_NEEDS_UPDATE | AGK_SPRITE_VERTEX_COLOR_NEEDS_UPDATE;
			float		m_fVisualRadius = 2;
			float		m_fColRadius = 2;

			Bone2D*		m_pBone = 0;
			Skeleton2D* m_pSkeleton = 0;

			// image variables
			cImage*		m_pImage = 0;
			AGKFontImage *m_pFontImage = 0;
			uint32_t	m_iImageID = 0;
			float		m_fColorRed		= 1.0f;
			float		m_fColorGreen	= 1.0f;
			float		m_fColorBlue	= 1.0f;
			float		m_fColorAlpha	= 1.0f;
			
			// shader variables
			AGKShader*	m_pShader = 0;

			float		m_fClipX = 0;
			float		m_fClipY = 0;
			float		m_fClipX2 = 0;
			float		m_fClipY2 = 0;
			
			// UV variables
			float		m_fUVBorder = 0;
			float		m_fUOffset = 0;
			float		m_fVOffset = 0;
			float		m_fUScale = 1;
			float		m_fVScale = 1;

			bool		m_bManaged = false;

			bool		m_bUVOverride = false;
			float		m_fU1 = 0;
			float		m_fV1 = 0;
			float		m_fU2 = 0;
			float		m_fV2 = 1;
			float		m_fU3 = 1;
			float		m_fV3 = 0;
			float		m_fU4 = 1;
			float		m_fV4 = 1;
			
			// collision variables
			int			m_iGroup = 0;
			uint32_t	m_iCategories = 0x0001;
			uint32_t	m_iCategoryMask = 0xFFFF;
			float*		m_fPolygonPointsTemp = 0;
			uint32_t	m_iPolygonPointsNum = 0;

			// animation variables
			int			m_iFrameCount = 0;
			int			m_iFrameArraySize = 0;
			int			m_iFrameWidth = 0;
			int			m_iFrameHeight = 0;
			int			m_iCurrentFrame = 0;
			int			m_iFrameStart = 0;
			int			m_iFrameEnd = 0;
			float		m_fFrameTimer = 0;
			float		m_fFrameChangeTime = 0;
			cSpriteFrame *m_pFrames = 0;

			void*		m_pUserData = 0;
			int*		m_pUserInts = 0;
			float*		m_pUserFloats = 0;
			uString*	m_pUserStrings = 0;
			int			m_iNumUserInts = 0;
			int			m_iNumUserFloats = 0;
			int			m_iNumUserStrings = 0;

			cSprite*	m_pPrevSprite = 0;
			cSprite*	m_pNextSprite = 0;

			enum ePhysicsMode
			{
				eStatic,
				eDynamic,
				eKinematic
			};

			enum ePhysicsShape
			{
				eNone,
				eBox,
				eCircle,
				ePolygon,
				eManual
			};

			// physics variables
			b2Body *m_phyBody = 0;
			b2Shape *m_phyShape = 0;
			b2Shape **m_phyAdditionalShapes = 0;
			uint32_t m_iNumAdditionalShapes = 0;
			ePhysicsShape m_eShape = eNone;
			b2DistanceOutput *m_colResult = 0;
			b2Contact *m_lastContact = 0;
			b2ContactEdge *m_pContactIter = 0;

			float m_fGravityScale;
			bool m_bAwake;
			bool m_bSleep;

			uint8_t m_iUserBlendFunc = (AGK_BLEND_ONE << 4) | AGK_BLEND_ZERO; // src in high 4 bits, dst in low 4 bits
			uint8_t m_iTransparencyMode = 0;

			void ReplacePhysicsShape( b2Shape *pOldShape, b2Shape *pNewShape );

			// internal
			void Initialize ( void );
			void RecalcVisualRadius();
			void RecalcColRadius();
			void CheckAndRemoveImage( cImage* pImage );
			void CheckTransparency();
			void AddShapeInternal( b2Shape* pShape );
			
			void SetTransparencyInternal( int mode );
			
		public:

			static int g_iPixelsDrawn;

			static void RemoveImage( cImage *pDelImage );
			static int UpdateVertexLayout( AGKVertexLayout *pLayout, AGKShader *pShader );
			static int GetSpriteVertexFloats() { return 3 + 4 + 2; } // position + color + uv
			static int GetSpriteVertexPosOffset() { return 0; }
			static int GetSpriteVertexColorOffset() { return 3; }
			static int GetSpriteVertexUVOffset() { return 7; }

			// construct/destruct
			cSprite();
			cSprite( const cSprite *pOtherSprite );
			cSprite( const uString &szImage );
			cSprite( cImage *pImage );
			~cSprite();

			void ImageDeleting( cImage *pImage );
		
			// get functions
			const char* GetName			( void ) { return m_sName.GetStr(); }
			uint32_t GetCreated				( void ) { return m_iCreated; }
			uint32_t GetID					( void ) { return m_iID; }
			float GetX					( void ) { return m_fX - m_fOffsetX; }
			float GetY					( void ) { return m_fY - m_fOffsetY; }
			float GetZ					( void ) { return m_fZ; }
			float GetXByOffset			( void ) { return m_fX; }
			float GetYByOffset			( void ) { return m_fY; }
			float GetOffsetX			( void ) { return m_fOffsetX; }
			float GetOffsetY			( void ) { return m_fOffsetY; }
			float GetAngle				( void ) { return m_fAngle * (180.0f/PI); }
			float GetAngleRad			( void ) { return m_fAngle; }
			float GetWidth				( void ) { return m_fWidth; }
			float GetHeight				( void ) { return m_fHeight; }
			float GetScaleX				( void ) { return m_fWidth / m_fOrigWidth; }
			float GetScaleY				( void ) { return m_fHeight / m_fOrigHeight; }
			int GetDepth				( void ) { return m_iDepth; }
			int GetFrameCount			( void ) { return m_iFrameCount; }
			int GetCurrentFrame			( void ) { return m_iCurrentFrame + 1; }
			int GetTransparencyMode		( void ) { return m_iTransparencyMode; }
			cImage* GetImagePtr			( void ) { return m_pImage; }
			bool GetVisible				( void ) { return (m_iFlags & AGK_SPRITE_VISIBLE) != 0; }
			bool GetInScreen			( void );
			bool GetActive				( void ) { return (m_iFlags & AGK_SPRITE_ACTIVE) != 0; }
			bool GetHitTest				( float x, float y );
			bool GetShouldCollide		( cSprite *pSprite2 );
			uint32_t GetColorRed			( void ) { return agkfloattoint(m_fColorRed * 255); }
			uint32_t GetColorGreen			( void ) { return agkfloattoint(m_fColorGreen * 255); }
			uint32_t GetColorBlue			( void ) { return agkfloattoint(m_fColorBlue * 255); }
			uint32_t GetColorAlpha			( void ) { return agkfloattoint(m_fColorAlpha * 255); }
			int GetScrollMode			( void ) { return (m_iFlags & AGK_SPRITE_SCROLL) != 0 ? 1 : 0; }
			int GetPlaying				( void ) { return (m_iFlags & AGK_SPRITE_PLAYING) != 0 ? 1 : 0; }
			bool GetScissorOn			( void ) { return (m_iFlags & AGK_SPRITE_SCISSOR_ON) != 0; }
			void GetClipValues			( int &x, int &y, int &width, int &height );
			int GetGroup				( void ) { return m_iGroup; }
			int HasAdditionalImages		( void );
			bool IsManaged() { return m_bManaged; }
			bool GetFlippedHorizontally	( void ) { return (m_iFlags & AGK_SPRITE_FLIPH) != 0; }
			bool GetFlippedVertically	( void ) { return (m_iFlags & AGK_SPRITE_FLIPV) != 0; }
			uint32_t GetRenderStateHash() { return m_renderState.GetHash(); }
			const AGKRenderState* GetRenderState() { return &m_renderState; }

			AGKShader* GetShader() { return m_pShader; }
			void SetShader( AGKShader* shader );

			float GetXFromPixel( int x );
			float GetYFromPixel( int y );
			int GetPixelFromX( float x );
			int GetPixelFromY( float y );

			float GetWorldXFromPoint( float x, float y );
			float GetWorldYFromPoint( float x, float y );
			float GetXFromWorld( float x, float y );
			float GetYFromWorld( float x, float y );

			// set functions
			void SetBone				( Bone2D *bone ) { m_pBone = bone; }
			void FixToSkeleton			( Skeleton2D *skeleton, Bone2D *bone, int zorder );
			void ResetSkeleton			() { m_pSkeleton = 0; m_pBone = 0; }
			void SetName				( const char* name ) { m_sName.SetStr( name ); }
			void SetID					( uint32_t ID ) { m_iID = ID; }
			void SetVisible				( bool bVisible ) { if ( bVisible ) m_iFlags |= AGK_SPRITE_VISIBLE; else m_iFlags &= ~AGK_SPRITE_VISIBLE; }
			void SetActive				( bool bActive ) { if ( bActive ) m_iFlags |= AGK_SPRITE_ACTIVE; else m_iFlags &= ~AGK_SPRITE_ACTIVE; }
			void SetColorF				( float fRed, float fGreen, float fBlue, float fAlpha=1 );
			void SetRedF				( float fRed );
			void SetGreenF				( float fGreen );
			void SetBlueF				( float fBlue );
			void SetAlphaF				( float fAlpha );
			void SetColor				( uint32_t iRed, uint32_t iGreen, uint32_t iBlue, uint32_t iAlpha=255 );
			void SetRed					( uint32_t iRed );
			void SetGreen				( uint32_t iGreen );
			void SetBlue				( uint32_t iBlue );
			void SetAlpha				( uint32_t iAlpha );
			void SetFlip				( int horz, int vert );
			void SetSnap				( int snap ) { if ( snap > 0 ) m_iFlags = m_iFlags | AGK_SPRITE_SNAP; else m_iFlags = m_iFlags & (~AGK_SPRITE_SNAP); }
			void SetPosition			( float fX, float fY );
			void SetPositionByOffset	( float fX, float fY );
			void SetX					( float fX );
			void SetY					( float fY );
			void SetZ					( float fZ );
			void SetAngle				( float fAngle );
			void SetAngleRad			( float fAngleRad );
			void SetDepth				( int iDepth );
			void SetSize				( float iWidth=-1, float iHeight=-1, bool bUpdateShape=true );
			void SetScale				( float x, float y );
			void SetScaleByOffset		( float x, float y );
			void SetImage				( const uString &szImage, bool bUpdateCollisionShape=false );
			void SetImage				( cImage *pImage, bool bUpdateCollisionShape=false );
			void SetAdditionalImage		( cImage *pImage, int stage );
			void SwitchImage			( cImage *pImage, bool bUpdateCollisionShape=false );
			void SetBlendFunction		( int src, int dst );
			void SetTransparency		( int mode );
			void SetOffset				( float x, float y );
			void SetUVBorder			( float border ) { if ( border < 0 ) border = 0; m_fUVBorder = border; }
			void SetUVOffset			( float u, float v ) { m_fUOffset = u; m_fVOffset = v; }
			void SetUVScale				( float scaleU, float scaleV );
			void FixToScreen			( int mode ) { if ( mode != 0 ) m_iFlags &= ~AGK_SPRITE_SCROLL; else m_iFlags |= AGK_SPRITE_SCROLL; }
			void SetScissor				( float x, float y, float x2, float y2 );
			void SetManualScissor		( int x, int y, int width, int height );
			void SetUV					( float u1, float v1, float u2, float v2, float u3, float v3, float u4, float v4 );
			void ResetUV				() { m_bUVOverride = false; }
			void SetManageImages		( int mode );

			void SetUserData			( void* data ) { m_pUserData = data; }
			void* GetUserData			( ) { return m_pUserData; }

			void SetUserInt				( int index, int value );
			void SetUserFloat			( int index, float value );
			void SetUserString			( int index, const uString& value );
			int GetUserInt				( int index );
			float GetUserFloat			( int index );
			const uString& GetUserString( int index );

			void SetFontImage			( AGKFontImage *pFontImage, float scale );
			const AGKFontImage* GetFontImage() { return m_pFontImage; }

			void SetAnimation			( int iFrameWidth, int iFrameHeight, int iFrameCount );
			void AppendAnimation		( cImage *pImage, int iFrameWidth, int iFrameHeight, int iFrameCount );
			void AddAnimationFrame		( cImage *pImage );
			void ExpandAnimationArray	( int newTotalFrames );
			void ClearAnimationFrames   ();
			void SetFrame				( int iFrame );
			void SetSpeed				( float fFps );
			void Play					( float fFps, bool bLoop=true, int iStart=-1, int iEnd=-1 );
			void Stop					() { m_iFlags &= ~AGK_SPRITE_PLAYING; }
			void Resume					();
			//void SetBlendFunction		( GLenum src, GLenum dest );

			void CheckImages			();
			void Draw					();
			void BatchDrawQuad			( float *pVertexData );
			
			void Update					( float time );
			void UpdateAnimation		( float time );

			// physics functions
			bool InBox( float x1, float y1, float x2, float y2 );
			bool InCircle( float x1, float y1, float radius );
			bool GetCollision			( cSprite *pSprite2 );

			float GetDistance			( cSprite *pSprite2 );
			float GetDistancePoint1X    ( void );
			float GetDistancePoint1Y    ( void );
			float GetDistancePoint2X    ( void );
			float GetDistancePoint2Y    ( void );

			bool GetPhysicsCollision( cSprite *pSprite2 );
			float GetPhysicsCollisionX( );
			float GetPhysicsCollisionY( );
			float GetPhysicsCollisionWorldX( );
			float GetPhysicsCollisionWorldY( );

			float GetPhysicsMass( );

			// can be used on its own without physics to detect hits and overlaps, sets main shape only
			void SetShape					( ePhysicsShape shape, int shapeID=-1 );
			void SetShapeBox				( float x, float y, float x2, float y2, float angle=0, int shapeID=-1 );
			void SetShapeCircle				( float x, float y, float raduis, int shapeID=-1 );
			void SetShapePolygon			( uint32_t numPoints, float *pPoints, int shapeID=-1 );
			void SetShapePolygon			( uint32_t numPoints, uint32_t index, float x, float y, int shapeID=-1 );
			void SetShapeChain				( uint32_t numPoints, float *pPoints, int loop, int shapeID );
			void SetShapeChain				( uint32_t numPoints, uint32_t index, int loop, float x, float y, int shapeID );

			// adds an additional shape to the physics body, not used on non-physics functions
			void AddShapeBox				( float x, float y, float x2, float y2, float angle=0 );
			void AddShapeCircle				( float x, float y, float raduis );
			void AddShapePolygon			( uint32_t numPoints, float *pPoints );
			void AddShapePolygon			( uint32_t numPoints, uint32_t index, float x, float y );
			void AddShapeChain				( uint32_t numPoints, float *pPoints, int loop );
			void AddShapeChain				( uint32_t numPoints, uint32_t index, int loop, float x, float y );

			void ClearAdditionalShapes		();
			int GetNumShapes				() { return m_phyShape ? m_iNumAdditionalShapes+1 : 0; }
			int GetShapeNumVertices			( int shapeID );
			float GetShapeVertexX			( int shapeID, int vertex );
			float GetShapeVertexY			( int shapeID, int vertex );

			//void Create

			void CalculatePhysicsCOM		();
			void SetPhysicsCOM				( float x, float y );
			float GetPhysicsCOMX			();
			float GetPhysicsCOMY			();

			void SetGroup					( int group, int shapeID=-1 );
			void SetCategoryBits			( uint32_t categories, int shapeID=-1 );
			void SetCategoryBit				( uint32_t category, int flag, int shapeID=-1 );
			void SetCollideBits				( uint32_t mask, int shapeID=-1 );
			void SetCollideBit				( uint32_t category, int flag, int shapeID=-1 );

			// physics dynamics/response
			void PrepareToClearPhysicsContacts();
			void SetPhysicsOn				( ePhysicsMode mode );
			void SetPhysicsOff				();
			void SetPhysicsDelete			();
			void SetPhysicsFriction			( float friction, int shapeID=-1 );
			void SetPhysicsRestitution		( float restitution, int shapeID=-1 );
			void SetPhysicsDensity			( float density, int shapeID=-1 );
			void SetPhysicsCanRotate		( bool rotate );
			void SetPhysicsVelocity			( float vx, float vy );
			void SetPhysicsAngularVelocity	( float va );
			void SetPhysicsDamping			( float damp );
			void SetPhysicsAngularDamping	( float damp );
			void SetPhysicsIsBullet			( bool bullet );
			void SetPhysicsMass				( float mass );
			void SetPhysicsIsSensor			( bool sensor, int shapeID=-1 );

			void SetPhysicsGravityScale ( float scale );
			float GetPhysicsGravityScale ( void );
			void SetPhysicsInitiallyAwake ( bool awake );
			void SetPhysicsAllowSleep ( bool sleep );
			float GetInertia ( void );
			bool GetIsBullet ( void );
			bool GetIsAwake ( void );

			float GetPhysicsVelocityX();
			float GetPhysicsVelocityY();
			float GetPhysicsAngularVelocity();
			
			void SetPhysicsForce			( float x, float y, float vx, float vy );
			void SetPhysicsTorque			( float a );
			void SetPhysicsLinearImpulse	( float x, float y, float vx, float vy );
			void SetPhysicsAngularImpulse	( float a );

			int GetFirstContact				();
			int GetNextContact				();
			float GetContactWorldX			();
			float GetContactWorldY			();
			cSprite* GetContactSprite2		();
			
			void UpdatePhysics				();
	};
}

#endif

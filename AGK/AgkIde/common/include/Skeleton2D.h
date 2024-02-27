#ifndef _H_SKELETON2D
#define _H_SKELETON2D

#define AGK_BONE_ANIMATE			0x0001
#define AGK_BONE_INHERIT_ROTATION	0x0002
#define AGK_BONE_INHERIT_SCALE		0x0004
#define AGK_BONE_ROOT				0x0008
#define AGK_BONE_PRE_SCALE			0x0010
#define AGK_BONE_UPDATED			0x0020

#define AGK_SKELETON_PLAYING        0x0001
#define AGK_SKELETON_LOOPING        0x0002
#define AGK_SKELETON_FLIPH			0x0004
#define AGK_SKELETON_FLIPV			0x0008
#define AGK_SKELETON_VISIBLE		0x0010

#define AGK_SLOT_ANIMATE            0x00000001

namespace AGK
{
	// items will be added and removed like a stack
	template<class T> class AGKStackArray
	{
		public:
			AGKStackArray( UINT size=2 ) { m_iMaxSize = size; if ( m_iMaxSize ) m_pItems = new T[ m_iMaxSize ]; }
			~AGKStackArray() { if ( m_pItems ) delete [] m_pItems; }

			void Resize( UINT size ) 
			{ 
				if ( size < m_iNumItems ) size = m_iNumItems;
				if ( size < 2 ) size = 2;
				if ( m_iMaxSize != size )
				{
					m_iMaxSize = size;
					T* newArray = new T[ m_iMaxSize ];
					if ( m_pItems )
					{
						memcpy( newArray, m_pItems, sizeof(T)*m_iNumItems );
						delete [] m_pItems;
					}
					m_pItems = newArray;
				}
			}

			void Clear() { m_iNumItems = 0; }

			void PushItem( T item )
			{
				if ( m_iMaxSize < m_iNumItems+1 )
				{
					if ( m_iMaxSize <= 1 ) m_iMaxSize = 2;
					else m_iMaxSize = m_iMaxSize + m_iMaxSize/2;
					T* newArray = new T[ m_iMaxSize ];
					if ( m_pItems )
					{
						memcpy( newArray, m_pItems, sizeof(T)*m_iNumItems );
						delete [] m_pItems;
					}
					m_pItems = newArray;
				}

				m_pItems[ m_iNumItems ] = item;
				m_iNumItems++;
			}

			T PopItem()
			{
				if ( m_iNumItems == 0 ) return 0;
				m_iNumItems--;
				return m_pItems[ m_iNumItems ];
			}

			UINT NumItems() { return m_iNumItems; }
			T& operator[](UINT i) { return m_pItems[ i ]; }
			T GetItem(UINT i) { return m_pItems[ i ]; }
		
		protected:
			T* m_pItems = 0;
			UINT m_iNumItems = 0;
			UINT m_iMaxSize = 0;
	};

	class cSprite;
	class Bone2D;

	// attachment key frames
	class Anim2DKeyFrameAttachment
	{
		public:
			float m_fTime;
			uString m_sSpriteName;
			cSprite *m_pSprite;
	};

	// bone key frames
	class Anim2DKeyFrameBone
	{
		public:
			float m_fTime;
			Bone2D *m_pBoneParent;
	};

	// ZOrder key frames
	class Anim2DKeyFrameZOrder
	{
		public:
			float m_fTime;
			int m_iZOrder;
	};

	// color key frames
	class Anim2DKeyFrameColor
	{
		public:
			float m_fTime;
			unsigned char m_iRed;
			unsigned char m_iGreen;
			unsigned char m_iBlue;
			unsigned char m_iAlpha;

			Anim2DKeyFrameColor() {}
			virtual ~Anim2DKeyFrameColor() {}

			virtual void Interpolate( Anim2DKeyFrameColor* pNext, float t, unsigned char &red, unsigned char &green, unsigned char &blue, unsigned char &alpha );
	};

	class Anim2DKeyFrameColorStepped : public Anim2DKeyFrameColor
	{
		public:
			Anim2DKeyFrameColorStepped() {}
			virtual ~Anim2DKeyFrameColorStepped() {}

			virtual void Interpolate( Anim2DKeyFrameColor* pNext, float t, unsigned char &red, unsigned char &green, unsigned char &blue, unsigned char &alpha )
			{
				red = m_iRed;
				green = m_iGreen;
				blue = m_iBlue;
				alpha = m_iAlpha;
			}
	};

	class Anim2DKeyFrameColorCurved : public Anim2DKeyFrameColor
	{
		public:
			float c1, c2, c3, c4;

			Anim2DKeyFrameColorCurved() {}
			virtual ~Anim2DKeyFrameColorCurved() {}

			virtual void Interpolate( Anim2DKeyFrameColor* pNext, float t, unsigned char &red, unsigned char &green, unsigned char &blue, unsigned char &alpha );
	};

	// rotation keyframes
	class Anim2DKeyFrameRotation
	{
		public:
			float m_fTime;
			float m_fRotation;

			Anim2DKeyFrameRotation() {}
			virtual ~Anim2DKeyFrameRotation() {}

			virtual void Interpolate( Anim2DKeyFrameRotation* pNext, float t, float &r )
			{
				if ( !pNext ) 
				{
					r = m_fRotation;
					return;
				}

				float oldR = m_fRotation;
				float newR = pNext->m_fRotation;

				if ( oldR < newR )
				{
					while ( newR - oldR > 180 ) oldR += 360;
				}
				else
				{
					while ( oldR - newR > 180 ) newR += 360;
				}

				r = oldR + t*(newR - oldR);				
			}
	};

	class Anim2DKeyFrameRotationStepped : public Anim2DKeyFrameRotation
	{
		public:
			Anim2DKeyFrameRotationStepped() {}
			virtual ~Anim2DKeyFrameRotationStepped() {}

			virtual void Interpolate( Anim2DKeyFrameRotation* pNext, float t, float &r )
			{
				r = m_fRotation;
			}
	};

	class Anim2DKeyFrameRotationCurved : public Anim2DKeyFrameRotation
	{
		public:
			float c1, c2, c3, c4;

			Anim2DKeyFrameRotationCurved() {}
			virtual ~Anim2DKeyFrameRotationCurved() {}

			virtual void Interpolate( Anim2DKeyFrameRotation* pNext, float t, float &r )
			{
				float guess = 0.5f;
				float newGuess = t;
				int iter = 0;

				// use newton raphson to find s for t (t=beizer x axis)
				do
				{
					guess = newGuess;
					newGuess = guess - ( (EvaluateBezier(c1, c3, guess) - t) / EvaluateBezierDt(c1, c3, guess) );
					iter++;
				} while ( fabs(newGuess-guess) > 0.0001f && iter < 10 );

				// get new t (from bezier y axis) for this s
				t = EvaluateBezier(c2, c4, newGuess);

				float oldR = m_fRotation;
				float newR = pNext->m_fRotation;

				if ( oldR < newR )
				{
					while ( newR - oldR > 180 ) oldR += 360;
				}
				else
				{
					while ( oldR - newR > 180 ) newR += 360;
				}

				r = oldR + t*(newR - oldR);	
			}
	};

	// position keyframes
	class Anim2DKeyFramePosition
	{
		public:
			float m_fTime;
			float m_fX;
			float m_fY;

			Anim2DKeyFramePosition() {}
			virtual ~Anim2DKeyFramePosition() {}

			virtual void Interpolate( Anim2DKeyFramePosition* pNext, float t, float &x, float &y )
			{
				if ( !pNext ) 
				{
					x = m_fX;
					y = m_fY;
					return;
				}

				x = m_fX + t*(pNext->m_fX - m_fX);
				y = m_fY + t*(pNext->m_fY - m_fY);
			}
	};

	class Anim2DKeyFramePositionStepped : public Anim2DKeyFramePosition
	{
		public:
			Anim2DKeyFramePositionStepped() {}
			virtual ~Anim2DKeyFramePositionStepped() {}

			virtual void Interpolate( Anim2DKeyFramePosition* pNext, float t, float &x, float &y )
			{
				x = m_fX;
				y = m_fY;
			}
	};

	class Anim2DKeyFramePositionCurved : public Anim2DKeyFramePosition
	{
		public:
			float c1, c2, c3, c4;

			Anim2DKeyFramePositionCurved() {}
			virtual ~Anim2DKeyFramePositionCurved() {}

			virtual void Interpolate( Anim2DKeyFramePosition* pNext, float t, float &x, float &y )
			{
				float guess = 0.5f;
				float newGuess = t;
				int iter = 0;

				// use newton raphson to find s for t (t=beizer x axis)
				do
				{
					guess = newGuess;
					newGuess = guess - ( (EvaluateBezier(c1, c3, guess) - t) / EvaluateBezierDt(c1, c3, guess) );
					iter++;
				} while ( fabs(newGuess-guess) > 0.00001f && iter < 10 );

				// get new t (from bezier y axis) for this s
				t = EvaluateBezier(c2, c4, newGuess);

				x = m_fX + t*(pNext->m_fX - m_fX);
				y = m_fY + t*(pNext->m_fY - m_fY);
			}
	};

	// scale keyframes
	class Anim2DKeyFrameScale
	{
		public:
			float m_fTime;
			float m_fScaleX;
			float m_fScaleY;

			Anim2DKeyFrameScale() {}
			virtual ~Anim2DKeyFrameScale() {}

			virtual void Interpolate( Anim2DKeyFrameScale* pNext, float t, float &x, float &y )
			{
				if ( !pNext ) 
				{
					x = m_fScaleX;
					y = m_fScaleY;
					return;
				}

				x = m_fScaleX + t*(pNext->m_fScaleX - m_fScaleX);
				y = m_fScaleY + t*(pNext->m_fScaleY - m_fScaleY);
			}
	};

	class Anim2DKeyFrameScaleStepped : public Anim2DKeyFrameScale
	{
		public:
			Anim2DKeyFrameScaleStepped() {}
			virtual ~Anim2DKeyFrameScaleStepped() {}

			virtual void Interpolate( Anim2DKeyFrameScale* pNext, float t, float &x, float &y )
			{
				x = m_fScaleX;
				y = m_fScaleY;
			}
	};

	class Anim2DKeyFrameScaleCurved : public Anim2DKeyFrameScale
	{
		public:
			float c1, c2, c3, c4;

			Anim2DKeyFrameScaleCurved() {}
			virtual ~Anim2DKeyFrameScaleCurved() {}

			virtual void Interpolate( Anim2DKeyFrameScale* pNext, float t, float &x, float &y )
			{
				float guess = 0.5f;
				float newGuess = t;
				int iter = 0;

				// use newton raphson to find s for t (t=beizer x axis)
				do
				{
					guess = newGuess;
					newGuess = guess - ( (EvaluateBezier(c1, c3, guess) - t) / EvaluateBezierDt(c1, c3, guess) );
					iter++;
				} while ( fabs(newGuess-guess) > 0.00001f && iter < 10 );

				// get new t (from bezier y axis) for this s
				t = EvaluateBezier(c2, c4, newGuess);

				x = m_fScaleX + t*(pNext->m_fScaleX - m_fScaleX);
				y = m_fScaleY + t*(pNext->m_fScaleY - m_fScaleY);
			}
	};

	// offset keyframes
	class Anim2DKeyFrameOffset
	{
		public:
			float m_fTime;
			float m_fOffsetX;
			float m_fOffsetY;

			Anim2DKeyFrameOffset() {}
			virtual ~Anim2DKeyFrameOffset() {}

			virtual void Interpolate( Anim2DKeyFrameOffset* pNext, float t, float &x, float &y )
			{
				if ( !pNext ) 
				{
					x = m_fOffsetX;
					y = m_fOffsetY;
					return;
				}

				x = m_fOffsetX + t*(pNext->m_fOffsetX - m_fOffsetX);
				y = m_fOffsetY + t*(pNext->m_fOffsetY - m_fOffsetY);
			}
	};

	class Anim2DKeyFrameOffsetStepped : public Anim2DKeyFrameOffset
	{
		public:
			Anim2DKeyFrameOffsetStepped() {}
			virtual ~Anim2DKeyFrameOffsetStepped() {}

			virtual void Interpolate( Anim2DKeyFrameOffset* pNext, float t, float &x, float &y )
			{
				x = m_fOffsetX;
				y = m_fOffsetY;
			}
	};

	class Anim2DKeyFrameOffsetCurved : public Anim2DKeyFrameOffset
	{
		public:
			float c1, c2, c3, c4;

			Anim2DKeyFrameOffsetCurved() {}
			virtual ~Anim2DKeyFrameOffsetCurved() {}

			virtual void Interpolate( Anim2DKeyFrameOffset* pNext, float t, float &x, float &y )
			{
				float guess = 0.5f;
				float newGuess = t;
				int iter = 0;

				// use newton raphson to find s for t (t=beizer x axis)
				do
				{
					guess = newGuess;
					newGuess = guess - ( (EvaluateBezier(c1, c3, guess) - t) / EvaluateBezierDt(c1, c3, guess) );
					iter++;
				} while ( fabs(newGuess-guess) > 0.00001f && iter < 10 );

				// get new t (from bezier y axis) for this s
				t = EvaluateBezier(c2, c4, newGuess);

				x = m_fOffsetX + t*(pNext->m_fOffsetX - m_fOffsetX);
				y = m_fOffsetY + t*(pNext->m_fOffsetY - m_fOffsetY);
			}
	};

	class Anim2DSlot
	{
		public:
			uString m_sSlotName;
			int m_iSlotIndex;
			int m_iTimelineID;

			UINT m_iNumColors;
			Anim2DKeyFrameColor **m_pColors;

			UINT m_iNumAttachments;
			Anim2DKeyFrameAttachment **m_pAttachments;

			UINT m_iNumBones;
			Anim2DKeyFrameBone **m_pBones;

			UINT m_iNumZOrder;
			Anim2DKeyFrameZOrder **m_pZOrder;

			UINT m_iNumRotations;
			Anim2DKeyFrameRotation **m_pRotations;

			UINT m_iNumPositions;
			Anim2DKeyFramePosition **m_pPositions;

			UINT m_iNumScales;
			Anim2DKeyFrameScale **m_pScales;

			UINT m_iNumOffsets;
			Anim2DKeyFrameOffset **m_pOffsets;

			Anim2DSlot();
			~Anim2DSlot();
	};

	class Slot2D
	{
		public:
			Skeleton2D* m_pSkeleton;

			uString m_sName;
			Bone2D *m_pParent;
			UINT m_bFlags;

			cSprite *m_pSprite;
			UINT m_iColor;
			float x, y, angle, sX, sY, offsetX, offsetY;

			cSprite *m_pOrigSprite;
			UINT m_iOrigColor;
			Bone2D *m_pOrigBone;
			int m_iZOrder;

			Anim2DSlot *m_pPrevAnim;
			Anim2DSlot *m_pAnim;

			UINT m_iPrevFrameColor;
			UINT m_iPrevFrameAttachment;
			UINT m_iPrevFrameBone;
			UINT m_iPrevFrameZOrder;
			UINT m_iPrevFrameRotation;
			UINT m_iPrevFramePosition;
			UINT m_iPrevFrameScale;
			UINT m_iPrevFrameOffset;
			
			UINT m_iCurrFrameColor;
			UINT m_iCurrFrameAttachment;
			UINT m_iCurrFrameBone;
			UINT m_iCurrFrameZOrder;
			UINT m_iCurrFrameRotation;
			UINT m_iCurrFramePosition;
			UINT m_iCurrFrameScale;
			UINT m_iCurrFrameOffset;

			Slot2D( Skeleton2D* parent );
			~Slot2D();

			void Tween( float prevtime, float currtime, float s );
			void Interpolate( float currtime );
	};

	// animation for 1 bone
	class Anim2DBone
	{
		public:
			uString m_sBoneName;
			int m_iBoneIndex;
			int m_iTimelineID;
			
			UINT m_iNumRotations;
			Anim2DKeyFrameRotation **m_pRotations;

			UINT m_iNumPositions;
			Anim2DKeyFramePosition **m_pPositions;

			UINT m_iNumScales;
			Anim2DKeyFrameScale **m_pScales;

			UINT m_iNumParents;
			Anim2DKeyFrameBone **m_pParents;

			Anim2DBone();
			~Anim2DBone();
	};

	class Bone2D
	{
		public:
			Skeleton2D* m_pSkeleton;

			uString m_sName;
			float length;
			float origX, origY, origAngle, origSX, origSY;
			float x, y, angle, sX, sY;
			float worldX, worldY, worldAngle, worldSX, worldSY;
			UINT m_bFlags;
			Bone2D *m_pParent;
			float m00, m01, m10, m11;

			Anim2DBone *m_pPrevAnim;
			Anim2DBone *m_pAnim;
			UINT m_iPrevFrameRotation;
			UINT m_iPrevFramePosition;
			UINT m_iPrevFrameScale;
			UINT m_iPrevFrameParent;
			UINT m_iCurrFrameRotation;
			UINT m_iCurrFramePosition;
			UINT m_iCurrFrameScale;
			UINT m_iCurrFrameParent;

			Bone2D();
			~Bone2D();

			void Tween( float prevtime, float currtime, float s );
			void Interpolate( float currtime );

			void UpdateWorldMatrix( int flipH, int flipV );
			void ResetToOrig();
	};

	// single animation for a set of bones, e.g. "walk"
	class Animation2D
	{
		public:
			uString m_sName;
			float m_fTime;
			
			UINT m_iNumBones;
			Anim2DBone *m_pBoneAnims;
			
			UINT m_iNumSlots;
			Anim2DSlot *m_pSlotAnims;
			
			Animation2D();
			~Animation2D();

			Anim2DBone* GetAnimForBone( const char* name );
			Anim2DSlot* GetAnimForSlot( const char* name );

			Anim2DBone* GetAnimForBoneTimeline( int timelineID );
			Anim2DSlot* GetAnimForSlotTimeline( int timelineID );
	};

	class ExternalSprite
	{
		public:
			cSprite *m_pSprite;
			int m_iZOrder;
			ExternalSprite *m_pPrev;
			ExternalSprite *m_pNext;

			ExternalSprite() { m_pSprite = 0; m_iZOrder = 0; m_pPrev = 0; m_pNext = 0; }
			~ExternalSprite() {}
	};

	class SpriterSprite
	{
		public:
			cSprite* m_pSprite;
			int m_iInUse;

			SpriterSprite() { m_pSprite = 0; m_iInUse = -1; }
			~SpriterSprite() {}
	};

	class SpriterFile
	{
		public:
			AGKStackArray<SpriterSprite> m_pSprites;
			float m_offsetX;
			float m_offsetY;

			SpriterFile() {}
			~SpriterFile() {}

			void ResetUsage() 
			{
				for( UINT i = 0; i < m_pSprites.NumItems(); i++ ) m_pSprites[ i ].m_iInUse = -1;
			}

			cSprite* GetSprite( int forSlot, UINT* isNew=0 )
			{
				if ( m_pSprites.NumItems() == 0 ) return 0;

				cSprite* pSprite = 0;
				for( UINT i = 0; i < m_pSprites.NumItems(); i++ )
				{
					if ( m_pSprites[ i ].m_iInUse == forSlot || m_pSprites[ i ].m_iInUse == -1 )
					{
						pSprite = m_pSprites[ i ].m_pSprite;
						m_pSprites[ i ].m_iInUse = forSlot;
						break;
					}
				}

				if ( !pSprite )
				{
					pSprite = new cSprite( m_pSprites[0].m_pSprite );
					SpriterSprite newSprite;
					newSprite.m_pSprite = pSprite;
					newSprite.m_iInUse = forSlot;
					m_pSprites.PushItem( newSprite );
					if ( isNew ) *isNew = 1;
				}
				else
				{
					if ( isNew ) *isNew = 0;
				}

				return pSprite;
			}

			void AddSprite( cSprite* pSprite, float offsetX, float offsetY )
			{
				SpriterSprite newSprite;
				newSprite.m_iInUse = -1;
				newSprite.m_pSprite = pSprite;
				m_pSprites.PushItem( newSprite );
				m_offsetX = offsetX;
				m_offsetY = offsetY;
			}
	};

	class SpriterFolder
	{
		public:
			AGKStackArray<SpriterFile*> m_pFiles;

			SpriterFolder() {}
			~SpriterFolder() 
			{
				for( UINT i = 0; i < m_pFiles.NumItems(); i++ ) delete m_pFiles[ i ];
			}
			
			void ResetUsage() 
			{
				for( UINT i = 0; i < m_pFiles.NumItems(); i++ ) m_pFiles[ i ]->ResetUsage();
			}
	};

	class Skeleton2D
	{
			friend class agk;

		protected:
			int m_isSpriter;

			UINT m_iNumAnimations;
			Animation2D *m_pAnimations;

			UINT m_iNumBones;
			Bone2D *m_pBones;

			AGKStackArray<Slot2D*> m_pSlots;
			AGKStackArray<SpriterFolder*> m_pFolders;
			AGKStackArray<cSprite*> m_pSprites;

			ExternalSprite *m_pFirstExtSprite;

			UINT m_iID;
			UINT m_bFlags;
			float m_fCurrTime;
			float m_fPrevTime;
			float m_fTweenTime;
			float m_fTotalTweenTime;
			float m_fSpeed;
			int m_iCurrAnimation;
			int m_iLoopCount;
			int m_iLoopTotal;

			float m_fX;
			float m_fY;
			float m_fAngle;

			int m_iDepth;

		public:
			Skeleton2D();
			~Skeleton2D();

			int IsSpriter() { return m_isSpriter; }

			void LoadFromSpine( const char* filename, float scale, cImage *pAtlas, int loadAnim );
			void LoadFromSpriter( const char* filename, float scale, cImage *pAtlas );
			Bone2D* GetBone( const char* name );
			Bone2D* GetBone( UINT index );
			int GetBoneIndex( const char* name );
			Slot2D* GetSlot( const char* name );
			int GetSlotIndex( const char* name );
			cSprite* GetSprite( const char* name );
			int GetAnimation( const char* name );

			void SetPosition( float x, float y );
			void SetAngle( float angle );
			void SetDepth( int depth );
			int GetDepth() { return m_iDepth; }
			void FixToScreen( int mode );
			void SetVisible( int mode );

			void AddExternalSprite( cSprite *sprite, int zorder );
			void RemoveExternalSprite( cSprite *sprite );

			float GetX() { return m_fX; }
			float GetY() { return m_fY; }
			float GetAngle() { return m_fAngle; }

			void SetAnimationSpeed( float speed );
			void PlayAnimation( const char* anim, float starttime, int loop, float tweentime );
			void SetAnimationFrame( const char* anim, float time, float tweentime );
			void StopAnimation();
			int GetIsAnimating();
			int GetIsTweening();
			float GetCurrentTime();
			float GetAnimationTime( const char* anim );

			void SetFlipH( int flip );
			void SetFlipV( int flip );

			void Update( float time );
			void Draw();
			void DrawBones();
			void DrawBoneNames();
	};
}

#endif
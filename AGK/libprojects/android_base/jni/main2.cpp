#include "agk.h"
#include <android/keycodes.h>

struct sPosition2D
{
	float x;
	float y;

	sPosition2D() { x = 0; y = 0; }
};

struct sMole
{
	enum eMoleState
	{
		eRising,
		eTop,
		eStunned,
		eFalling,
		eBottom
	};

	float fWaitTimer;
	eMoleState eState;
	int iSpriteID;
	int iGridPosition;
	float fSpeed;
	bool bHit;

	sMole() { eState = eBottom; iSpriteID = 0; fSpeed = 10.0f; fWaitTimer = 0.0f; iGridPosition = 0; bHit = false; }
};

sPosition2D g_sGridPositions[ 9 ];
sPosition2D g_sGridPositionsTop[ 9 ];
sMole g_sMoles[ 9 ];

int iScore = 0;

float g_fFps = 0;

// shared variables test
cNetwork g_Network1;

// images
int iBackground;
int iForeground;
int iOverlay1;
int iOverlay2;
int iOverlay3;
int iMole;


class MyThread : public AGKThread
{
	public:

	UINT Run()
	{
		while ( !m_bTerminate )
		{
			agk::Warning( "Thread Test" );
			agk::Sleep( 1000 );
		}

		return 0;
	}

};

extern "C"
{
MyThread myThread;

	void init( void *ptr )
	{
		agk::InitGL(ptr);
	}

	void updateptr( void *ptr )
	{
		agk::Warning("Updating Pointer");
		//agk::MasterReset();
		//agk::InitGL(ptr);
		agk::UpdatePtr(ptr);
	}

	void clearptr( void *ptr )
	{
		agk::UpdatePtr(ptr);
	}

	void cleanup()
	{
		//g_Network1.CloseNetwork();
		agk::MasterReset();
	}

	void begin()
	{
		//myThread.Start();
		/*
		agk::SetClearColor( 255,0,0 );
		//agk::SetOrientationAllowed( 1,1, 0,0 );

		agk::LoadImage( 1, "red_table.jpg" );
		agk::CreateSprite( 1, 1 );
		agk::SetSpriteDepth( 1, 100 );
		agk::SetSpriteSize( 1, 80, -1 );

		agk::CreateSprite( 2, 0 );
		agk::SetSpritePosition( 2, 50,50 );
		agk::SetSpriteColor( 2, 0,255,0,255 );
		agk::CreateSprite( 3, 0 );
		agk::SetSpritePosition( 3, 90,90 );
		agk::SetSpriteColor( 3, 0,0,255,255 );
		agk::CreateSprite( 4, 0 );
		agk::SetSpritePosition( 4, 0,0 );
		agk::SetSpriteColor( 4, 0,255,255,255 );

		agk::LoadSound( 1, "face.wav" );
		agk::SetSoundSystemVolume( 50 );

		agk::LoadMusic( 1, "ingame.mp3" );
		agk::LoadMusic( 2, "starioland.mp3" );
		agk::SetMusicSystemVolume( 100 );
		agk::PlayMusic( 1, 1, 1, 2 );
		*/

		agk::SetDisplayAspect( 2.0f/3.0f ); // portrait

		//agk::SetVirtualResolution( 320,480 );
		//agk::SetOrientationAllowed( 0,0, 1,1 );
		agk::SetPhysicsGravity( 0,50 ); // in pixels per second squared
		agk::SetPhysicsDebugOn();
		agk::SetErrorMode( 1 );
		agk::SetGenerateMipmaps( true );
		agk::SetDefaultMinFilter( 1 );
		agk::SetDefaultMagFilter( 1 );
		agk::SetSortTextures( true );

		g_Network1.JoinNetwork( "My Network", "Android" );

		agk::SetCurrentDir( "levels/mole" );

		iBackground = agk::LoadImage ( "background.png" );
		iForeground = agk::LoadImage ( "foreground.png" );
		iOverlay1 = agk::LoadImage ( "overlay1.png" );
		iOverlay2 = agk::LoadImage ( "overlay2.png" );
		iOverlay3 = agk::LoadImage ( "overlay3.png" );
		iMole = agk::LoadImage ( "mole.png" );

		agk::SetCurrentDir( "" );

		UINT iItem0 = agk::LoadImage ( "levels/mole/item0.png" );
		UINT iItem1 = agk::LoadImage ( "levels/mole/item1.png" );
		UINT iItem2 = agk::LoadImage ( "levels/mole/item2.png" );
		UINT iItem3 = agk::LoadImage ( "levels/mole/item3.png" );
		UINT iItem4 = agk::LoadImage ( "levels/mole/item4.png" );
		UINT iItem5 = agk::LoadImage ( "levels/mole/overlay3.png" );


		// physics sprite
		agk::CreateSprite( 99, iMole );
		agk::SetSpriteSize( 99, 10, -1 );
		agk::SetSpritePositionByOffset( 99, 30, 50 );
		agk::SetSpriteAnimation( 99, 82, 88, 11 );
		agk::SetSpriteFrame( 99, 1 );
		//agk::SetSpriteShape( 99, 3 );
		agk::SetSpritePhysicsOn( 99, 2 );
		agk::SetSpritePhysicsRestitution( 99, 0.8f );
		agk::AddSpriteShapeCircle( 99, 0,-5, 2.5 );
		agk::SetSpritePhysicsCOM( 99, 0,0 ); //reset center of mass to offset point
		//agk::SetSpritePhysicsMass( 99, 4 );

		agk::CreateSprite( 98, 0 );
		agk::SetSpriteSize( 98, 30, -1 );
		agk::SetSpritePositionByOffset( 98, 60, 50 );
		//agk::SetSpriteAnimation( 98, 82, 88, 11 );
		//agk::SetSpriteFrame( 98, 1 );
		agk::AddSpriteAnimationFrame( 98, iItem0 );
		agk::AddSpriteAnimationFrame( 98, iItem1 );
		agk::AddSpriteAnimationFrame( 98, iItem2 );
		agk::AddSpriteAnimationFrame( 98, iItem3 );
		agk::AddSpriteAnimationFrame( 98, iItem4 );
		agk::SetSpriteDepth( 98, 0 );
		agk::SetSpriteFlip( 98, 0, 0 );
		agk::SetSpriteFrame( 98, 1 );
		agk::PlaySprite( 98, 5, 1, 1,5 );
		agk::SetSpriteShape( 98, 3 ); //polygon
		agk::SetSpritePhysicsOn( 98, 2 );
		//agk::SetSpriteColor( 98, 255, 0, 0, 128 );

		// global force test
		//agk::CreatePhysicsForce( 50,50, 300, 100, -1, false );

		//joint test
		agk::CreateDistanceJoint( 99, 98, 30,50, 60,50 );

		agk::CreateSprite( 97, 0 );
		agk::SetSpriteSize( 97, 20, -1 );
		agk::SetSpritePosition( 97, 50, 70 );
		//agk::SetSpriteAnimation( 97, 82, 88, 11 );
		//agk::SetSpriteFrame( 97, 1 );
		agk::SetSpriteShapeBox( 97, 0,0, agk::GetSpriteWidth(97), agk::GetSpriteHeight(97) );
		agk::SetSpriteScaleByOffset( 97, 1, 2 );
		//agk::SetSpritePhysicsOn( 97, 1 );
		//agk::SetSpritePhysicsIsSensor( 97, 1 );

		// accelerometer sprite
		agk::CreateSprite( 96, iMole );
		agk::SetSpriteSize( 96, 5, -1 );
		agk::SetSpriteAnimation( 96, 82, 88, 11 );
		agk::SetSpriteFrame( 96, 1 );
		agk::SetSpritePositionByOffset( 96, 50, 30 );
		agk::SetSpriteDepth( 96, 0 );
		agk::SetSpriteTransparency( 96, 0 );

		// create level sprites
		agk::CreateSprite ( 1, iBackground );
		agk::CreateSprite ( 2, iForeground );
		agk::CreateSprite ( 3, iOverlay1 );
		agk::CreateSprite ( 4, iOverlay2 );
		agk::CreateSprite ( 5, iOverlay3 );

		agk::SetSpriteSize( 1, 100, -1 );
		agk::SetSpriteSize( 2, 100, -1 );
		agk::SetSpriteSize( 3, 100, -1 );
		agk::SetSpriteSize( 4, 100, -1 );
		agk::SetSpriteSize( 5, 100, -1 );

		//agk::SetSpriteVisible( 1, 0 );
		//agk::FixSpriteToScreen( 1, 1 );
		//agk::SetSpriteScrollMode( 98, 1 );
		//agk::SetSpriteUVOffset( 2, 0.1f, 0.1f );

		// position level sprites
		agk::SetSpritePosition( 1, 0, 0 );

		float posY = agk::GetVirtualHeight()-agk::GetSpriteHeight(2);
		agk::SetSpritePosition( 2, 0, posY );

		float posY2 = posY + 0.42f*agk::GetSpriteHeight(2);
		agk::SetSpritePosition( 3, 0, posY2 );

		float posY3 = posY + 0.67f*agk::GetSpriteHeight(2);
		agk::SetSpritePosition( 4, 0, posY3 );

		float posY4 = posY + 0.925f*agk::GetSpriteHeight(2);
		agk::SetSpritePosition( 5, 0, posY4 );

		// set level depth, leaving room in between the overlays for moles
		agk::SetSpriteDepth( 1, 100 );
		agk::SetSpriteDepth( 2, 99 );
		agk::SetSpriteDepth( 3, 30 );
		agk::SetSpriteDepth( 4, 20 );
		agk::SetSpriteDepth( 5, 10 );

		// set level transparency
		agk::SetSpriteTransparency( 1, 1 );
		agk::SetSpriteTransparency( 2, 1 );
		agk::SetSpriteTransparency( 3, 1 );
		agk::SetSpriteTransparency( 4, 1 );
		agk::SetSpriteTransparency( 5, 1 );

		agk::SetSpriteUVBorder( 3, 0.5f );
		agk::SetSpriteUVBorder( 4, 0.5f );
		agk::SetSpriteUVBorder( 5, 0.5f );

		// assign position values to our grid pattern
		// top row
		g_sGridPositions[ 0 ].x = 2.5;	g_sGridPositions[ 0 ].y = posY2+1;
		g_sGridPositions[ 1 ].x = 40;	g_sGridPositions[ 1 ].y = posY2+1;
		g_sGridPositions[ 2 ].x = 77.5;	g_sGridPositions[ 2 ].y = posY2+1;

		// middle row
		g_sGridPositions[ 3 ].x = 2.5;	g_sGridPositions[ 3 ].y = posY3+1;
		g_sGridPositions[ 4 ].x = 40;	g_sGridPositions[ 4 ].y = posY3+1;
		g_sGridPositions[ 5 ].x = 77.5;	g_sGridPositions[ 5 ].y = posY3+1;

		// bottom row
		g_sGridPositions[ 6 ].x = 2.5;	g_sGridPositions[ 6 ].y = posY4+1;
		g_sGridPositions[ 7 ].x = 40;	g_sGridPositions[ 7 ].y = posY4+1;
		g_sGridPositions[ 8 ].x = 77.5;	g_sGridPositions[ 8 ].y = posY4+1;

		for ( int i = 0; i < 9; i++ )
		{
			g_sGridPositionsTop[ i ].x = g_sGridPositions[ i ].x;
			g_sGridPositionsTop[ i ].y = g_sGridPositions[ i ].y - 12;
		}

		// create mole sprites
		for ( int i = 11; i < 20; i++ )
		{
			if ( i == 11 )
			{
				agk::CreateSprite ( i, iMole );
				agk::SetSpriteSize( i, 20, -1 );
				agk::SetSpriteAnimation( i, 82, 88, 11 );
				agk::SetSpriteFrame( i, 1 );
				agk::SetSpriteColor( i, 255,255,255, 255 );
				agk::SetSpriteTransparency( i, 1 );
				agk::SetSpriteShape( i, 2 );
			}
			else
			{
				agk::CloneSprite( i, 11 );
			}

			int iMoleID = i - 11;
			g_sMoles[ iMoleID ].iSpriteID = i;
			g_sMoles[ iMoleID ].iGridPosition = iMoleID;

			agk::SetSpritePosition( i, g_sGridPositions[ iMoleID ].x, g_sGridPositions[ iMoleID ].y );
			if ( iMoleID == 0 || iMoleID == 1 || iMoleID == 2 ) agk::SetSpriteDepth( i, 35 );
			if ( iMoleID == 3 || iMoleID == 4 || iMoleID == 5 ) agk::SetSpriteDepth( i, 25 );
			if ( iMoleID == 6 || iMoleID == 7 || iMoleID == 8 ) agk::SetSpriteDepth( i, 15 );
		}

		agk::LoadImage( 120, "ascii3.png" );

		//agk::SetSpriteImage( 2, 120 );

		// text test
		agk::CreateText( 1, "Hello World\nAbcdefghijklmnopqrstuvwxyz" );
		agk::SetTextFontImage( 1, 120 );
		agk::SetTextPosition( 1, 50, 18 );
		agk::SetTextColor( 1, 0,0,0 );
		agk::SetTextSpacing( 1, 0 );
		agk::SetTextDepth( 1, 3 );
		agk::SetTextAlignment( 1, 1 );
		agk::SetTextSize( 1, 4 );
		agk::SetTextLineSpacing( 1, 1 );
		//agk::SetTextVisible( 1, 0 );

		UINT iParticle = agk::LoadImage( "particle.png" );
		agk::CreateParticles( 1, 50,0 );
		agk::SetParticlesAngle( 1, 20 ); // give them a slight variation in direction
		agk::SetParticlesDepth( 1, 99 );
		agk::SetParticlesFrequency( 1, 30 );
		//agk::SetParticlesVelocityRange( 1, 0.5, 2 );
		agk::SetParticlesLife( 1, 3 );
		agk::SetParticlesSize( 1, 20 );
		agk::SetParticlesStartZone( 1, -50,0,50,0 );
		agk::SetParticlesImage( 1, iParticle );
		agk::SetParticlesDirection( 1, 0, 10.0f ); // main direction is straight down
		agk::SetParticlesColorInterpolation( 1, 1 );
		agk::SetParticlesMax( 1, 100 );

		// fade in
		agk::AddParticlesColorKeyFrame( 1, 0.0f, 255,255,255,0 );
		agk::AddParticlesColorKeyFrame( 1, 0.3f, 255,255,255,255 );

		// fade out
		agk::AddParticlesColorKeyFrame( 1, 2.7f, 255,255,255,255 );
		agk::AddParticlesColorKeyFrame( 1, 3.0f, 255,255,255,0 );

		agk::AddVirtualJoystick( 1, 15,90, 25 );
		//agk::SetVirtualJoystickAlpha( 1, 150, 200 );
		agk::SetVirtualJoystickImageInner( 1, iParticle );
		agk::SetVirtualJoystickImageInner( 1, 0 );

		agk::SetPrintColor( 0,0,0,255 );
		agk::SetPrintSize( 3.5f );

		agk::SetViewZoomMode( 1 );

		agk::CompleteRawJoystickDetection();

		agk::AddVirtualButton( 1, 89, 93, 20 );
		//agk::SetVirtualButtonColor( 1,0,0,255 );
		agk::SetVirtualButtonText( 1, "Test" );

		agk::SetSpritePosition( 67, 0,0 );

		//OutputDebugString( "test\n" );
	}

	void loop()
	{
		/*
		static float y = 0;
		agk::SetSpritePosition( 1, 0, y );
		y += 0.1f;

		agk::Print( agk::GetRawKeyPressed( AGK_KEY_ENTER ) );
		agk::Print( agk::GetRawKeyPressed( AGK_KEY_BACK ) );

		// grab touch events
		int count = 0;
		UINT iTouchID = agk::GetRawFirstTouchEvent(true);
		while ( iTouchID > 0 )
		{
			//agk::StartTextInput();
			int type = agk::GetRawTouchType( iTouchID );
			float fX = agk::GetRawTouchStartX( iTouchID );
			float fY = agk::GetRawTouchStartY( iTouchID );
			float fX2 = agk::GetRawTouchCurrentX( iTouchID );
			float fY2 = agk::GetRawTouchCurrentY( iTouchID );

			agk::SetSpritePosition( iTouchID+1, fX2, fY2 );
			count++;

			//agk::Print( iTouchID );

			iTouchID = agk::GetRawNextTouchEvent();
		}

		if ( agk::GetPointerPressed() == 1 ) agk::PlaySound( 1 );

		//uString str;
		//str.Format( "Y: %.1f", y );
		//agk::Warning( str );

		agk::Sync();
		*/

		// update fps
		uString str;
		str.Format( "FPS: %.1f", agk::ScreenFPS() );
		agk::Print( str );

		if ( agk::GetRawJoystickExists( 1 ) )
		{
			str.Format( "X: %.2f, Y: %.2f, Z: %.2f", agk::GetRawJoystickX(1), agk::GetRawJoystickY(1), agk::GetRawJoystickZ(1) );
			agk::Print( str );
		}

		float x = agk::GetSpriteXByOffset( 96 );
		float y = agk::GetSpriteYByOffset( 96 );

		//x += agk::GetDirectionX();
		//y += agk::GetDirectionY();

		//float angle = agk::GetSpriteAngle( 96 );
		//agk::Print( angle );
		//angle -= 1;

		x += agk::GetJoystickX();
		y += agk::GetJoystickY();

		if ( x < 0 ) x = 0;
		if ( x > agk::GetVirtualWidth() ) x = (float) agk::GetVirtualWidth();

		if ( y < 0 ) y = 0;
		if ( y > agk::GetVirtualHeight() ) y = (float) agk::GetVirtualHeight();

		agk::SetSpritePositionByOffset( 96, x, y );
		//agk::SetSpriteAngle( 96, angle );

		if ( agk::GetTextInputCompleted() && !agk::GetTextInputCancelled() )
		{
			uString str;
			agk::GetTextInput( str );
			agk::SetTextString( 1, str );
		}

		// shared variables test
		/*
		g_Network1.SetLocalVariableF( "PosX", agk::GetPointerX(), 0 );
		g_Network1.SetLocalVariableF( "PosY", agk::GetPointerY(), 0 );

		int clients = g_Network1.GetNumClients();
		agk::PrintC( "Num Clients: " ); agk::Print( clients );
		if ( clients > 0 )
		{
			int clientID = g_Network1.GetFirstClient();
			while ( clientID > 0 )
			{
				if ( g_Network1.GetClientDisconnected( clientID ) == 1 )
				{
					// clean up client data
					UINT iSprite = g_Network1.GetClientUserData( clientID, 0 );
					if ( iSprite > 0 )
					{
						if ( agk::GetSpriteExists( iSprite ) ) agk::DeleteSprite( iSprite );
						g_Network1.SetClientUserData( clientID, 0, 0 ); // make sure we don't try deleting the sprite again
					}

					UINT iText = g_Network1.GetClientUserData( clientID, 1 );
					if ( iText > 0 )
					{
						if ( agk::GetTextExists( iText ) ) agk::DeleteText( iText );
						g_Network1.SetClientUserData( clientID, 1, 0 );
					}

					// this will not delete immediately, but signal the client for deletion. Assume this "client disconnected" code will be triggered again before it is finally deleted.
					g_Network1.DeleteDisconnectedClient( clientID );
				}
				else
				{
					int iUserData = g_Network1.GetClientUserData( clientID, 0 );
					if ( iUserData == 0 )
					{
						// new client
						// add sprite to client user data
						UINT iSprite = agk::CreateSprite( iMole );
						agk::SetSpriteSize( iSprite, 10, -1 );
						agk::SetSpriteAnimation( iSprite, 82, 88, 11 );
						agk::SetSpriteFrame( iSprite, 1 );
						g_Network1.SetClientUserData( clientID, 0, iSprite );

						uString sName;
						g_Network1.GetClientName( clientID, sName );

						// add text to user data
						UINT iText = agk::CreateText( sName );
						//agk::SetTextSpacing( iText, -2.5f );
						agk::SetTextSize( iText, 3.5f );
						agk::SetTextColor( iText, 0,0,0 );
						agk::SetTextAlignment( iText, 1 ); // center
						g_Network1.SetClientUserData( clientID, 1, iText );

						//cNetworkMessage *pMsg = new cNetworkMessage();
						//pMsg->AddString("i have app");
						//g_Network1.SendMessage( clientID, pMsg );
					}

					uString sName;
					g_Network1.GetClientName( clientID, sName );
					float ping = g_Network1.GetClientPing( clientID );

					uString msg;
					msg.Format( "%s: %.1fms", sName.GetStr(), ping*1000 );
					agk::Print( msg );

					float x = g_Network1.GetClientVariableF( clientID, "PosX" );
					float y = g_Network1.GetClientVariableF( clientID, "PosY" );

					UINT iSprite = g_Network1.GetClientUserData( clientID, 0 );
					UINT iText = g_Network1.GetClientUserData( clientID, 1 );

					if ( iSprite > 0 )
					{
						agk::SetSpritePositionByOffset( iSprite, x, y );
					}
					if ( iText > 0 ) agk::SetTextPosition( iText, x, y+4 );
				}

				// GetNextClient() must be called until it returns 0 or a deadlock in the network code will result
				clientID = g_Network1.GetNextClient();
			}
		}

		cNetworkMessage *pMsg = g_Network1.GetMessage();
		while ( pMsg )
		{
			UINT clientID = pMsg->GetSenderID();
			uString sName;
			g_Network1.GetClientName( clientID, sName );

			uString sText;
			pMsg->GetString( sText );

			uString sMsg;
			sMsg.Format( "Received message from: %s, %s", sName.GetStr(), sText.GetStr() );
			agk::Warning( sMsg );

			delete pMsg;
			pMsg = g_Network1.GetMessage();
		}
		*/

		static int iLastHit = 0;

		// pointer code
		if ( agk::GetPointerPressed() == 1 )
		{
			float fX = agk::GetPointerX();
			float fY = agk::GetPointerY();

			// find which mole is hit, if any
			for ( int i = 0; i < 9; i++ )
			{
				// only matters if the mole is moving or waiting at the top
				if ( g_sMoles[ i ].eState == sMole::eBottom ) continue;
				if ( g_sMoles[ i ].bHit ) continue;

				int iSpriteID = g_sMoles[ i ].iSpriteID;

				if ( agk::GetSpriteHitTest(iSpriteID, fX, fY) )
				{
					switch( g_sMoles[ i ].eState )
					{
						case sMole::eRising: iScore += 100; break;
						case sMole::eTop: iScore += 50; break;
						case sMole::eFalling: iScore += 20; break;
					}

					//sprintf( str, "Score: %d", iScore );
					//Output( str );

					agk::PlaySprite( iSpriteID, 3, 1, 5, 7 );
					g_sMoles[ i ].eState = sMole::eFalling;
					g_sMoles[ i ].bHit = true;
				}
			}

			iLastHit = agk::GetSpriteHitTest( 97, fX, fY );

			// impulse test
			agk::SetSpritePhysicsImpulse( 99, agk::GetSpriteXByOffset(99), agk::GetSpriteYByOffset(99), 0, -5000);

			//agk::PlaySound( 1 );

			//agk::StartTextInput();

			//cNetworkMessage *msg = new cNetworkMessage();
			//msg->AddString( "Test Message 2" );
			//g_Network1.SendMessage( 0, msg );
		}

		// update moles
		for ( int i = 0; i < 9; i++ )
		{
			// Rising
			if ( g_sMoles[ i ].eState == sMole::eRising )
			{
				int iSpriteID = g_sMoles[ i ].iSpriteID;
				int iGridID = g_sMoles[ i ].iGridPosition;

				float x = agk::GetSpriteX( iSpriteID );
				float y = agk::GetSpriteY( iSpriteID );
				if ( y > g_sGridPositionsTop[ iGridID ].y ) agk::SetSpritePosition( iSpriteID, x, y - g_sMoles[ i ].fSpeed * agk::GetFrameTime() );
				else
				{
					g_sMoles[ i ].eState = sMole::eTop;
					g_sMoles[ i ].fWaitTimer = 4.0f;
					agk::PlaySprite( iSpriteID, 3.0f, 1, 1, 4 );
				}
			}

			// Waiting at the top
			if ( g_sMoles[ i ].eState == sMole::eTop )
			{
				g_sMoles[ i ].fWaitTimer -= agk::GetFrameTime();
				if ( g_sMoles[ i ].fWaitTimer <= 0 )
				{
					g_sMoles[ i ].fWaitTimer = 0;
					g_sMoles[ i ].eState = sMole::eFalling;
				}
			}

			// Falling
			if ( g_sMoles[ i ].eState == sMole::eFalling )
			{
				int iSpriteID = g_sMoles[ i ].iSpriteID;
				int iGridID = g_sMoles[ i ].iGridPosition;

				float x = agk::GetSpriteX( iSpriteID );
				float y = agk::GetSpriteY( iSpriteID );
				if ( y < g_sGridPositions[ iGridID ].y ) agk::SetSpritePosition( iSpriteID, x, y + g_sMoles[ i ].fSpeed * agk::GetFrameTime() );
				else
				{
					g_sMoles[ i ].eState = sMole::eBottom;
					agk::StopSprite( iSpriteID );
					g_sMoles[ i ].bHit = false;
					agk::SetSpriteFrame( iSpriteID, 1 );
				}
			}

			// Waiting at the bottom
			if ( g_sMoles[ i ].eState == sMole::eBottom )
			{
				int iRnd = rand() % 6000;
				if ( iRnd <= 10 )
				{
					g_sMoles[ i ].eState = sMole::eRising;
					agk::PlaySprite( g_sMoles[ i ].iSpriteID, 3.0f, 1, 8, 11 );
				}
			}
		}

		agk::Sync();
	}

	void updatesize()
	{
		agk::Warning( "Changing device size" );
		agk::UpdateDeviceSize();
	}

	void setaccel( float x, float y, float z )
	{
		x /= 10.0f;
		y /= 10.0f;
		z /= 10.0f;

		// check orientation
		static int lastmode = 0;
		int mode = 0;
		if ( agk::Abs(y) > agk::Abs(x) + 0.2 )
		{
			if ( y < -0.4 ) mode = 2;
			if ( y > 0.4 ) mode = 1;
		}
		else if ( agk::Abs(x) > agk::Abs(y) + 0.2 )
		{
			if ( x < -0.4 ) mode = 4;
			if ( x > 0.4 ) mode = 3;
		}

		if ( mode > 0 && mode != lastmode ) agk::OrientationChanged( mode );
		lastmode = mode;

		agk::Accelerometer( -x, y, z );
		//uString str;
		//str.Format( "X: %.2f, Y: %.2f, Z: %.2f", -x, y, z );
		//agk::Warning( str );
	}

	void touchdown( int id, float x, float y )
	{
		agk::TouchPressed( id+1, x, y );
	}

	void touchmoved( int id, float x, float y )
	{
		agk::TouchMoved( id+1, x, y );
	}

	void touchup( int id, float x, float y )
	{
		agk::TouchReleased( id+1, x, y );
	}

	int TranslateKey( int key )
	{
		switch( key )
		{
			case AKEYCODE_DEL: return AGK_KEY_BACK;
			case AKEYCODE_TAB: return AGK_KEY_TAB;
			case AKEYCODE_ENTER: return AGK_KEY_ENTER;
			case AKEYCODE_SHIFT_LEFT: return AGK_KEY_SHIFT;
			case AKEYCODE_SHIFT_RIGHT: return AGK_KEY_SHIFT;
			//case : return AGK_KEY_CONTROL;
			case AKEYCODE_BACK: return AGK_KEY_ESCAPE;
			case AKEYCODE_SPACE: return AGK_KEY_SPACE;
			case AKEYCODE_PAGE_UP: return AGK_KEY_PAGEUP;
			case AKEYCODE_PAGE_DOWN: return AGK_KEY_PAGEDOWN;
			//case : return AGK_KEY_END;
			//case : return AGK_KEY_HOME;
			case AKEYCODE_DPAD_LEFT: return AGK_KEY_LEFT;
			case AKEYCODE_DPAD_UP: return AGK_KEY_UP;
			case AKEYCODE_DPAD_RIGHT: return AGK_KEY_RIGHT;
			case AKEYCODE_DPAD_DOWN: return AGK_KEY_DOWN;
			//case : return AGK_KEY_INSERT;
			//case : return AGK_KEY_DELETE;

			case AKEYCODE_0: return AGK_KEY_0;
			case AKEYCODE_1: return AGK_KEY_1;
			case AKEYCODE_2: return AGK_KEY_2;
			case AKEYCODE_3: return AGK_KEY_3;
			case AKEYCODE_4: return AGK_KEY_4;
			case AKEYCODE_5: return AGK_KEY_5;
			case AKEYCODE_6: return AGK_KEY_6;
			case AKEYCODE_7: return AGK_KEY_7;
			case AKEYCODE_8: return AGK_KEY_8;
			case AKEYCODE_9: return AGK_KEY_9;

			case AKEYCODE_A: return AGK_KEY_A;
			case AKEYCODE_B: return AGK_KEY_B;
			case AKEYCODE_C: return AGK_KEY_C;
			case AKEYCODE_D: return AGK_KEY_D;
			case AKEYCODE_E: return AGK_KEY_E;
			case AKEYCODE_F: return AGK_KEY_F;
			case AKEYCODE_G: return AGK_KEY_G;
			case AKEYCODE_H: return AGK_KEY_H;
			case AKEYCODE_I: return AGK_KEY_I;
			case AKEYCODE_J: return AGK_KEY_J;
			case AKEYCODE_K: return AGK_KEY_K;
			case AKEYCODE_L: return AGK_KEY_L;
			case AKEYCODE_M: return AGK_KEY_M;
			case AKEYCODE_N: return AGK_KEY_N;
			case AKEYCODE_O: return AGK_KEY_O;
			case AKEYCODE_P: return AGK_KEY_P;
			case AKEYCODE_Q: return AGK_KEY_Q;
			case AKEYCODE_R: return AGK_KEY_R;
			case AKEYCODE_S: return AGK_KEY_S;
			case AKEYCODE_T: return AGK_KEY_T;
			case AKEYCODE_U: return AGK_KEY_U;
			case AKEYCODE_V: return AGK_KEY_V;
			case AKEYCODE_W: return AGK_KEY_W;
			case AKEYCODE_X: return AGK_KEY_X;
			case AKEYCODE_Y: return AGK_KEY_Y;
			case AKEYCODE_Z: return AGK_KEY_Z;

			case AKEYCODE_GRAVE: return 223; break; // `
			case AKEYCODE_MINUS: return 189; break; // -
			case AKEYCODE_EQUALS: return 187; break; // =
			case AKEYCODE_LEFT_BRACKET: return 219; break; // [
			case AKEYCODE_RIGHT_BRACKET: return 221; break; // ]
			case AKEYCODE_SEMICOLON: return 186; break; // ;
			case AKEYCODE_APOSTROPHE: return 192; break; // '
			case AKEYCODE_COMMA: return 188; break; // ,
			case AKEYCODE_PERIOD: return 190; break; // .
			case AKEYCODE_SLASH: return 191; break; // /
			case AKEYCODE_BACKSLASH: return 220; break; // \

			default: return 0;
		}

		return 0;
	}

	int AsciiKey( int key )
	{
		if ( agk::GetRawKeyState( AGK_KEY_SHIFT ) == 1 )
		{
			// shift pressed
			switch( key )
			{
				case AKEYCODE_0: return ')';
				case AKEYCODE_1: return '!';
				case AKEYCODE_2: return '"';
				case AKEYCODE_3: return '£';
				case AKEYCODE_4: return '$';
				case AKEYCODE_5: return '%';
				case AKEYCODE_6: return '^';
				case AKEYCODE_7: return '&';
				case AKEYCODE_8: return '*';
				case AKEYCODE_9: return '(';

				case AKEYCODE_A: return 'A';
				case AKEYCODE_B: return 'B';
				case AKEYCODE_C: return 'C';
				case AKEYCODE_D: return 'D';
				case AKEYCODE_E: return 'E';
				case AKEYCODE_F: return 'F';
				case AKEYCODE_G: return 'G';
				case AKEYCODE_H: return 'H';
				case AKEYCODE_I: return 'I';
				case AKEYCODE_J: return 'J';
				case AKEYCODE_K: return 'K';
				case AKEYCODE_L: return 'L';
				case AKEYCODE_M: return 'M';
				case AKEYCODE_N: return 'N';
				case AKEYCODE_O: return 'O';
				case AKEYCODE_P: return 'P';
				case AKEYCODE_Q: return 'Q';
				case AKEYCODE_R: return 'R';
				case AKEYCODE_S: return 'S';
				case AKEYCODE_T: return 'T';
				case AKEYCODE_U: return 'U';
				case AKEYCODE_V: return 'V';
				case AKEYCODE_W: return 'W';
				case AKEYCODE_X: return 'X';
				case AKEYCODE_Y: return 'Y';
				case AKEYCODE_Z: return 'Z';

				case AKEYCODE_COMMA: return '<';
				case AKEYCODE_PERIOD: return '>';
				case AKEYCODE_MINUS: return '_';
				case AKEYCODE_EQUALS: return '+';
				case AKEYCODE_LEFT_BRACKET: return '{';
				case AKEYCODE_RIGHT_BRACKET: return '}';
				case AKEYCODE_BACKSLASH: return '|';
				case AKEYCODE_SEMICOLON: return ':';
				case AKEYCODE_APOSTROPHE: return '@';
				case AKEYCODE_SLASH: return '?';

				default: return 0;
			}
		}
		else
		{
			// shift not pressed
			switch( key )
			{
				case AKEYCODE_0: return '0';
				case AKEYCODE_1: return '1';
				case AKEYCODE_2: return '2';
				case AKEYCODE_3: return '3';
				case AKEYCODE_4: return '4';
				case AKEYCODE_5: return '5';
				case AKEYCODE_6: return '6';
				case AKEYCODE_7: return '7';
				case AKEYCODE_8: return '8';
				case AKEYCODE_9: return '9';

				case AKEYCODE_A: return 'a';
				case AKEYCODE_B: return 'b';
				case AKEYCODE_C: return 'c';
				case AKEYCODE_D: return 'd';
				case AKEYCODE_E: return 'e';
				case AKEYCODE_F: return 'f';
				case AKEYCODE_G: return 'g';
				case AKEYCODE_H: return 'h';
				case AKEYCODE_I: return 'i';
				case AKEYCODE_J: return 'j';
				case AKEYCODE_K: return 'k';
				case AKEYCODE_L: return 'l';
				case AKEYCODE_M: return 'm';
				case AKEYCODE_N: return 'n';
				case AKEYCODE_O: return 'o';
				case AKEYCODE_P: return 'p';
				case AKEYCODE_Q: return 'q';
				case AKEYCODE_R: return 'r';
				case AKEYCODE_S: return 's';
				case AKEYCODE_T: return 't';
				case AKEYCODE_U: return 'u';
				case AKEYCODE_V: return 'v';
				case AKEYCODE_W: return 'w';
				case AKEYCODE_X: return 'x';
				case AKEYCODE_Y: return 'y';
				case AKEYCODE_Z: return 'z';

				case AKEYCODE_STAR: return '*';
				case AKEYCODE_POUND: return '#';
				case AKEYCODE_COMMA: return ',';
				case AKEYCODE_PERIOD: return '.';
				case AKEYCODE_MINUS: return '-';
				case AKEYCODE_EQUALS: return '=';
				case AKEYCODE_LEFT_BRACKET: return '[';
				case AKEYCODE_RIGHT_BRACKET: return ']';
				case AKEYCODE_BACKSLASH: return '\\';
				case AKEYCODE_SEMICOLON: return ';';
				case AKEYCODE_APOSTROPHE: return '\'';
				case AKEYCODE_SLASH: return '/';
				case AKEYCODE_AT: return '@';
				case AKEYCODE_PLUS: return '+';

				default: return 0;
			}
		}
	}

	void keydown( int key )
	{
		agk::KeyDown( TranslateKey(key) );
		int ch = AsciiKey(key);
		if ( ch != 0 )
		{
			agk::CharDown( ch );
		}
	}

	void keyup( int key )
	{
		agk::KeyUp( TranslateKey(key) );
	}
}

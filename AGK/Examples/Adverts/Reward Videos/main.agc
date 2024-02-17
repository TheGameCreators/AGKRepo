
// set window properties
SetWindowTitle( "Reward Videos" )
SetWindowSize( 1024, 768, 0 )

SetVirtualResolution(1024,768)
UseNewDefaultFonts(1)

// Must request consent for personalised ads from EU users for GDPR compliance, 
// this will do nothing for other users. If users refuse then AdMob ads will be non-personalised.
LoadConsentStatusAdMob( "", "" )
while( GetConsentStatusAdMob() < 0 )
	Sync()
endwhile
if ( GetConsentStatusAdMob() = 0 ) 
	RequestConsentAdMob()
	while( GetConsentStatusAdMob() < 0 )
		Sync()
	endwhile
endif

SetAdMobTesting(1) // this may not work on Android for reward videos

if ( GetDeviceBaseName() = "ios" )
	SetChartboostDetails( "543ff063c26ee43689496a1e", "098f0986d98b5871f65860dd6f054f46562dd2b4" ) // iOS reward video
	SetAdMobRewardAdDetails( "ca-app-pub-6042722625366627/5278320966" ) // iOS Reward Video
elseif ( GetDeviceBaseName() = "android" )
	SetChartboostDetails( "543d4c45c26ee42c70b4064b", "1d29ce7123264bd22f660dbf611576d7ac3703c7" ) // Android reward video
	SetAdMobRewardAdDetails( "ca-app-pub-6042722625366627/7082908567" ) // Android reward video
endif

// tell Chartboost we will be using reward videos, AdMob does this automatically when we set the details
CacheRewardAdChartboost()

AddVirtualButton( 1, 120, 768-120, 200 )
SetVirtualButtonText( 1, 'AdMob\nReward' )

AddVirtualButton( 2, 1024-120, 768-120, 200 )
SetVirtualButtonText( 2, 'Chartboost\nReward' )

AddVirtualButton( 3, 512, 768-100, 180 )
SetVirtualButtonText( 3, 'Reset\nRewards' )

AddVirtualButton( 6, 1024-100, 100, 180 )
SetVirtualButtonText( 6, 'Consent' )

do
	if ( GetVirtualButtonReleased(1) ) then ShowRewardAdAdMob()
	if ( GetVirtualButtonReleased(2) ) then ShowRewardAdChartboost()
	if ( GetVirtualButtonReleased(3) ) 
		ResetRewardAdMob()
		ResetRewardChartboost()
	endif
	if ( GetVirtualButtonReleased(6) ) then RequestConsentAdMob()
	
	print( "FPS: " + str(ScreenFPS()) )
	print( "Consent Status: " + str(GetConsentStatusAdMob()) )
	print( "AdMob Reward Ad Loaded: " + str(GetRewardAdLoadedAdMob()) )
	print( "Chartboost Reward Ad Loaded: " + str(GetRewardAdLoadedChartboost()) )
	print( "" )
	print( "AdMob Rewarded: " + str(GetRewardAdRewardedAdMob()) )
	print( "AdMob Rewarded Value: " + str(GetRewardAdValueAdMob()) )
	print( "Chartboost Rewarded: " + str(GetRewardAdRewardedChartboost()) )	
		
	sync()
loop

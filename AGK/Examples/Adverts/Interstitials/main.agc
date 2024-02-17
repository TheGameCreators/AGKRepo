
// set window properties
SetWindowTitle( "Interstitials" )
SetWindowSize( 1024, 768, 0 )

SetVirtualResolution(1024,768)
UseNewDefaultFonts(1)

// Must request consent for personalised ads from EU users for GDPR compliance, 
// this will do nothing for other users. If users refuse then AdMob ads will be non-personalised.
// If you do not request consent then all AdMob ads will default to non-personalised
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

SetAdMobTesting(1)

if ( GetDeviceBaseName() = "ios" )
	SetChartboostDetails( "543ff063c26ee43689496a1e", "098f0986d98b5871f65860dd6f054f46562dd2b4" ) // iOS interstitial
	SetAdMobDetails( "ca-app-pub-6042722625366627/5617595769" ) // iOS interstitial
elseif ( GetDeviceBaseName() = "android" )
	SetChartboostDetails( "543d4c45c26ee42c70b4064b", "1d29ce7123264bd22f660dbf611576d7ac3703c7" ) // Android interstitial
	SetAdMobDetails( "ca-app-pub-6042722625366627/9628992965" ) // Android interstitial
endif

AddVirtualButton( 1, 120, 768-120, 200 )
SetVirtualButtonText( 1, "AdMob" )

AddVirtualButton( 2, 512, 768-120, 200 )
SetVirtualButtonText( 2, "Chartboost" )

AddVirtualButton( 6, 1024-100, 100, 180 )
SetVirtualButtonText( 6, 'Consent' )

do
	if ( GetVirtualButtonReleased(1) ) then ShowFullscreenAdvertAdMob()
	if ( GetVirtualButtonReleased(2) ) then ShowFullscreenAdvertChartboost()
	if ( GetVirtualButtonReleased(6) ) then RequestConsentAdMob()
	
	Print( "FPS: " + str(ScreenFPS()) )
	Print( "AdMob Loaded: " + str(GetFullscreenAdvertLoadedAdMob()) )
	Print( "Chartboost Loaded: " + str(GetFullscreenAdvertLoadedChartboost()) )
	print( "AdMob Consent Status: " + str(GetConsentStatusAdMob()) )
	Sync()
loop

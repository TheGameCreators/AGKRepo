
// show all errors
SetErrorMode(2)

// set window properties
SetWindowTitle( "InAppPurchase" )
SetWindowSize( 768, 1024, 0 )
SetVirtualResolution( 768, 1024 )
SetOrientationAllowed( 1, 1, 1, 1 )
UseNewDefaultFonts( 1 )
SetPrintSize( 30 )

// all products must be added before calling InAppPurchaseSetup
if ( GetDeviceBaseName() = "ios" )
	InAppPurchaseAddProductID( "com.thegamecreators.agk_player.iap_test_consumable", 0 )
	InAppPurchaseAddProductID( "com.thegamecreators.agk_player.iap_test_subscription", 2 )
else
	InAppPurchaseAddProductID( "agk_test", 0 )
	InAppPurchaseAddProductID( "agk_player.iap_test_subscription", 2 )
endif

InAppPurchaseSetup()


CreateText( 1, "Start Subscription With Plan" )
SetTextSize( 1, 40 )
SetTextPosition( 1, 384, 700 )
SetTextAlignment( 1, 1 )

for i = 1 to 8
	buttonID = 10 + i
	AddVirtualButton( buttonID, 20 + (i * 80), 800, 70 )
	SetVirtualButtonSize( buttonID, 70, 70 )
	SetVirtualButtonText( buttonID, str(i) )
next i

AddVirtualButton( 1, 192, 950, 100 )
SetVirtualButtonSize( 1, 160, 100 )
SetVirtualButtonText( 1, 'Purchase\nConsumable' )

AddVirtualButton( 3, 384, 950, 100 )
SetVirtualButtonSize( 3, 160, 100 )
SetVirtualButtonText( 3, 'Reset\nConsumable' )

AddVirtualButton( 4, 576, 950, 100 )
SetVirtualButtonSize( 4, 160, 100 )
SetVirtualButtonText( 4, 'Restore\nPurchases' )

do
	Print( "FPS: " + str(ScreenFPS()) )
	
	// display some information
    Print( "Consumable: " + GetInAppPurchaseDescription(0) )
    Print( "    Price: " + GetInAppPurchaseLocalPrice(0) + ", State: " + str(GetInAppPurchaseAvailable2(0)) )
    Print( "Subscription: " + GetInAppPurchaseDescription(1) )
    Print( "    State: " + str(GetInAppPurchaseAvailable2(1)) )
    numPlans = GetInAppPurchaseSubNumPlans(1)
    Print( "    Num Plans: " + str(numPlans) )
    
    for i = 0 to numPlans-1
    	Print( "    Plan " + str(i+1) )
    	Print( "        Tags: " + GetInAppPurchaseSubPlanTags(1,i) )
    	Print( "        " + FormatSubscriptionPlan( 1, i ) )
    next i
    
    if ( GetVirtualButtonReleased(1) ) then InAppPurchaseActivate(0) // purchase item
	if ( GetVirtualButtonReleased(3) and GetInAppPurchaseAvailable2(0) = 4 ) // reset purchase 
		InAppPurchaseResetPurchase( GetInAppPurchaseToken(0) )
	endif
	if ( GetVirtualButtonReleased(4) ) then InAppPurchaseRestore()
	
	for i = 1 to 8
		buttonID = 10 + i
		if ( i > numPlans )
			SetVirtualButtonActive( buttonID, 0 )
		else
			SetVirtualButtonActive( buttonID, 1 )
			if ( GetVirtualButtonReleased(buttonID) ) 
				planToken$ = GetInAppPurchaseSubPlanToken( 1, i-1 )
				InAppPurchaseActivateWithPlan(1, planToken$) // purchase subscription
			endif
		endif   
	next i 
	
	Sync()
loop

function SubscriptionUnitName( unit as string )
	name as string
	name = "Unknown"
	select left( unit, 1 )
		case 'D'
			name = "Day"
		endcase
		case 'W'
			name = "Week"
		endcase
		case 'M'
			name = "Month"
		endcase
		case 'Y'
			name = "Year"
		endcase
	endselect
endfunction name

function SubscriptionUnitNum( unit as string )
	num = val( mid(unit,2,-1) )
endfunction num

function FormatSubscriptionPlan( productID as integer, planID as integer )
	output as string
	output = ""
	numPeriods = GetInAppPurchaseSubPlanNumPeriods( productID, planID )
	for i = 0 to numPeriods-1
		price$ = GetInAppPurchaseSubPlanPrice( productID, planID, i )
		unit$ = GetInAppPurchaseSubPlanDurationUnit( productID, planID, i )
		unitName$ = SubscriptionUnitName( unit$ )
		unitNum = SubscriptionUnitNum( unit$ )
		count = GetInAppPurchaseSubPlanDuration( productID, planID, i )
		payment = GetInAppPurchaseSubPlanPaymentType( productID, planID, i )
		select payment
			case 0 // free
				total = count * unitNum
				output = output + "Free for " + str(total) + " " + unitName$
				if ( total > 1 ) then output = output + "s"
			endcase
			case 1 // one off payment
				total = count * unitNum
				output = output + price$ + " for " + str(total) + " " + unitName$
				if ( total > 1 ) then output = output + "s"
			endcase
			case 2 // recurring for fixed number of times
				if ( count > 1 )
					output = output + price$ + " every "
					if ( unitNum > 1 ) 
						output = output + str(unitNum) + " " + unitName$ + "s"
					else
						output = output + unitName$
					endif
					total = count * unitNum
					output = output + " for " + str(total) + " " + unitName$ + "s"
				else
					output = output + price$ + " for " + str(unitNum) + " " + unitName$ 
					if ( unitNum > 1 ) then output = output + "s"
				endif
			endcase
			case 3 // recurring until cancelled
				output = output + price$ + " every "
				if ( unitNum > 1 ) 
					output = output + str(unitNum) + " " + unitName$ + "s"
				else
					output = output + unitName$
				endif
			endcase
		endselect
		if ( i < numPeriods-1 )then output = output + " then "
	next i
endfunction output

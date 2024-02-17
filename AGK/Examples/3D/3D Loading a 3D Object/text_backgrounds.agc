// TextBackground Functions.  Usage:
// insert TextBackrounds.agc and call Init_Text_Backgrounds before using any of the other functions
// Default text background depth is one level below the text used when calling the function.

Type _type_TextBackground
	tbg_spr_topleft_ID 		as integer 
	tbg_spr_topright_ID		as integer
	tbg_spr_bottomleft_ID	as integer 
	tbg_spr_bottomright_ID	as integer
	tbg_spr_top_ID			as integer
	tbg_spr_bottom_ID		as integer
	tbg_spr_middle_ID		as integer
	tbg_width				as float
	tbg_height				as float
	tbg_corner_radius		as float
	tbg_deleted				as integer
EndType

//
// Initialise TextBackground code - This needs to be called before using any other TextBackground functions
//
Function Init_Text_Backgrounds ()
	Global Text_Background_Corner as integer
	Text_Background_Corner = LoadImage("corner.png")
	// TextBackgroundCounter is a variable used to track the number of text backgrounds created
	Global TextBackgroundCounter as integer
	TextBackgroundCounter = 0
	Global MyTextBackground as _type_TextBackground []
EndFunction

Function CreateTextBackground(txtID as integer, rounded_corners as integer, radius# as float, padding# as float, alpha as float)
	// As we are creating a new textbackground we must increase the counter by 1.
	inc TextBackgroundCounter

    // Increase the length of the MyTextBackground array (we could have also inserted a new MyTextBackground onto the end of the array using .insert
	// but either method works just fine).
	MyTextBackground.length = TextBackgroundCounter

	textwidth# = GetTextTotalWidth(txtID)
	textheight# = GetTextTotalHeight(txtID) * 0.8
	GTY_plus_offset# = GetTextY(txtID)+GetTextTotalHeight(txtID) * 0.15
	

	MyTextBackground[TextBackgroundCounter].tbg_width = textwidth#+(padding# * 2.0)

	// create and position the four corners (if rounded corners are required
	if rounded_corners = 1

		MyTextBackground[TextBackgroundCounter].tbg_spr_topleft_ID = createsprite(Text_Background_Corner)
		SetSpriteDepth(MyTextBackground[TextBackgroundCounter].tbg_spr_topleft_ID,GetTextDepth(txtID)+1)
		SetSpriteSize(MyTextBackground[TextBackgroundCounter].tbg_spr_topleft_ID,radius#,radius#)
		SetSpritePosition(MyTextBackground[TextBackgroundCounter].tbg_spr_topleft_ID,GetTextX(txtID)-padding#,GTY_plus_offset#-padding#)

		MyTextBackground[TextBackgroundCounter].tbg_spr_topright_ID = createsprite(Text_Background_Corner)
		SetSpriteDepth(MyTextBackground[TextBackgroundCounter].tbg_spr_topright_ID,GetTextDepth(txtID)+1)
		SetSpriteSize(MyTextBackground[TextBackgroundCounter].tbg_spr_topright_ID,radius#,radius#)
		SetSpriteFlip(MyTextBackground[TextBackgroundCounter].tbg_spr_topright_ID,1,0)
		SetSpritePosition(MyTextBackground[TextBackgroundCounter].tbg_spr_topright_ID,GetTextX(txtID)+textwidth#+padding#-radius#,GTY_plus_offset#-padding#)

		MyTextBackground[TextBackgroundCounter].tbg_spr_bottomleft_ID = createsprite(Text_Background_Corner)
		SetSpriteDepth(MyTextBackground[TextBackgroundCounter].tbg_spr_bottomleft_ID,GetTextDepth(txtID)+1)
		SetSpriteSize(MyTextBackground[TextBackgroundCounter].tbg_spr_bottomleft_ID,radius#,radius#)
		SetSpriteFlip(MyTextBackground[TextBackgroundCounter].tbg_spr_bottomleft_ID,0,1)
		SetSpritePosition(MyTextBackground[TextBackgroundCounter].tbg_spr_bottomleft_ID,GetTextX(txtID)-padding#,GTY_plus_offset#+textheight#+padding#-radius#)

		MyTextBackground[TextBackgroundCounter].tbg_spr_bottomright_ID = createsprite(Text_Background_Corner)
		SetSpriteDepth(MyTextBackground[TextBackgroundCounter].tbg_spr_bottomright_ID,GetTextDepth(txtID)+1)
		SetSpriteSize(MyTextBackground[TextBackgroundCounter].tbg_spr_bottomright_ID,radius#,radius#)
		SetSpriteFlip(MyTextBackground[TextBackgroundCounter].tbg_spr_bottomright_ID,1,1)
		SetSpritePosition(MyTextBackground[TextBackgroundCounter].tbg_spr_bottomright_ID,GetTextX(txtID)+textwidth#+padding#-radius#,GTY_plus_offset#+textheight#+padding#-radius#)
	endif

	// create the middle section
	MyTextBackground[TextBackgroundCounter].tbg_spr_middle_ID = createsprite(0)
	SetSpriteDepth(MyTextBackground[TextBackgroundCounter].tbg_spr_middle_ID,GetTextDepth(txtID)+1)
	SetSpriteSize(MyTextBackground[TextBackgroundCounter].tbg_spr_middle_ID,padding#+textwidth#+padding#,textheight#+((padding#-radius#)*2))
	SetSpritePosition(MyTextBackground[TextBackgroundCounter].tbg_spr_middle_ID,getTextX(txtID)-padding#,GTY_plus_offset#-padding#+radius#)

	// create the top section
	MyTextBackground[TextBackgroundCounter].tbg_spr_top_ID = createsprite(0)
	SetSpriteDepth(MyTextBackground[TextBackgroundCounter].tbg_spr_top_ID,GetTextDepth(txtID)+1)
	SetSpriteSize(MyTextBackground[TextBackgroundCounter].tbg_spr_top_ID,textwidth#+((padding#-radius#)*2),radius#)
	SetSpritePosition(MyTextBackground[TextBackgroundCounter].tbg_spr_top_ID,getTextX(txtID)-padding#+radius#,GTY_plus_offset#-padding#)

	// create the bottom section
	MyTextBackground[TextBackgroundCounter].tbg_spr_bottom_ID = createsprite(0)
	SetSpriteDepth(MyTextBackground[TextBackgroundCounter].tbg_spr_bottom_ID,GetTextDepth(txtID)+1)
	SetSpriteSize(MyTextBackground[TextBackgroundCounter].tbg_spr_bottom_ID,textwidth#+((padding#-radius#)*2),radius#)
	SetSpritePosition(MyTextBackground[TextBackgroundCounter].tbg_spr_bottom_ID,getTextX(txtID)-padding#+radius#,GTY_plus_offset#+textheight#+padding#-radius#)

	// set background alpha to required level
	SetSpriteColorAlpha(MyTextBackground[TextBackgroundCounter].tbg_spr_bottom_ID,alpha)
	SetSpriteColorAlpha(MyTextBackground[TextBackgroundCounter].tbg_spr_topleft_ID,alpha)
	SetSpriteColorAlpha(MyTextBackground[TextBackgroundCounter].tbg_spr_topright_ID,alpha)
	SetSpriteColorAlpha(MyTextBackground[TextBackgroundCounter].tbg_spr_bottomleft_ID,alpha)
	SetSpriteColorAlpha(MyTextBackground[TextBackgroundCounter].tbg_spr_bottomright_ID,alpha)
	SetSpriteColorAlpha(MyTextBackground[TextBackgroundCounter].tbg_spr_middle_ID,alpha)
	SetSpriteColorAlpha(MyTextBackground[TextBackgroundCounter].tbg_spr_top_ID,alpha)

endfunction TextBackgroundCounter

Function SetTextBackgroundColor(txtbgID as integer, red as integer, green as integer, blue as integer, alpha as integer)
	result = 0
	if txtbgID <= MyTextBackground.length and txtbgID > 0
		if MyTextBackground[txtbgID].tbg_deleted = 0
			SetSpriteColor(MyTextBackground[TextBackgroundCounter].tbg_spr_bottom_ID,red,green,blue,alpha)
			SetSpriteColor(MyTextBackground[TextBackgroundCounter].tbg_spr_topleft_ID,red,green,blue,alpha)
			SetSpriteColor(MyTextBackground[TextBackgroundCounter].tbg_spr_topright_ID,red,green,blue,alpha)
			SetSpriteColor(MyTextBackground[TextBackgroundCounter].tbg_spr_bottomleft_ID,red,green,blue,alpha)
			SetSpriteColor(MyTextBackground[TextBackgroundCounter].tbg_spr_bottomright_ID,red,green,blue,alpha)
			SetSpriteColor(MyTextBackground[TextBackgroundCounter].tbg_spr_middle_ID,red,green,blue,alpha)
			SetSpriteColor(MyTextBackground[TextBackgroundCounter].tbg_spr_top_ID,red,green,blue,alpha)
			result = 1
		endif
	endif
endfunction result

function DeleteTextBackground(txtbgID as integer)
	// delete the sprites for txtbgID.  We are not removing the array element from the Mytextbackground array as it is 
	// unlikely any app will need so many text backgrounds that removing them from the array is warranted.
	// This means the MyTextBackground will continue to increase as new textbackgrounds are created, but is actually faster than deleting and tracking array elements.
	result = 0
	if txtbgID <= MyTextBackground.length and txtbgID > 0
		if MyTextBackground[txtbgID].tbg_deleted = 0
			DeleteSprite(MyTextBackground[txtbgID].tbg_spr_bottom_ID)
			DeleteSprite(MyTextBackground[txtbgID].tbg_spr_topleft_ID)
			DeleteSprite(MyTextBackground[txtbgID].tbg_spr_topright_ID)
			DeleteSprite(MyTextBackground[txtbgID].tbg_spr_bottomleft_ID)
			DeleteSprite(MyTextBackground[txtbgID].tbg_spr_bottomright_ID)
			DeleteSprite(MyTextBackground[txtbgID].tbg_spr_middle_ID)
			DeleteSprite(MyTextBackground[txtbgID].tbg_spr_top_ID)
			result = 1
			MyTextBackground[txtbgID].tbg_deleted = 1
		endif
	endif
endfunction result


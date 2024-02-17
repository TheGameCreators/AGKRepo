
// Project: Floating Squares
// Created: 2014-10-17
// By: Magnus Esko

/*
This project demonstrates how to..
	..use the virtual resolution to handle multiple resolutions
	..code for varying framerates
	..use an array to manipulate multiple sprites
	..read and apply accelerometer data
	..detect multiple touches
	..render to a render-image

This is a very physical demonstration and is best to test on a phone or tablet.
But there are also keyboard and mouse controls so you can test this on your PC/Mac.

A bunch of colorful squares are randomly created and you can move them around
by tilting your phone or touching the screen. The squares are rendered to a separate
image over and over, to create colorful trails. Try playing around with it and you
will discover various ways to manipulate the squares.

On PC/Mac you can use the arrow keys to simulate tilting and the mouse to simulate touiching.

Because we are using multitouch we need to check for touch and mouse separately.
Otherwise AGK has pointer commands that accepts both single touch and mouse.
*/


// you can play around with these constant values to alter the result
#constant number_of_squares 200
#constant friction 50
#constant finger_force 100
#constant gravity 100



// -------------------------------------------------------------------------------------------------------------
// Types
// -------------------------------------------------------------------------------------------------------------

// a type to handle various data
type datatype
	id
	size
	x as float
	y as float
	speedx as float
	speedy as float
	forcex as float
	forcey as float
endtype



// -------------------------------------------------------------------------------------------------------------
// Setting up the screen
// -------------------------------------------------------------------------------------------------------------

// naming our window on PC/Mac
SetWindowTitle( "Floating Squares" )

// variable to store the screen aspect
global aspect as float
aspect = GetDisplayAspect()

// variable to store the virtual resolution
// we use a virtual resolution that is 1000 pixels high
// and use the screen aspect to calculate the virtual with
global screen as datatype
screen.x = 1000*aspect
screen.y = 1000

// using a virtual resolution makes things a lot easier, works regardless of actual screen resolution
// this ensures that whatever we create is scaled to fit the screen
// since we use a variable virtual with, a wide screen like on PC/Mac will have more room
// if we didn't allow for either a variable with or hight, we would get either a stretched image or black borders
SetVirtualResolution( screen.x, screen.y )

// creating a render image and sprite to use for motion blur
// mobile devices are limited in image sizes, they must be a power by 2 value on each axis
// maximum size on each axis is usually 1024, on a PC or Mac you are not limited like this
// here we select between two different ones depending on if the screen is wide or tall
if screen.x > screen.y
	renderimage = createrenderimage( 1024, 512, 0, 0 )
else
	renderimage = createrenderimage( 512, 1024, 0, 0 )
endif
rendersprite = createsprite(0)
setspriteimage( rendersprite, renderimage )

// this sprite fills the render image with black to fade out the bluring
black = CreateSprite( 0 )
setspritesize( black, screen.x, screen.y )
SetSpriteColor( black, 0, 0, 0, 0 )
// we set it at a high depth to ensure it's behind everything else
SetSpriteDepth( black, 1000 )



// -------------------------------------------------------------------------------------------------------------
// Creating sprites
// -------------------------------------------------------------------------------------------------------------

// creating an array to store values for our squares
dim square[number_of_squares] as datatype

// setting up sprites
for i = 1 to square.length
	// creating aprites
	square[i].id = CreateSprite( 0 )

	// randomly sizing them
	square[i].size = random(10,25)
	SetSpriteSize( square[i].id, square[i].size, square[i].size )

	// bigger squares on top of smaller ones gives a deapth feeling
	SetSpriteDepth( square[i].id, 100-square[i].size )

	// this function places all squares on the screen
	resetsquares()

	// coloring the sprites in different colors, this code ensures they are bright but not white
	SetSpriteColor(square[i].id, random( 100, 230 ), random( 100, 230 ), random( 100, 230 ), 255 )
	temp = random( 1, 3 )
	select temp
		case 1 : SetSpriteColorRed( square[i].id, 255 ) : endcase
		case 2 : SetSpriteColorGreen( square[i].id, 255 ) : endcase
		case 3 : SetSpriteColorBlue( square[i].id, 255 ) : endcase
	endselect
next i



// -------------------------------------------------------------------------------------------------------------
// Declaring other variables
// -------------------------------------------------------------------------------------------------------------

// variable to store device tilting
global tilt as datatype

// variable to store the time it takes to do a loop
// this means we can make everything move the same speed even if the framerate drops
global looptime as float

// variable to store screen touching state
// state 0 = no touch
// state 1 = touch started outside reset area
// state 2 = touched reset area
global touchstate



// -------------------------------------------------------------------------------------------------------------
// Creating text
// -------------------------------------------------------------------------------------------------------------

// turn off filtering for the text to make it look crisp
SetTextDefaultMagFilter(0)

// instruction text at the top of the screen
global text_top
text_top = CreateText( "Tilt IT and Poke IT!" )
settextsize( text_top, 42 )
// alignment of 1 means the text is centered around it's position
SetTextAlignment( text_top, 1 )
// here we use our virtual with (screen.x) to place the text in the center
SetTextPosition( text_top, screen.x/2, 20 )
SetTextColor(text_top, 80, 80, 80, 255 )

// instruction text at the bottom
global text_bottom
text_bottom = CreateText( "Touch bottom to reset" )
settextsize( text_bottom, 42 )
SetTextAlignment( text_bottom, 1 )
// here we also use our virtual height (screen.y) to position the text 60 pixels up from the bottom of the screen
SetTextPosition( text_bottom, screen.x/2, screen.y -60 )
SetTextColor(text_bottom, 80, 80, 80, 255 )



// -------------------------------------------------------------------------------------------------------------
// Main loop starts here
// -------------------------------------------------------------------------------------------------------------

do

	// -------------------------------------------------------------------------------------------------------------
	// Updating various things
	// -------------------------------------------------------------------------------------------------------------

	// store the time it took to do the loop
	looptime = GetFrameTime()

	// change the alpha of the black sprite depending on the current framerate
	temp = looptime * 1200
	// if the alpha is too low we will get noticeable ghosting, so this line limits it to 20
	if temp < 20 then temp = 20
	SetSpriteColorAlpha( black, temp )

	// reading the device accelerometer
	tilt.x = GetRawAccelX()
	tilt.y = GetRawAccelY()

	// faking accelerometer with arrow keys to test on PC/Mac
	if GetRawKeyState(37) then tilt.x = -0.7
	if GetRawKeyState(38) then tilt.y = -0.7
	if GetRawKeyState(39) then tilt.x = 0.7
	if GetRawKeyState(40) then tilt.y = 0.7

	// apply gravity
	tilt.x = tilt.x * gravity / 100
	tilt.y = tilt.y * gravity / 100



	// -------------------------------------------------------------------------------------------------------------
	// Checking for a reset
	// -------------------------------------------------------------------------------------------------------------

	// reset squares with touch
	if GetRawTouchCurrentY( 1 ) > 900 and touchstate = 0
		resetsquares()
		touchstate = 2
	endif

	// reset squares with mouse
	if GetRawMouseLeftPressed() and GetRawMouseY() > 900
		resetsquares()
		touchstate = 2
	endif



	// -------------------------------------------------------------------------------------------------------------
	// Applying forces and updating sprites
	// -------------------------------------------------------------------------------------------------------------

	// going through all our sprites
	for i = 1 to square.length
		// applying the device tilting as force
		square[i].forcex = tilt.x
		square[i].forcey = tilt.y

		// if within reach of any finger then apply force
		if GetRawTouchCount(1) > 0 and touchstate < 2
			for t = 1 to GetRawTouchCount(1)
				if abs( square[i].x + square[i].size/2 - GetRawTouchCurrentX(t)  ) <= finger_force and abs( square[i].y + square[i].size/2 - GetRawTouchCurrentY(t)  ) <= finger_force
					square[i].forcex = ( square[i].x + square[i].size/2 - GetRawTouchCurrentX(t) ) / 5000 * finger_force
					square[i].forcey = ( square[i].y + square[i].size/2 - GetRawTouchCurrentY(t) ) / 5000 * finger_force
				endif
			next t
			touchstate = 1
		endif

		// if within reach of the mouse then apply force
		if GetRawMouseLeftState() and touchstate < 2
			if abs( square[i].x + square[i].size/2 - GetRawMouseX() ) <= finger_force and abs( square[i].y + square[i].size/2 - GetRawMouseY() ) <= finger_force
				square[i].forcex = ( square[i].x + square[i].size/2 - GetRawMouseX() ) / 5000 * finger_force
				square[i].forcey = ( square[i].y + square[i].size/2 - GetRawMouseY() ) / 5000 * finger_force
			endif
		endif

		// altering the speed of the square depending on it's force, using friction
		square[i].speedx = square[i].speedx + ( square[i].forcex - square[i].speedx ) * friction / 1000.0 * looptime*60
		square[i].speedy = square[i].speedy + ( square[i].forcey - square[i].speedy ) * friction / 1000.0 * looptime*60

		// altering the coordinates of the sprites depending on the speed
		// larger squares are "closer" to the screen and will move faster
		square[i].x = square[i].x + ( square[i].speedx * square[i].size * looptime * 50.0 )
		square[i].y = square[i].y + ( square[i].speedy * square[i].size * looptime * 50.0 )

		// making sure the squares stay within the screen
		// force and speed is then set to 0 for that axis
		if square[i].x <= 0
			square[i].x = 0
			square[i].forcex = 0
			square[i].speedx = 0
		endif
		if square[i].x >= screen.x - square[i].size
			square[i].x = screen.x - square[i].size
			square[i].forcex = 0
			square[i].speedx = 0
		endif
		if square[i].y <= 0
			square[i].y = 0
			square[i].forcey = 0
			square[i].speedy = 0
		endif
		if square[i].y >= screen.y - square[i].size
			square[i].y = screen.y - square[i].size
			square[i].forcey = 0
			square[i].speedy = 0
		endif

		SetSpritePosition( square[i].id, square[i].x, square[i].y )
	next i

	// reset touchstate when nothing touches the screen
	if GetRawTouchCount(1) = 0 and GetRawMouseLeftState() = 0
		touchstate = 0
	endif



	// -------------------------------------------------------------------------------------------------------------
	// Drawing to the screen
	// -------------------------------------------------------------------------------------------------------------

	// when using motion blur we don't use sync, instead we will render to our render image
	setrendertoimage( renderimage, 0 )
	update(0)
	render()

	// setting render back to the screen
	setrendertoscreen()

	// set the render sprite visible and to fill the screen
	setspritevisible( rendersprite, 1 )
	SetSpriteSize( rendersprite, screen.x, screen.y )

	// draw the render sprite and update the screen
	drawsprite( rendersprite )
	swap()

	// hide the render sprite
	setspritevisible( rendersprite, 0 )




	// exit out of the program when the alt key or Android back button is pressed.
	if GetRawKeyPressed(27) then end

loop



// -------------------------------------------------------------------------------------------------------------
// Function
// -------------------------------------------------------------------------------------------------------------

function resetsquares()
	// this function resets the squares

	for i = 1 to square.length
		// placing sprites around the screen, regardless of screen size
		square[i].x = random( screen.x*0.1, screen.x*0.9 )
		square[i].y = random( screen.y*0.1, screen.y*0.9 )
		SetSpritePosition( square[i].id, square[i].x, square[i].y )

		// resetting the values in the array
		square[i].forcey = 0
		square[i].speedy = 0
		square[i].forcex = 0
		square[i].speedx = 0
	next i
endfunction

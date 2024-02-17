// Psychedelic Painter - Particle Painting Tech Demo
// Developed Exclusively with AGK V2 (Alpha Build 8) 
// Programmed by Matthew J. Pilz [linkedPIXEL LLC] for AGK Showcase Challenge
// Credits: info@matthewpilz.com | http://linkedpixel.com/ | @mattpilz | xCept @ TGC Forums
// October 2014

// Video Demonstration: http://youtu.be/q1bwJNETNNc

// This demo showcases a creative use of AGK V2's Render To Image capabilities for a 2D entertainment application
// Mouse and full multitouch capabilities are integrated and the app runs on Android, iOS, Mac and PC without modifications
// Along the way, other V2 features are sprinkled about including V2 array commands, draw commands, particle effects, tweening
// Every significant line is well-commented with additional notes included when necessary

// Basic Configurable Attributes
#CONSTANT FULLSCREEN            0     // 0 = Windowed Mode | 1 = Fullscreen Mode
#CONSTANT WINDOWSIZE_WIDTH      1024  // Width of window when not in fullscreen view
#CONSTANT WINDOWSIZE_HEIGHT     768   // Height of window when not in fullscreen view
#CONSTANT TEXTURE_QUALITY       9    // Texture Draw Quality (7 = 128x128 | 8 = 256x256 | 9 = 512x512 | 10 = 1024x1024 | Etc.) 

// Call Core Sub-Routines
Gosub _Initialize_Variables         // Instantiates variables and user-defined types
Gosub _Initialize_App               // Sets screen resolution and prepares app for use
Gosub _Load_Media                   // Loads and generates all necessary media
Gosub _Main_Loop                    // The main loop of the app that runs until termination


`=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=.
` Initialize Variables
`=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-='
_Initialize_Variables:

	// USER-DEFINED TYPE: Interface Buttons (For Color Selection)
	Type obj_Button
		id As Integer          
		sizeX As Float
		sizeY As Float       
		positionX As Float
		positionY As Float
		active As Integer
	EndType

	// Create array of buttons (The user will have 8 color choices in this demo)
	Global button As obj_Button[8]
	
	// USER-DEFINED TYPE: Pointer Data for Easy Reference
	Type obj_Pointer
		startX As Float             // Pointer position X when first contact is made
		startY As Float             // Pointer position Y when first contact is made
		currentX As Float           // Pointer position X in current frame
		currentY As Float           // Pointer position Y in current frame
		lastX As Float              // Pointer position X in last frame
		lastY As Float              // Pointer position Y in last frame
		isOverMenu As Integer       // Flag to indicate whether active pointer is over menu item
		particleId As Integer
	EndType
	Global pointer As obj_Pointer

	// USER-DEFINED TYPE: Global Partical Properties
	Type obj_Particles
		minSize As Integer          // Minimum possible size of emitted particles
		maxSize As Integer          // Maximum possible size of emitted particles
		minScale As Integer          // Minimum possible scale when particles first start emitting (100 = original size)
		maxScale As Integer          // Maximum possible scale when particles conclude emitting (100 = original size)
		minLife As Integer          // Minimum life of emitted particles in (100 = 1.0 Second)
		maxLife As Integer          // Maximum life of emitted particles (100 = 1.0 Second)
		minRotation As Integer      // Minimum rotation of emitted particles
		maxRotation As Integer      // Maximum rotation of emitted particles
		minVelocity As Integer      // Minimum velocity of emitted particles
		maxVelocity As Integer      // Maximum velocity of emitted particles
		minFrequency As Integer     // Minimum frequency of emitted particles
		maxFrequency As Integer     // Maximum frequency of emitted particles
		minAmount As Integer        // Minimum amount of emitted particles
		maxAmount As Integer        // Maximum amount of emitted particles
		angleRange As Integer	    // Angle radius of emitted particles
	EndType
	Global particles As obj_Particles

	// Define Base Particle Properties - All particle emitters will be derived from these base values 
	particles.minSize = Map(64, 0, 1024, 0, GetDeviceWidth()) // Arbritrary value based on device resolution
	particles.maxSize = Map(96, 0, 1024, 0, GetDeviceWidth()) // Arbritrary value based on device resolution
	particles.minScale = 50
	particles.maxScale = 150
	particles.minLife = 125
	particles.maxLife = 500
	particles.minRotation = 0
	particles.maxRotation = 360
	particles.minVelocity = 5
	particles.maxVelocity = 25
	particles.minFrequency = 15
	particles.maxFrequency = 35
	particles.minAmount = 5
	particles.maxAmount = 40
	particles.angleRange = 45
	
	// Particle Image Texture Array
	Global particleImage As Integer[]
	
	// Particle Tracker (Useful for Deleting Emitters Etc.)
	Global particleTracker As Integer[]
	
	// Render Image and Canvas
	Global renderImage As Integer      // This will be the actual RENDER IMAGE that gets drawn to in real-time
	Global renderObject As Integer     // This will be the OBJECT that displays the RENDER IMAGE on the screen
	Global galleryImage As Integer     // This will be used to save the artwork as an external image 
	
	// USER-DEFINED TYPE: Color Value Storage RGBA
	Type colorRGBA
		r As Float
		g As Float
		b As Float
		a As Float
	EndType
	
	// USER-DEFINED TYPE: Color Ramp for Storing Available Color Data
	Type obj_ColorRamp
		totalRamps As Integer
		color As colorRGBA[5]
	EndType
	Global colorRamp As obj_ColorRamp[10]
	
	// Variable for Tracking Chosen Color
	Global chosenColor As Integer
	Global lastChosenColor As Integer
	
	// Variable for handling Save Artwork
	Global isSavingArtwork As Integer
	
	// Variable for storing platform type (ios, android, windows or mac)
	Global deviceType As String : deviceType = GetDeviceBaseName()

	// Basic Tweens for Toggling of Color Buttons
	Global tweenActiveAlpha = 255    // Opacity of button when active
	Global tweenInactiveAlpha = 192  // Opacity of button when inactive
	Global tweenToggleActive As Integer : tweenToggleActive = CreateTweenSprite(0.65)
	Global tweenToggleInactive As Integer : tweenToggleInactive = CreateTweenSprite(0.65)
	SetTweenSpriteAlpha(tweenToggleActive, tweenInactiveAlpha, tweenActiveAlpha, TweenSmooth2())
	SetTweenSpriteAlpha(tweenToggleInactive, tweenActiveAlpha, tweenInactiveAlpha, TweenSmooth2())

	// Tween animation for Saving Image notice
	Global tweenSaveIntro As Integer : tweenSaveIntro = CreateTweenSprite(1.0)
	Global tweenSaveOutro As Integer : tweenSaveOutro = CreateTweenSprite(1.0)

	// Set default color selection
	chosenColor = 7 : lastChosenColor = chosenColor
	
Return


`=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=.
` Initialize App
`=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-='
_Initialize_App:

	// Sets the title of the window (ignored when in fullscreen mode)
	SetWindowTitle("Psychedelic Painter - Particle Painting Demo by Matthew J. Pilz (linkedPIXEL LLC)")

	// Sets the window size and activates fullscreen mode if needed
	SetWindowSize(WINDOWSIZE_WIDTH, WINDOWSIZE_HEIGHT, FULLSCREEN)
	
	// Fix the framerate; Change 1 to 0 to conserve CPU and battery at the cost of some performance on mobile devices
	SetSyncRate(60, 1)

	// Set the actual working resolution to match the device's available width and height
	SetVirtualResolution(GetDeviceWidth(), GetDeviceHeight())

	// Set draw area scissoring -- Required to be 0, 0, 0, 0 for Render Image compatibility on various devices
	SetScissor(0, 0, 0, 0)
	
	// Make sure the timer is accurate
	ResetTimer()

	// Set default depth for Quad object
	SetGlobal3DDepth(100)

Return


`=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=.
` Load Media
`=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-='
_Load_Media:

	// Load several particle images to give subtle variance -- Uses V2's new dynamic array techinques
	particleImage.insert(LoadImage("particles_1.png"))
	particleImage.insert(LoadImage("particles_2.png"))
	particleImage.insert(LoadImage("particles_3.png"))
	particleImage.insert(LoadImage("particles_4.png"))
	particleImage.insert(LoadImage("particles_5.png"))
	
	// Load notice to display when saving
	Global saveNoticeSpr As integer : saveNoticeSpr = LoadSprite("savenotice.png")
	SetSpriteDepth(saveNoticeSpr, 0)
	SetSpriteColorAlpha(saveNoticeSpr, 0)
	
	// Resize notice sprite to fit screen resolution
	resizedSpr = map(256, 0, 768, 0, GetDeviceHeight())
	SetSpriteSize(saveNoticeSpr, resizedSpr, -1)

	// Set default position
	SetSpritePosition(saveNoticeSpr, (GetDeviceWidth() * 0.5) - (GetSpriteWidth(saveNoticeSpr) * 0.5), 0 - GetSpriteHeight(saveNoticeSpr))

	// Prepare notice tween animations
	SetTweenSpriteAlpha(tweenSaveIntro, 64, 255, TweenSmooth2())
	SetTweenSpriteY(tweenSaveIntro, 0 - GetSpriteHeight(saveNoticeSpr), (GetDeviceHeight() * 0.5) - (GetSpriteHeight(saveNoticeSpr) * 0.5), TweenBounce())
	SetTweenSpriteAlpha(tweenSaveOutro, 255, 0, TweenSmooth2())
	SetTweenSpriteY(tweenSaveOutro, (GetDeviceHeight() * 0.5) - (GetSpriteHeight(saveNoticeSpr) * 0.5), GetDeviceHeight(), TweenBounce())

	// Create a render canvas to draw onto 
	// NOTE: The Render Image should be an exponent of 2 or it will behave badly on many mobile devices
	//       This command also supports depth map and mipmapping for use in 3D object texturing, if needed
	renderImage = createRenderImage(2 ^ TEXTURE_QUALITY, 2 ^ TEXTURE_QUALITY,0, 0 )

	// There are several ways to render a fullscreen texture to the view, including:
	// 1.) Create a Sprite with same dimensions as screen resolution and set its texture to the render image
	// 2.) Create a Plane object, position it to match the camera viewport and set its texture to the render image
	// 3.) Create a Quad object, which is a convenience function in AGK specifically for applying fullscreen shaders
	// In my experience, the most accurate and fail-safe method across all devices and platforms is to use a Quad object

	// Create a Quad Object for displaying Render Image on screen
	renderObject = CreateObjectQuad()
	SetObjectImage(renderObject, renderImage, 0)
	SetObjectVisible(renderObject, 1)

	// Generate Color Selection Buttons Dynamically (No Media) and Basic File Menu
	generateButtons()
		
	// Toggle color button to indicate which color is selected
	ToggleColorButton(button[chosenColor].id)

	// Fill the render image with a solid color at first
	ClearCanvas()
	
Return


`=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=.
` Main Loop - Where All the Magic Happens
`=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-='
_Main_Loop:

	// Repeat command block until the user terminates the app
	Do
		
		// Update all objects on screen (including changes made since last frame)
		Update(0)
				
		// Hide interactive elements before handling canvas draw
		toggleHUD(0)

		// Switch all rendering to the render image
		setRenderToImage(renderImage, -1)

		// Handle All User Interactions and Particle Movements
		HandleUserInteraction()

		 // Update all objects for render image buffer 
		 Update(0)
		 
		 // Render output to buffer
		 Render()

		// Show interactive elements again
		toggleHUD(1)
	
		// Revert back to screen buffer for on-screen rendering
		SetRenderToScreen()

		// Update All Tween Animations
		UpdateAllTweens( GetFrameTime() )	

		// Draw Render Object with Modified Render Image Texture
		DrawObject(renderObject)
		
		// Save Artwork if Requested
		If isSavingArtwork = 1
			
			// Make sure notice is in place before starting save
			If GetTweenSpritePlaying(tweenSaveIntro, saveNoticeSpr) = 0
				
				// Process image and save to user's disk
				SaveArtwork()
			
				// Transition out notice
				isSavingArtwork = 0
				PlayTweenSprite(tweenSaveOutro, saveNoticeSpr, 0.25)
				
			Endif
			
		Endif
	 
		// Render the Output to Screen and Swap Buffer for Next Frame
		Render()
		Swap()
		
		// Reinitialize render image upon resume from background on mobile devices
		// Some devices will otherwise lose the buffer context and the app won't work right
		If GetResumed() = 1 And (deviceType = "android" Or deviceType = "ios")
			
			// Delete original render image and create a new one
			If GetImageExists(renderImage) Then DeleteImage(renderImage)
			renderImage = createRenderImage(2 ^ TEXTURE_QUALITY, 2 ^ TEXTURE_QUALITY,0, 0 )

			// Apply new render image to existing render object quad
			SetObjectImage(renderObject, renderImage, 0)
			SetObjectVisible(renderObject, 1)
				
			// Clear the canvas and prepare to draw again!
			ClearCanvas()

		Endif

	Loop
Return


`=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=.
` FUNCTION: Handle User Interaction
`=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-='
Function HandleUserInteraction()

	// Handle File Buttons
	If GetVirtualButtonReleased(1) Then ClearCanvas() // 'New' Button
	If GetVirtualButtonReleased(2)                    // 'Save' Button
		
		// Toggle save mode
		isSavingArtwork = 1
		
		// Bring save image notice into view as it can take a little while to save on some devices
		PlayTweenSprite(tweenSaveIntro, saveNoticeSpr, 0.0)

	Endif
	If GetVirtualButtonReleased(3) Then End           // 'Exit' Button
	
	// Allow ESC to exit app on Windows and Mac
	// NOTE: key code 27 will also be called when pressing 'back' on Android, but it is not advised to force-quit apps on Android or iOS
	If GetRawKeyReleased(27) And (deviceType = "windows" Or DeviceType = "mac")
		End // Terminate app
	Endif

	If(GetPointerPressed() = 1)
		pointer.currentX = GetPointerX()
		pointer.currentY = GetPointerY()
	Endif
	
	// Pointer is currently down, handle appropriate actions
	If(GetPointerState() = 1)

		// If the device is touch-capable, handle each available touch
		If GetMultiTouchExists() And GetRawTouchCount(1) > 0
			
			// Retrieve first touch ID (the parameter specifies to include ALL touches)
			touchId = GetRawFirstTouchEvent(1)

			// Cycle through all touches
			While touchId > 0
				
				// Store updated pointer data in array
				UpdatePointerData(touchId, 0)
				
				// Update particle system for given touch
				UpdateParticleSystem(touchId)
				
				// Get next touch -- If no more touches are available then 0 will be returned and the loop will break
				touchId = GetRawNextTouchEvent()
				
			EndWhile

		// Not multitouch, assume mouse input
		Else
			
			// Store updated pointer data in array
			UpdatePointerData(0, 1)
			
			// Update particle system for given touch
			UpdateParticleSystem(0)

		Endif
	
	// No pointer is down, do some cleanup
	Else
		
		// Reset particle ID, important for mouse input
		pointer.particleId = 0

		// Delete old emitters
		For i = 0 To particleTracker.length
			pId = particleTracker[i]
			if pId > 0 
				If GetParticlesExists(pId)
					If GetParticlesMaxReached(pId) // Make sure emitters are maxed out before deleting
						DeleteParticles(pId)				
						particleTracker.remove(particleTracker.find(pId)) // Remove particle emitter ID from particle tracking array
					Endif
				Endif
			Endif
		Next i
		
	Endif

EndFunction


`=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=.
` FUNCTION: Update Touch Data - Stores pointer data logically (mouse/touch)
`=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-='
Function UpdatePointerData(touchId As Integer, inputType As Integer)
	
	// Determine if a new particle emitter needs to be created (i.e., first touch)
	// GetRawTouchValue() will contain the particle emitter ID if one has already been created
	SpawnParticleSystem = 0
	If (inputType = 0 And GetRawTouchValue(touchId) = 0) Or (inputType = 1 And pointer.particleId = 0)
		SpawnParticleSystem = 1
	Endif
	
	// Store pointer coordinates into array
	Select inputType
	
		// Touch
		Case 0:
			// Store coordinates of starting touch position
			pointer.startX = GetRawTouchStartX(touchId)
			pointer.startY = GetRawTouchStartY(touchId)

			// Store coordinates of last touch position (i.e., from last frame)
			pointer.currentX = GetRawTouchCurrentX(touchId)
			pointer.currentY = GetRawTouchCurrentY(touchId)

			// Store coordinates of current touch position
			pointer.lastX = GetRawTouchLastX(touchId)
			pointer.lastY = GetRawTouchLastY(touchId)
		EndCase
		
		// Mouse
		Case Default:
			
			// Store coordinates of last mouse position
			pointer.lastX = pointer.currentX
			pointer.lastY = pointer.currentY

			// Store coordinates of current mouse position
			pointer.currentX = GetPointerX()
			pointer.currentY = GetPointerY()

		EndCase
		
	EndSelect
	
	// Check to see if user is over a color button
	// NOTE: I add +1 to the coordinates to ensure that if the user is at coord 0 the hit will still be detected
	For i = 0 to button.length - 1
		If GetSpriteHitTest(button[i].id, pointer.currentX + 1, pointer.currentY + 1) = 1
			chosenColor = i
			pointer.isOverMenu = 1
			If chosenColor <> lastChosenColor Then ToggleColorButton(button[i].id)
			lastChosenColor = chosenColor
			ExitFunction
		Endif
	Next i
	
	// If we made it this far in function, active pointer is NOT over menu item
	pointer.isOverMenu = 0

	// Generate new particle system if needed (i.e., on pointer press)
	If spawnParticleSystem = 1

		// Spawn new particle system at current coordinates
		pId = SpawnParticleSystem(pointer.currentX, pointer.currentY)
		
		// Assign particle ID of current touch ID for later reference
		// Note: AGK includes SetRawTouchValue() and GetRawTouchValue() for storing an arbitrary value per touch
		SetRawTouchValue(touchId, pId)
		
		// To ensure mouse compatibility, we'll also reference the particle emitter ID in a custom pointer property 
		pointer.particleId = pId
		
	Endif

EndFunction


`=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=.
` FUNCTION: Spawn Particle System
`=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-='
Function SpawnParticleSystem(x As Float, y As Float)

	// Calculate actual particle life (i.e., between 1.0 and 6.0 seconds)
	particleLife# = Random(particles.minLife, particles.maxLife) / 100.0
	particleMinSize = 0.0625 * GetDeviceHeight()
	particleMaxSize = 0.109375 * GetDeviceHeight()
	
	// Create new particle system at given coordinates 
	pId = CreateParticles(x, y)
	particleTracker.insert(pId)

	// Set various attributes of new particle system
	SetParticlesColorInterpolation(pId, 1)
	SetParticlesSize(pId,Random2(particles.minSize, particles.maxSize))
	SetParticlesLife(pId, particleLife#)
	SetParticlesVelocityRange(pId, particles.minVelocity, particles.maxVelocity)
	SetParticlesRotationRange(pId, particles.minRotation, particles.maxRotation)
	SetParticlesFrequency(pId, Random2(particles.minFrequency, particles.maxFrequency))
	SetParticlesMax(pId, Random2(particles.minAmount, particles.maxAmount))
	SetParticlesAngle(pId, particles.angleRange)
	SetParticlesImage(pId, particleImage[Random(0, particleImage.length)])
	
	// Add a subtle default velocity to the system
	SetParticlesDirection(pId, Random2(1, 2)-2, Random2(1, 2)-2)

	// Update colors of particle emitter
	updateColorValues(pId, particleLife#)

	// Randomize Particle Scale Over Time
	AddParticlesScaleKeyFrame(pId, 0, Random(particles.minScale / 3, particles.minScale) / 100.0)
	AddParticlesScaleKeyFrame(pId, particleLife#, Random(particles.maxScale / 3, particles.maxScale) / 100.0) 

EndFunction pId


`=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=.
` FUNCTION: Update Particle System - Called Based on User Interaction
`=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-='
Function UpdateparticleSystem(touchId As Integer)

	// First check to see if pointer is in same spot or over menu item, if so then do not update particle emitter
	If (pointer.currentX = pointer.lastX And pointer.currentY = pointer.lastY) Or pointer.isOverMenu = 1
		//ExitFunction
	Endif

	// Get particle emitter ID from stored touch value
	If touchId > 0
		pId = GetRawTouchValue(touchId)
	Else
		pId = pointer.particleId
	Endif
	
	// Update particle data, first checking that the emitter exists
	If GetParticlesExists(pId)
		
		// Reset particle count so taht new particles can spawn at current pointer position
		ResetParticleCount(pId)
		
		// Update particle direction based on pointer movement
		SetParticlesDirection(pId, pointer.currentX - pointer.lastX, pointer.currentY - pointer.lastY)

		// Update particle position to current pointer location
		SetParticlesPosition(pId, pointer.currentX, pointer.currentY)
	Else
		
	Endif

EndFunction


`=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=.
` FUNCTION: Spawn Particle System
`=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-='
Function ToggleColorButton(sprId As Integer)
	

	// Loop through all color buttons to visually toggle them accordingly
	For i = 0 To button.length - 1

		// First stop any concurrent tweens and reset sprites to base values as needed
		If GetTweenSpritePlaying(tweenToggleActive, button[i].id)
			StopTweenSprite(tweenToggleActive, button[i].id)
			SetSpriteColorAlpha(button[i].id, tweenActiveAlpha)
		Endif
		If GetTweenSpritePlaying(tweenToggleInactive, button[i].id)
			StopTweenSprite(tweenToggleInactive, button[i].id)
			SetSpriteColorAlpha(button[i].id, tweenInactiveAlpha)
		Endif
		
		// Current button matches chosen button, highlight it
		If button[i].id = sprId
			PlayTweenSprite(tweenToggleActive, sprId, 0.0)
			
		// Current button is not chosen button, fade it
		Else
			If(GetSpriteColorAlpha(button[i].id) > tweenInactiveAlpha) Then PlayTweenSprite(tweenToggleInactive, button[i].id, 0.0)
		Endif

	Next i
	
EndFunction

`=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=.
` FUNCTION: Toggle HUD - Used to hide and show elements when drawing to canvas
`=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-='
Function toggleHUD(state As Integer)

	// Rather than hiding and showing the interface elements, an alternative approach would be to position them
	// outside of the renderable view and then restoring their original position. I have found subtle framerate
	// improvements in more advanced projects using such an approach.
	
	// Toggle Color Buttons
	For i = 0 To button.length - 1
		SetSpriteVisible(button[i].id, state)
	Next i
	
	// Toggle File Buttons
	For i = 1 To 3
		SetVirtualButtonVisible(i, state)
	Next i
	
	// Save Image Notice
	SetSpriteVisible(saveNoticeSpr, state)
	
EndFunction


`=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=.
` FUNCTION: Clear Canvas
`=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-='
Function ClearCanvas()
	
	// Unbind particle emitters from all touches to ensure they aren't still controllable
	/*
	For i = 0 To pointer.length
		pointer[i].particleId = 0
	Next i
	*/
	
	// Destroy all existing particle emitters
	For i = 0 To particleTracker.length
		if particleTracker[i] > 0 
			If GetParticlesExists(particleTracker[i])
				SetParticlesActive(particleTracker[i], 0)
				DeleteParticles(particleTracker[i])
			Endif
		Endif
	Next i
	particleTracker.length = 0
	
	// Hide all HUD elements
	toggleHUD(0)
	
	// Create temporary sprite of solid color that matches device resolution
	tmpBgSpr = CreateSprite(0)
	SetSpriteSize(tmpBgSpr, GetDeviceWidth(), GetDeviceHeight())
	SetSpriteColor(tmpBgSpr, 0, 0, 0, 255)

	// Set renderer to use render image
	SetRenderToImage(renderImage, 0)

	// Remove artifacts as a precaution
	ClearScreen()

	// Draw solid sprite to render image to clear it
	DrawSprite(tmpBgSpr)

	// Render buffer
	Render()
	
	// Delete temporary sprite
	DeleteSprite(tmpBgSpr)
	
	// Show all HUD elements
	toggleHUD(0)

	// Return rendering to screen
	SetRenderToScreen()

EndFunction


`=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=.
` FUNCTION: Save Artwork to User's Documents
`=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-='
Function SaveArtwork()
	
	// Hide HUD before grabbing final final image
	toggleHUD(0)

	// Update screen contents
	Update(0)

	SetRenderToScreen()
	
	// Draw canvas and render to screen 
	DrawObject(renderObject)
	Render()
	
	// Grab image that matches screen resolution (rather than the render image resolution, which is different)
	galleryImage = GetImage(0, 0, GetDeviceWidth(), GetDeviceHeight())
	
	// Get current date and time for filename generation
	currentTime$ = GetCurrentTime()
	currentDate$ = GetCurrentDate()
	
	// For simplicity of naming we'll remove the dashes from the date format (YYYY-MM-DD)
	formattedDate$ = ""
	For i = 1 to 3
		formattedDate$ = formattedDate$ + GetStringToken(currentDate$, "-", i)
	Next i
	
	// The time format (HH:MM:SS) is not filename-friendly, so we'll remove the dashes
	formattedTime$ = ""
	For i = 1 to 3
		formattedTime$ = formattedTime$ + GetStringToken(currentTime$, ":", i)
	Next i
	
	// Create final filename for saving
	fName$ = "Artwork-" + formattedDate$ + "-" + formattedTime$ + ".png"
		
	// Windows or Mac
	If deviceType = "windows" Or deviceType = "mac" 

		// Change the write path to the user's Documents folder so they are easy to access
		SetRawWritePath(GetDocumentsPath())
		SetFolder("/PsychedelicPainter")
		
		// On PC/Mac we can use SaveImage() in conjunction with SetRawWritePath() to easily save the image
		SaveImage(galleryImage, fName$)
		
	// iOS or Android
	Else
		
		// The SaveImage() command is not an ideal solution for mobile as it gets saved to a protected directory
		// Instead, the PrintImage() command will save the artwork to a user-accessible directory
		// On Android, this is within the Pictures folder of the root storage
		// On iOS, this is in the user's Photo Library
		PrintImage(galleryImage, 100)

	Endif
	
	// Show HUD again
	toggleHUD(1)
	
EndFunction


`=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=.
` FUNCTION: Update Particle Emitter Colors
`=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-='
Function updateColorValues(pId As Integer, pLife As Float)
	
	// Delete any previously set particle color attributes
	ClearParticlesColors(pId)
	
	// Prepare the chosen color
	// NOTE: Solid colors will have randomized variance from dark-to-bright for greater depth
	//       Special psychedelic color contains more advanced color ramping
	 
	Select chosenColor
		
		// Red
		Case 0
			colorRamp[chosenColor].color[0].r = Random(32, 96) 
			colorRamp[chosenColor].color[0].g = 0
			colorRamp[chosenColor].color[0].b = 0
			colorRamp[chosenColor].color[0].a = 255

			colorRamp[chosenColor].color[1].r = Random(128, 255)
			colorRamp[chosenColor].color[1].g = 0
			colorRamp[chosenColor].color[1].b = 0
			colorRamp[chosenColor].color[1].a = 255
		EndCase
		
		// Orange
		Case 1
			colorRamp[chosenColor].color[0].r = Random(32, 96) 
			colorRamp[chosenColor].color[0].g = colorRamp[chosenColor].color[0].r / 2
			colorRamp[chosenColor].color[0].b = 0
			colorRamp[chosenColor].color[0].a = 255

			colorRamp[chosenColor].color[1].r = Random(128, 255)
			colorRamp[chosenColor].color[1].g = colorRamp[chosenColor].color[1].r / 2
			colorRamp[chosenColor].color[1].b = 0
			colorRamp[chosenColor].color[1].a = 255
		EndCase
		
		// Yellow
		Case 2
			colorRamp[chosenColor].color[0].r = Random(32, 96) 
			colorRamp[chosenColor].color[0].g = colorRamp[chosenColor].color[0].r
			colorRamp[chosenColor].color[0].b = 0
			colorRamp[chosenColor].color[0].a = 255

			colorRamp[chosenColor].color[1].r = Random(128, 255)
			colorRamp[chosenColor].color[1].g = colorRamp[chosenColor].color[1].r
			colorRamp[chosenColor].color[1].b = 0
			colorRamp[chosenColor].color[1].a = 255
		EndCase
		
		// Green
		Case 3
			colorRamp[chosenColor].color[0].r = 0
			colorRamp[chosenColor].color[0].g = Random(32, 96) 
			colorRamp[chosenColor].color[0].b = 0
			colorRamp[chosenColor].color[0].a = 255

			colorRamp[chosenColor].color[1].r = 0
			colorRamp[chosenColor].color[1].g = Random(128, 255)
			colorRamp[chosenColor].color[1].b = 0
			colorRamp[chosenColor].color[1].a = 255
		EndCase	
			
		// Aqua
		Case 4
			colorRamp[chosenColor].color[0].r = 0
			colorRamp[chosenColor].color[0].g = Random(32, 96) 
			colorRamp[chosenColor].color[0].b = colorRamp[chosenColor].color[0].g
			colorRamp[chosenColor].color[0].a = 255

			colorRamp[chosenColor].color[1].r = 0
			colorRamp[chosenColor].color[1].g = Random(128, 255)
			colorRamp[chosenColor].color[1].b = colorRamp[chosenColor].color[1].g
			colorRamp[chosenColor].color[1].a = 255
		EndCase	
		
		// Blue
		Case 5
			colorRamp[chosenColor].color[0].r = 0
			colorRamp[chosenColor].color[0].g = 0 
			colorRamp[chosenColor].color[0].b = Random(32, 96)
			colorRamp[chosenColor].color[0].a = 255

			colorRamp[chosenColor].color[1].r = 0
			colorRamp[chosenColor].color[1].g = 0
			colorRamp[chosenColor].color[1].b = Random(128, 255)
			colorRamp[chosenColor].color[1].a = 255
		EndCase	
				
		// Purple
		Case 6
			colorRamp[chosenColor].color[0].r = Random(32, 96)
			colorRamp[chosenColor].color[0].g = 0 
			colorRamp[chosenColor].color[0].b = colorRamp[chosenColor].color[0].r
			colorRamp[chosenColor].color[0].a = 255

			colorRamp[chosenColor].color[1].r = Random(128, 255)
			colorRamp[chosenColor].color[1].g = 0
			colorRamp[chosenColor].color[1].b = colorRamp[chosenColor].color[0].r
			colorRamp[chosenColor].color[1].a = 255
		EndCase			
		
		// Psychedelic	
		Case 7
			
			// There will be five total ramps of varying colors
			colorRamp[chosenColor].totalRamps = 5

			// Temporary array to determine order of particle colors 
			Dim colorOrder[5]
			
			// Randomly generate color sequence
			Select Random(0, 5)
				Case 0 : colorOrder[0] = 0 : colorOrder[1] = 1 : colorOrder[2] = 2 : colorOrder[3] = 3 : colorOrder[4] = 4 :  colorOrder[4] = 5 : EndCase
				Case 1 : colorOrder[0] = 1 : colorOrder[1] = 2 : colorOrder[2] = 3 : colorOrder[3] = 4 : colorOrder[4] = 5 :  colorOrder[4] = 0 : EndCase
				Case 2 : colorOrder[0] = 2 : colorOrder[1] = 3 : colorOrder[2] = 4 : colorOrder[3] = 5 : colorOrder[4] = 0 :  colorOrder[4] = 1 : EndCase
				Case 3 : colorOrder[0] = 3 : colorOrder[1] = 4 : colorOrder[2] = 5 : colorOrder[3] = 0 : colorOrder[4] = 1 :  colorOrder[4] = 2 : EndCase
				Case 4 : colorOrder[0] = 4 : colorOrder[1] = 5 : colorOrder[2] = 0 : colorOrder[3] = 1 : colorOrder[4] = 2 :  colorOrder[4] = 3 : EndCase
				Case 5 : colorOrder[0] = 5 : colorOrder[1] = 0 : colorOrder[2] = 1 : colorOrder[3] = 2 : colorOrder[4] = 3 :  colorOrder[4] = 4 : EndCase
			EndSelect
			
			// Determine particle interval per color
			pInterval# = pLife / colorRamp[chosenColor].totalRamps
			
			// Add particle color keyframes
			AddParticlesColorKeyFrame(pId, pInterval# * colorOrder[0],255,0,0,255 )       // RED
			AddParticlesColorKeyFrame(pId, pInterval# * colorOrder[1],255,255,0,255 )     // YELLOW
			AddParticlesColorKeyFrame(pId, pInterval# * colorOrder[2],0,255,0,255 )       // GREEN
			AddParticlesColorKeyFrame(pId, pInterval# * colorOrder[3],0,255,255,255 )     // AQUA
			AddParticlesColorKeyFrame(pId, pInterval# * colorOrder[4],0,0,255,255 )       // BLUE
			AddParticlesColorKeyFrame(pId, pInterval# * colorOrder[5],255,0,255,255 )     // PURPLE	

			// Destroy temporary array
			Undim colorOrder[]	
			
			// Break out of function
			ExitFunction		

		EndCase
		
	EndSelect

	// If still in function, then a standard color was chosen and can be applied now; Each ramps up from dark to light
	colorRamp[chosenColor].totalRamps = 1
	
	// Determine particle interval per color
	pInterval# = pLife / colorRamp[chosenColor].totalRamps

	// Loop through total ramps and apply particle color keyframes
	For i = 0 To colorRamp[chosenColor].totalRamps
		AddParticlesColorKeyFrame(pId, pInterval# * i, colorRamp[chosenColor].color[i].r,colorRamp[chosenColor].color[i].g,colorRamp[chosenColor].color[i].b,colorRamp[chosenColor].color[i].a )
	Next i
	
EndFunction


`=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=.
` FUNCTION: Generate Interactive Buttons
`=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-='
Function generateButtons()
	
	// Calculate the width and height of each button based on screen resolution and total number of buttons
	buttonWidth = GetDeviceWidth() / button.length
	buttonHeight =  map(54, 0, 768, 0, GetDeviceHeight())

	// Prepare back buffer for drawing using AGK V2's integrated drawing commands
	Swap()
	
	// Store color values for top and bottom shades of button (grayscale gradient to be modified later)
	borderSize = 4
	colorTopL = MakeColor(255, 255, 255)
	colorTopR = colorTopL
	colorBottomL = MakeColor(196, 196, 196)
	colorBottomR = colorBottomL
	colorBorder = MakeColor(255, 255, 255)

	// Draw a rectangle 
	DrawBox(0, 0, buttonWidth, buttonHeight, colorBorder, colorBorder, colorBorder, colorBorder, 1) // Border
	DrawBox(borderSize, borderSize, buttonWidth - borderSize, buttonHeight - borderSize, colorTopL, colorTopR, colorBottomL, colorBottomR, 1) // Button Fill

	// Actually draw the contents to the frame buffer
	Render() 

	// Use GetImage() to store drawn image as a single texture for all button sprites
	btnImageStandard = GetImage(0, 0, buttonWidth, buttonHeight)

	// Special button image for Psychedelic effect
	colorTopL = MakeColor(255, 0, 0)    // Red
	colorTopR = MakeColor(0, 255, 0)    // Green
	colorBottomL = MakeColor(0, 0, 255) // Blue
	colorBottomR = MakeColor(255, 255, 0) // Yellow
	DrawBox(0, 0, buttonWidth, buttonHeight, colorTopR, colorTopL, colorBottomR, colorBottomL, 1) // Border
	DrawBox(borderSize, borderSize, buttonWidth - borderSize, buttonHeight - borderSize, colorTopL, colorTopR, colorBottomL, colorBottomR, 1) // Button Fill
	btnImagePlasma = GetImage(0, 0, buttonWidth, buttonHeight)

	// Restore original frame buffer
	ClearScreen()
	Swap()

	// Generate individual buttons
	btnCnt = 0
	For i = 0 To button.length - 1
		
		// Create a new button sprite using the previously generated texture image
		Inc btnCnt, 1
		
		// Apply standard button texture or plasma texture depending on button number
		If i < button.length - 1
			CreateSprite(btnCnt, btnImageStandard)
		Else
			CreateSprite(btnCnt, btnImagePlasma)
		Endif
		
		// Keep track of sprite ID and which row and column the current button is at for future use
		button[i].id = btnCnt
		button[i].active = 0
		
		// Update button's position on screen and store for future use
		button[i].positionX = i * buttonWidth
		button[i].positionY = 0
		button[i].sizeX = buttonWidth
		button[i].sizeY = buttonHeight
		SetSpritePosition(btnCnt, button[i].positionX, button[i].positionY)
		SetSpriteColorAlpha(btnCnt, 255)
		SetSpriteDepth(btnCnt, 0) // Make sure buttons are above all else

	Next i
	
	// Update grayscale button colors to reflect available options
	SetSpriteColor(button[0].id, 255, 0, 0, 255)     // Red
	SetSpriteColor(button[1].id, 255, 128, 0, 255)   // Orange
	SetSpriteColor(button[2].id, 255, 255, 0, 255)   // Yellow
	SetSpriteColor(button[3].id, 0, 255, 0, 255)     // Green
	SetSpriteColor(button[4].id, 0, 255, 255, 255)   // Aqua
	SetSpriteColor(button[5].id, 0, 0, 255, 255)     // Blue
	SetSpriteColor(button[6].id, 255, 0, 255, 255)   // Purple
	
	fileBtnRadius = map(64, 0, 1024, 0, GetDeviceWidth())

	// Simple New Button
	AddVirtualButton(1, 0, 0, fileBtnRadius)
	SetVirtualButtonText(1, "New")
	SetVirtualButtonColor(1, 145, 145, 225)
	
	// Simple Save Button
	AddVirtualButton(2, 0, 0, fileBtnRadius)
	SetVirtualButtonText(2, "Save")
	SetVirtualButtonColor(2, 145, 225, 145)
	
	// Simple Exit Button
	AddVirtualButton(3, 0, 0, fileBtnRadius)
	SetVirtualButtonText(3, "Exit")
	SetVirtualButtonColor(3, 225, 145, 145)

	// Position file menu buttons on screen
	If (deviceType = "windows" Or deviceType = "mac")
		SetVirtualButtonPosition(1, GetDeviceWidth() * 0.5 - fileBtnRadius - 2, GetDeviceHeight() - fileBtnRadius / 2 - 2)
		SetVirtualButtonPosition(2, GetDeviceWidth() * 0.5, GetDeviceHeight() - fileBtnRadius / 2 - 2)
		SetVirtualButtonPosition(3, GetDeviceWidth() * 0.5 + fileBtnRadius + 2, GetDeviceHeight() - fileBtnRadius / 2 - 2)

	// Delete 'exit' button when on Android or iOS to conform with app best-practices
	Else
		DeleteVirtualButton(3)
		SetVirtualButtonPosition(1, GetDeviceWidth() * 0.5 - fileBtnRadius / 2 - 2, GetDeviceHeight() - fileBtnRadius / 2 - 2)
		SetVirtualButtonPosition(2, GetDeviceWidth() * 0.5 + fileBtnRadius / 2 + 2, GetDeviceHeight() - fileBtnRadius / 2 - 2)		
	Endif
	
EndFunction

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=.
// UTILITY FUNCTION: Map New Value from Old Value
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-='
Function map(value As Float, istart As Float, istop As Float, ostart As Float, ostop As Float)
	result = ostart + (ostop - ostart) * ((value - istart) / (istop - istart))
EndFunction result

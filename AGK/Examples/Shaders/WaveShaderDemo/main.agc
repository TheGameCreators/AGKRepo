// Wave shader demo
// Used by kind permission of the shader author Alexander Alekseev

/*
 * "Seascape" by Alexander Alekseev aka TDM - 2014
 * License Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported License.
 * Contact: tdmaav@gmail.com
 */
 
UseNewDefaultFonts( 1 )
SetPrintSize( 40 )
SetPrintColor( 255,255,255 )

setvirtualresolution(1024,768)
SetClearColor( 101,120,154 )
SetScissor(0,0,0,0)
SetGenerateMipmaps(1)
SetCameraRange(1,0.1,100)
SetWindowAllowResize(1)

LoadShader( 1, "Quad.vs", "Water.ps" )

CreateObjectQuad( 1 )
SetObjectShader( 1, 1 )

SetShaderConstantByName( 1, "iResolution", 1024,768,0,0 )
SetShaderConstantByName( 1, "xy", 0,0,0,0 )
SetShaderConstantByName( 1, "y", 0,0,0,0 )
SetShaderConstantByName( 1, "iMouse", 0,0,0,0 )

do
	SetShaderConstantByName( 1, "iTime", Timer(),0,0,0 )      
	DrawObject(1)
	Sync()
loop

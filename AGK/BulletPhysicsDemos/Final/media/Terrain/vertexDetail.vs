// The attribute keyword tells the compiler that this is an 
// input variable that comes with each vertex data structure.
// Types vec2 and vec4 declare that the data is a vector of 
// floats; vec can have 2-4 components.

attribute vec3 position;
attribute vec3 normal;
attribute vec2 uv;

// When you need some external variables passed to the 
// shader from your source code, you need to declare them 
// as uniform. Type mat4 declares that the data is a 
// 4×4 matrix of floats. 

uniform mat4 agk_WorldViewProj; 

// This is object texture 0
uniform vec4 uvBounds0;

// This is object texture 1
uniform vec4 uvBounds1;

// Scale Input by AGK Tier1 : ex: SetShaderConstantByName( shaderindex, "ScaleDetailMap",0.1 ,0.1,0, 0 )
uniform vec2 ScaleDetailMap;

// The vertex shader needs to send some data to the 
// pixel shader. To notate variables that you pass from 
// a vertex shader to a pixel shader, you use the 
// varying keyword.

varying vec2 uv0Varying;
varying vec2 uv1Varying;
 
void main()
{
  // Vertex shaders need to fill the built-in variable 
  // gl_Position with the transformed vertex position.
  gl_Position = agk_WorldViewProj * vec4(position,1);
  
  // This passes the input coordinates of the color texture to the pixel shader 
  // unchanged by assigning them to the varying variable.  
  uv0Varying = uv * uvBounds0.xy + uvBounds0.zw;

  
  // This passes the input coordinates of the Detail Map to the pixel shader 
  // changed by assigning them with the inversed Scale Value to the varying variable.  
  uv1Varying = uv * (uvBounds1.xy+(1.0/ScaleDetailMap)) + uvBounds1.zw;  

}
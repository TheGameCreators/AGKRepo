// combined model/view/projection matrix
uniform mat4 agk_WorldViewProj;

// Per-vertex position information 
attribute vec3 position;
attribute vec3 normal;
attribute vec2 uv;

uniform float agk_time;
uniform vec4 uvBounds0;

// we set our object image wrap uv's to a vaue of 1
varying vec2 uv0Varying;
varying vec2 uv1Varying;

void main()
{
   gl_Position = agk_WorldViewProj * vec4(position,1);
   
   // we alter uv coordinates as time passes
   uv0Varying = uv - vec2(0.05 + agk_time * 0.0015, 0.0);
   uv1Varying = uv + vec2(0.0, agk_time * 0.001);
}
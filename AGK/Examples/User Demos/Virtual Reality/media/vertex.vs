attribute vec3 position;
attribute vec3 normal;
attribute vec2 uv;
attribute vec2 uv1;

varying vec2 uv0Varying;
varying vec2 uv1Varying;

uniform vec4 uvBounds0;
uniform mat4 agk_WorldViewProj;
	
void main()
{ 
	gl_Position = agk_WorldViewProj * vec4(position,1);
	uv0Varying = uv * uvBounds0.xy + uvBounds0.zw;
	uv1Varying = uv1 * uvBounds0.xy + uvBounds0.zw;
}

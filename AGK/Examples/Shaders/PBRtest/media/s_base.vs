attribute vec3 position;
attribute mediump vec3 normal;
attribute mediump vec3 binormal;
attribute mediump vec3 tangent;
attribute mediump vec2 uv;

varying vec2 uv0Varying;
varying vec2 uv1Varying;
varying vec3 posVarying;
varying vec3 normalVarying;
varying vec3 binormalVarying;
varying vec3 tangentVarying;

uniform mediump vec4 uvBounds0;
uniform mat4 agk_World;
uniform mat4 agk_ViewProj;
uniform mat3 agk_WorldNormal;

void main(){	
	uv0Varying = uv * uvBounds0.xy + uvBounds0.zw;
	vec4 pos = agk_World * vec4(position,1);
	normalVarying = agk_WorldNormal * normal;
	binormalVarying = agk_WorldNormal *binormal;
	tangentVarying = agk_WorldNormal *tangent;
	posVarying = pos.xyz;
	gl_Position = agk_ViewProj * pos;
}


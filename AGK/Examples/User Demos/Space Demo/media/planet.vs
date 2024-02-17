// combined model/view/projection matrix
uniform mat4 agk_WorldViewProj;
uniform mat4 agk_World;
uniform mat4 agk_ViewProj;
uniform mat3 agk_WorldNormal;

// Per-vertex position information 
attribute vec3 position;
attribute vec3 normal;
attribute vec2 uv;

varying vec2 uvVarying;
varying vec3 normalVarying;
varying vec3 posVarying;

void main()
{
   vec4 pos      = agk_World * vec4(position,1);
   gl_Position   = agk_ViewProj * pos;
   vec3 norm     = agk_WorldNormal * normal;
   posVarying    = pos.xyz;
   normalVarying = norm;
   uvVarying     = uv;  
}
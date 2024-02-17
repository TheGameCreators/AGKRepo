// combined model/view/projection matrix
uniform mat4 agk_WorldViewProj;

// Per-vertex position information 
attribute vec3 position;
attribute vec2 uv;

varying vec2 uvVarying;

void main()
{
   gl_Position = agk_WorldViewProj * vec4(position,1);
   uvVarying = uv;
}
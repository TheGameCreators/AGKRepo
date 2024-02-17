attribute highp vec3 position;
attribute mediump vec2 uv;
 
varying mediump vec2 uvVarying;
 
uniform highp mat4 agk_WorldViewProj;
uniform mediump vec4 uvBounds0;
 
void main()
{ 
    uvVarying = uv * uvBounds0.xy + uvBounds0.zw;
    gl_Position = agk_WorldViewProj * vec4(position,1.0);
}
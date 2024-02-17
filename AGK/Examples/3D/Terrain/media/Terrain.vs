attribute highp vec3 position;
attribute mediump vec3 normal;
attribute mediump vec2 uv;

varying highp vec3 posVarying;
varying mediump vec3 normalVarying;
varying mediump vec2 uvVarying;
varying mediump vec3 lightVarying;
varying mediump vec3 colorVarying;

// this must appear exactly as it is for it to work, spaces and all.
// it will be filled in by AGK
mediump vec3 GetVSLighting( mediump vec3 normal, highp vec3 pos );

uniform highp mat3 agk_WorldNormal;
uniform highp mat4 agk_World;
uniform highp mat4 agk_ViewProj;

uniform mediump vec4 uvBounds0;

// if you want to change the following values when the app is running, change 
// them to uniforms, remove the default values, and use SetShaderConstantByName 
// after loading the shader

// all colors multiplied by 1.5 to boost them a bit because the detail texture is quite dark
mediump vec3 grassColor = vec3(0.89, 1.08, 0.77);
mediump vec3 snowColor = vec3(1.5, 1.5, 1.5);
mediump vec3 rockColor = vec3(0.75, 0.75, 0.75);
mediump float snowHeightStart = 25.0; // height that grass begins to turn to snow
mediump float snowHeightEnd = 35.0; // height that it becomes completely snow
mediump float rockStart = 0.8; // normals with Y greater than this will be 100% grass
mediump float rockEnd = 0.4; // normals with Y less than this will be 100% rock

// to improve performance precalculate 
//   1.0 / (snowHeightEnd-snowHeightStart)
// and
//   1.0 / (rockStart-rockEnd) 
// you can also turn these into uniforms and calculate them in your app

mediump float snowTransition = 0.1; // 1.0 / (35.0 - 25.0)
mediump float rockTransition = 2.5; // 1.0 / (0.8 - 0.4)

void main()
{ 
    uvVarying = uv * uvBounds0.xy + uvBounds0.zw;
    highp vec4 pos = agk_World * vec4(position,1);
    gl_Position = agk_ViewProj * pos;
    mediump vec3 norm = normalize(agk_WorldNormal * normal);
    posVarying = pos.xyz;
    normalVarying = norm;
    lightVarying = GetVSLighting( norm, posVarying );
    
    // blend grass and snow colors based on world height
    highp float t = clamp( (pos.y-snowHeightStart)*snowTransition, 0.0, 1.0 );
	mediump vec3 color = mix( grassColor, snowColor, t );
	
	// blend from that color to rock color based on the angle of the normal
	t = clamp((norm.y-rockEnd)*rockTransition, 0.0, 1.0); 
	colorVarying = mix( rockColor, color, t );
}

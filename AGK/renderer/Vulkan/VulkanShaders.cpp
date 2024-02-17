#if !defined(DISABLE_VULKAN) && (!defined(__APPLE__) || defined(__LP64__))

#include "agk.h"
#include "AGKVulkan.h"
#include "GLSLParser.h"
#include "GLSLtoSPIRV/glslang_main/glslang_main.h"
#include "spirv_reflect.h"

VulkanShader* VulkanShader::g_pAllShaderObjects = 0;

void VulkanShader::RemoveShaderObject() 
{
	if ( !m_pNextShader && !m_pPrevShader && g_pAllShaderObjects != this ) return;

	if ( m_pPrevShader ) m_pPrevShader->m_pNextShader = m_pNextShader;
	else g_pAllShaderObjects = m_pNextShader;
	if ( m_pNextShader ) m_pNextShader->m_pPrevShader = m_pPrevShader;
	m_pNextShader = 0;
	m_pPrevShader = 0;
}

void VulkanShader::AddShaderObject()
{
	if ( m_pNextShader || m_pPrevShader || g_pAllShaderObjects == this ) return;

	if ( g_pAllShaderObjects ) g_pAllShaderObjects->m_pPrevShader = this;
	m_pPrevShader = 0;
	m_pNextShader = g_pAllShaderObjects;
	g_pAllShaderObjects = this;
}

const char* VulkanRenderer::GetShaderDefaultFullScreenVS()
{
	return "\
	attribute vec3 position;\n\
	varying vec2 uvVarying;\n\
	uniform float agk_invert;\n\
	\
	void main() {\n\
		gl_Position = vec4(position.xy*vec2(1,agk_invert),0.5,1.0);\n\
		uvVarying = (position.xy*vec2(0.5,-0.5) + 0.5);\n\
	}";
}

const char* VulkanRenderer::GetShaderDefaultSpriteVS()
{
	return "\
	attribute vec3 position;\n\
	attribute vec4 color;\n\
	attribute vec2 uv;\n\
	varying vec2 uvVarying;\n\
	varying vec4 colorVarying;\n\
	uniform mat4 agk_Ortho;\n\
	void main() { \n\
		gl_Position = agk_Ortho * vec4(position,1.0);\n\
		uvVarying = uv;\n\
		colorVarying = color;\n\
	}";
}

AGKShader* VulkanRenderer::LoadDefaultShaderColor()
{
	AGKShader *pShader = new AGKShader();

	pShader->m_sVSFilename.SetStr( "Default Color Shader" );
	pShader->m_sPSFilename.SetStr( "Default Color Shader" );

	pShader->m_sVSSource.SetStr( "\
	#version 110\n\
	attribute vec3 position;\n\
	attribute vec4 color;\n\
	varying vec4 colorVarying;\n\
	uniform mat4 agk_Ortho;\n\
	void main() {\n\
		gl_Position = agk_Ortho * vec4(position,1.0);\n\
		colorVarying = color;\n\
	}" );

	pShader->m_sPSSource.SetStr( "\
	#version 110\n\
	varying vec4 colorVarying;\n\
	void main() { gl_FragColor = colorVarying; }" );

	pShader->m_iFlags |= AGK_SHADER_IS_DEFAULT;

	InternalCompileShader( pShader, AGK_VK_SHADER_TYPE_NO_LAYOUTS );
	InternalEnumerateShaderResources( pShader );

	return pShader;
}

AGKShader* VulkanRenderer::LoadDefaultShaderTexColor()
{
	AGKShader *pShader = new AGKShader();

	pShader->m_sVSFilename.SetStr( "Default TexColor Shader" );
	pShader->m_sPSFilename.SetStr( "Default TexColor Shader" );

	pShader->m_sVSSource.SetStr( "\
	#version 110\n\
	attribute vec3 position;\n\
	attribute vec4 color;\n\
	attribute vec2 uv;\n\
	\
	varying vec2 uvVarying;\n\
	varying vec4 colorVarying;\n\
	\
	uniform mat4 agk_Ortho;\n\
	\
	void main() {\n\
		gl_Position = agk_Ortho * vec4(position,1.0);\n\
		uvVarying = uv;\n\
		colorVarying = color;\n\
	}" );

	pShader->m_sPSSource.SetStr( "\
	#version 110\n\
	uniform sampler2D texture0;\n\
	varying vec2 uvVarying;\n\
	varying vec4 colorVarying;\n\
	\
	void main() { \n\
		gl_FragColor = texture2D(texture0, uvVarying) * colorVarying;\n\
	}" );

	pShader->m_iFlags |= AGK_SHADER_IS_DEFAULT;

	InternalCompileShader( pShader, AGK_VK_SHADER_TYPE_NO_LAYOUTS );
	InternalEnumerateShaderResources( pShader );

	return pShader;
}

AGKShader* VulkanRenderer::LoadDefaultShaderFont()
{
	AGKShader *pShader = new AGKShader();

	pShader->m_sVSFilename.SetStr( "Default Font Shader" );
	pShader->m_sPSFilename.SetStr( "Default Font Shader" );

	pShader->m_sVSSource.SetStr( "\
	#version 110\n\
	attribute vec3 position;\n\
	attribute vec4 color;\n\
	attribute vec2 uv;\n\
	\
	varying vec2 uvVarying;\n\
	varying vec4 colorVarying;\n\
	\
	uniform mat4 agk_Ortho;\n\
	\
	void main() { \n\
		gl_Position = agk_Ortho * vec4(position,1.0);\n\
		uvVarying = uv;\n\
		colorVarying = color;\n\
	}" );
	
	pShader->m_sPSSource.SetStr( "\
	#version 110\n\
	uniform sampler2D texture0;\n\
	varying vec2 uvVarying;\n\
	varying vec4 colorVarying;\n\
	void main() {\n\
		gl_FragColor = colorVarying;\n\
		gl_FragColor.a = gl_FragColor.a * texture2D(texture0, uvVarying).r;\n\
	}" );

	pShader->m_iFlags |= AGK_SHADER_IS_DEFAULT;

	InternalCompileShader( pShader, AGK_VK_SHADER_TYPE_NO_LAYOUTS );
	InternalEnumerateShaderResources( pShader );

	return pShader;
}

AGKShader* VulkanRenderer::LoadDefaultShaderShadow()
{
	AGKShader *pShader = new AGKShader();

	pShader->m_sVSFilename.SetStr( "Shadow Shader" );
	pShader->m_sPSFilename.SetStr( "Shadow Shader" );

	pShader->m_sVSSource.SetStr( "\
	#version 110\n\
	attribute vec3 position;\n\
	\
	uniform mat4 agk_ShadowWorldProj;\n\
	\
	void main() { \n\
		gl_Position = agk_ShadowWorldProj * vec4(position,1.0);\n\
	}" );
	
	pShader->m_sPSSource.SetStr( "\
	#version 110\n\
	void main() {}" );

	pShader->m_iFlags |= AGK_SHADER_IS_DEFAULT | AGK_SHADER_IS_CUSTOM;

	InternalCompileShader( pShader, AGK_VK_SHADER_TYPE_NO_LAYOUTS );
	InternalEnumerateShaderResources( pShader );

	return pShader;
}

AGKShader* VulkanRenderer::LoadDefaultShaderShadowAlpha()
{
	AGKShader *pShader = new AGKShader();

	pShader->m_sVSFilename.SetStr( "Shadow Alpha Mask Shader" );
	pShader->m_sPSFilename.SetStr( "Shadow Alpha Mask Shader" );

	pShader->m_sVSSource.SetStr( "\
	#version 110\n\
	attribute vec3 position;\n\
	attribute vec2 uv;\n\
	\
	varying vec2 uvVarying;\n\
	\
	uniform vec4 uvBounds0;\n\
	uniform vec4 textureBounds0;\n\
	\
	uniform mat4 agk_ShadowWorldProj;\n\
	\
	void main() { \n\
		gl_Position = agk_ShadowWorldProj * vec4(position,1.0);\n\
		uvVarying = uv * uvBounds0.xy + uvBounds0.zw;\n\
		uvVarying = uvVarying * textureBounds0.xy + textureBounds0.zw;\n\
	}" );
	
	pShader->m_sPSSource.SetStr( "\
	#version 110\n\
	uniform sampler2D texture0;\n\
	varying vec2 uvVarying;\n\
	void main() {\n\
	    float alpha = texture2D(texture0, uvVarying).a;\n\
	    if ( alpha < 0.5 ) discard;\n\
	}" );

	pShader->m_iFlags |= AGK_SHADER_IS_DEFAULT | AGK_SHADER_IS_CUSTOM;

	InternalCompileShader( pShader, AGK_VK_SHADER_TYPE_NO_LAYOUTS );
	InternalEnumerateShaderResources( pShader );

	return pShader;
}

AGKShader* VulkanRenderer::LoadDefaultShaderShadowBone( uint32_t numBones )
{
	AGKShader *pShader = new AGKShader();

	pShader->m_sVSFilename.SetStr( "Shadow Bone Shader" );
	pShader->m_sPSFilename.SetStr( "Shadow Bone Shader" );

	char szBoneCount[ 12 ];
	sprintf( szBoneCount, "%d", numBones );

	pShader->m_sVSSource.SetStr( "\
	#version 110\n\
	attribute vec3 position;\n\
	attribute vec4 boneweights;\n\
	attribute vec4 boneindices;\n\
	\
	uniform mat4 agk_ShadowProj;\n\
	uniform vec4 agk_bonequats1[" ); pShader->m_sVSSource.AppendAscii(szBoneCount); pShader->m_sVSSource.AppendAscii("];\n\
	uniform vec4 agk_bonequats2[" ); pShader->m_sVSSource.AppendAscii(szBoneCount); pShader->m_sVSSource.AppendAscii("];\n\
	\
	vec3 transformDQ( vec3 p, vec4 q1, vec4 q2 ) {\n\
	    p += 2.0 * cross( q1.xyz, cross(q1.xyz, p) + q1.w*p );\n\
	    p += 2.0 * (q1.w*q2.xyz - q2.w*q1.xyz + cross(q1.xyz,q2.xyz));\n\
	    return p;\n\
	}\n\
	\
	void main() { \n\
		vec4 q1 = agk_bonequats1[ int(boneindices.x) ] * boneweights.x;\n\
		q1 += agk_bonequats1[ int(boneindices.y) ] * boneweights.y;\n\
		q1 += agk_bonequats1[ int(boneindices.z) ] * boneweights.z;\n\
		q1 += agk_bonequats1[ int(boneindices.w) ] * boneweights.w;\n\
		\
		vec4 q2 = agk_bonequats2[ int(boneindices.x) ] * boneweights.x;\n\
		q2 += agk_bonequats2[ int(boneindices.y) ] * boneweights.y;\n\
		q2 += agk_bonequats2[ int(boneindices.z) ] * boneweights.z;\n\
		q2 += agk_bonequats2[ int(boneindices.w) ] * boneweights.w;\n\
		\
		float len = 1.0/length(q1);\n\
		q1 *= len;\n\
		q2 = (q2 - q1*dot(q1,q2)) * len;\n\
		\
		gl_Position = agk_ShadowProj * vec4( transformDQ(position,q1,q2), 1.0 );\n\
	}" );
	
	pShader->m_sPSSource.SetStr( "\
	#version 110\n\
	void main() {}" );

	pShader->m_iFlags |= AGK_SHADER_IS_DEFAULT | AGK_SHADER_IS_CUSTOM;

	InternalCompileShader( pShader, AGK_VK_SHADER_TYPE_NO_LAYOUTS );
	InternalEnumerateShaderResources( pShader );

	return pShader;
}

AGKShader* VulkanRenderer::LoadDefaultShaderShadowBoneAlpha( uint32_t numBones )
{
	AGKShader *pShader = new AGKShader();

	pShader->m_sVSFilename.SetStr( "Shadow Bone Alpha Mask Shader" );
	pShader->m_sPSFilename.SetStr( "Shadow Bone Alpha Mask Shader" );

	char szBoneCount[ 12 ];
	sprintf( szBoneCount, "%d", numBones );

	pShader->m_sVSSource.SetStr( "\
	#version 110\n\
	attribute vec3 position;\n\
	attribute vec4 boneweights;\n\
	attribute vec4 boneindices;\n\
	attribute vec2 uv;\n\
	\
	varying vec2 uvVarying;\n\
	\
	uniform vec4 uvBounds0;\n\
	uniform vec4 textureBounds0;\n\
	\
	uniform mat4 agk_ShadowProj;\n\
	uniform vec4 agk_bonequats1[" ); pShader->m_sVSSource.AppendAscii(szBoneCount); pShader->m_sVSSource.AppendAscii("];\n\
	uniform vec4 agk_bonequats2[" ); pShader->m_sVSSource.AppendAscii(szBoneCount); pShader->m_sVSSource.AppendAscii("];\n\
	\
	vec3 transformDQ( vec3 p, vec4 q1, vec4 q2 ) {\n\
	    p += 2.0 * cross( q1.xyz, cross(q1.xyz, p) + q1.w*p );\n\
	    p += 2.0 * (q1.w*q2.xyz - q2.w*q1.xyz + cross(q1.xyz,q2.xyz));\n\
	    return p;\n\
	}\n\
	\
	void main() { \n\
		vec4 q1 = agk_bonequats1[ int(boneindices.x) ] * boneweights.x;\n\
		q1 += agk_bonequats1[ int(boneindices.y) ] * boneweights.y;\n\
		q1 += agk_bonequats1[ int(boneindices.z) ] * boneweights.z;\n\
		q1 += agk_bonequats1[ int(boneindices.w) ] * boneweights.w;\n\
		\
		vec4 q2 = agk_bonequats2[ int(boneindices.x) ] * boneweights.x;\n\
		q2 += agk_bonequats2[ int(boneindices.y) ] * boneweights.y;\n\
		q2 += agk_bonequats2[ int(boneindices.z) ] * boneweights.z;\n\
		q2 += agk_bonequats2[ int(boneindices.w) ] * boneweights.w;\n\
		\
		float len = 1.0/length(q1);\n\
		q1 *= len;\n\
		q2 = (q2 - q1*dot(q1,q2)) * len;\n\
		\
		gl_Position = agk_ShadowProj * vec4( transformDQ(position,q1,q2), 1.0 );\n\
	}" );
	
	pShader->m_sPSSource.SetStr( "\
	#version 110\n\
	uniform sampler2D texture0;\n\
	varying vec2 uvVarying;\n\
	void main() {\n\
	    float alpha = texture2D(texture0, uvVarying).a;\n\
	    if ( alpha < 0.5 ) discard;\n\
	}" );

	pShader->m_iFlags |= AGK_SHADER_IS_DEFAULT | AGK_SHADER_IS_CUSTOM;

	InternalCompileShader( pShader, AGK_VK_SHADER_TYPE_NO_LAYOUTS );
	InternalEnumerateShaderResources( pShader );

	return pShader;
}

AGKShader* VulkanRenderer::LoadDefaultShader3DParticlesTex()
{
	AGKShader *pShader = new AGKShader();

	pShader->m_sVSFilename.SetStr( "Default 3D Particles Tex Shader" );
	pShader->m_sPSFilename.SetStr( "Default 3D Particles Tex Shader" );

	pShader->m_sVSSource.SetStr( "\
	#version 110\n\
	attribute vec3 position;\n\
	attribute vec4 color;\n\
	attribute vec2 uv;\n\
	\
	varying vec2 uvVarying;\n\
	varying vec4 colorVarying;\n\
	\
	uniform mat4 agk_ViewProj;\n\
	\
	void main() {\n\
		gl_Position = agk_ViewProj * vec4(position,1.0);\n\
		uvVarying = uv;\n\
		colorVarying = color;\n\
	}" );

	pShader->m_sPSSource.SetStr( "\
	#version 110\n\
	uniform sampler2D texture0;\n\
	varying vec2 uvVarying;\n\
	varying vec4 colorVarying;\n\
	\
	void main() { \n\
		gl_FragColor = texture2D(texture0, uvVarying) * colorVarying;\n\
	}" );

	pShader->m_iFlags |= AGK_SHADER_IS_DEFAULT;

	InternalCompileShader( pShader, AGK_VK_SHADER_TYPE_NO_LAYOUTS );
	InternalEnumerateShaderResources( pShader );

	return pShader;
}

AGKShader* VulkanRenderer::LoadDefaultShader3DParticlesColor()
{
	AGKShader *pShader = new AGKShader();

	pShader->m_sVSFilename.SetStr( "Default 3D Particles Color Shader" );
	pShader->m_sPSFilename.SetStr( "Default 3D Particles Color Shader" );

	pShader->m_sVSSource.SetStr( "\
	#version 110\n\
	attribute vec3 position;\n\
	attribute vec4 color;\n\
	\
	varying vec4 colorVarying;\n\
	\
	uniform mat4 agk_ViewProj;\n\
	\
	void main() {\n\
		gl_Position = agk_ViewProj * vec4(position,1.0);\n\
		colorVarying = color;\n\
	}" );

	pShader->m_sPSSource.SetStr( "\
	#version 110\n\
	varying vec4 colorVarying;\n\
	\
	void main() { \n\
		gl_FragColor = colorVarying;\n\
	}" );

	pShader->m_iFlags |= AGK_SHADER_IS_DEFAULT;

	InternalCompileShader( pShader, AGK_VK_SHADER_TYPE_NO_LAYOUTS );
	InternalEnumerateShaderResources( pShader );

	return pShader;
}

AGKShader* VulkanRenderer::LoadDefaultShaderVideo()
{
    AGKShader *pShader = new AGKShader();

    pShader->m_sVSFilename.SetStr( "Video Fullscreen" );
    pShader->m_sPSFilename.SetStr( "Video Texture Android" );

    pShader->m_sVSSource.SetStr( "\
    attribute vec3 position;\n\
    varying vec2 uvVarying;\n\
    uniform vec4 uvBounds;\n\
    uniform float agk_invert;\n\
    void main() {\n\
        gl_Position = vec4(position.xy*vec2(1.0,agk_invert),0.5,1.0);\n\
        uvVarying = (position.xy*vec2(0.5,-0.5) + 0.5) * uvBounds.xy + uvBounds.zw;\n\
    }" );

                                /*
    pShader->m_sPSSource.SetStr( "\
    #extension GL_OES_EGL_image_external : require\n\
    #ifdef GL_ES\n\
        precision highp float;\n\
    #endif\n\
    uniform samplerExternalOES videoTexture;\n\
    varying highp vec2 uvVarying;\n\
    void main() {\n\
        gl_FragColor = texture2D( videoTexture, uvVarying );\n\
    }" );
                                 */

    pShader->m_iFlags |= AGK_SHADER_IS_DEFAULT;

    InternalCompileShader( pShader, AGK_VK_SHADER_TYPE_NO_LAYOUTS );
    InternalEnumerateShaderResources( pShader );

    return pShader;
}

AGKShader* VulkanRenderer::LoadDefaultShaderDeviceCamera()
{
    AGKShader *pShader = new AGKShader();

    pShader->m_sVSFilename.SetStr( "Camera Fullscreen" );
    pShader->m_sPSFilename.SetStr( "Camera Texture Android" );

    pShader->m_sVSSource.SetStr( "\
    attribute vec3 position;\n\
    varying vec2 uvVarying;\n\
    uniform vec4 orientation;\n\
    uniform float agk_invert;\n\
    void main() {\n\
        gl_Position = vec4(position.xy*vec2(1.0,agk_invert),0.5,1.0);\n\
        vec2 uv = position.xy*vec2(0.5,-0.5);\n\
        uvVarying.x = uv.x*orientation.x + uv.y*orientation.y + 0.5;\n\
        uvVarying.y = uv.x*orientation.z + uv.y*orientation.w + 0.5;\n\
    }" );

                                /*
    pShader->m_sPSSource.SetStr( "\
    #extension GL_OES_EGL_image_external : require\n\
    #ifdef GL_ES\n\
        precision highp float;\n\
    #endif\n\
    uniform samplerExternalOES videoTexture;\n\
    varying highp vec2 uvVarying;\n\
    void main() {\n\
        gl_FragColor = texture2D( videoTexture, uvVarying );\n\
    }" );
                                 */

    pShader->m_iFlags |= AGK_SHADER_IS_DEFAULT;

    InternalCompileShader( pShader, AGK_VK_SHADER_TYPE_NO_LAYOUTS );
    InternalEnumerateShaderResources( pShader );
                                
    pShader->SetConstantByName( "orientation", 1,0,0,1 );

    return pShader;
}

AGKShader* VulkanRenderer::LoadDefaultShaderARAndroid()
{
    AGKShader *pShader = new AGKShader();

    pShader->m_sVSFilename.SetStr( "AR Fullscreen" );
    pShader->m_sPSFilename.SetStr( "AR Texture Android" );

    pShader->m_sVSSource.SetStr( "\
    attribute highp vec3 position;\n\
    varying highp vec2 uvVarying;\n\
    uniform highp vec4 orientation;\n\
    uniform mediump float agk_invert;\n\
    void main() {\n\
        gl_Position = vec4(position.xy*vec2(1.0,agk_invert),0.5,1.0);\n\
        highp vec2 uv = position.xy*vec2(0.5,-0.5);\n\
        uvVarying.x = uv.x*orientation.x + uv.y*orientation.y + 0.5;\n\
        uvVarying.y = uv.x*orientation.z + uv.y*orientation.w + 0.5;\n\
    }" );

                                /*
    pShader->m_sPSSource.SetStr( "\
    #extension GL_OES_EGL_image_external : require\n\
    #ifdef GL_ES\n\
        precision highp float;\n\
    #endif\n\
    uniform samplerExternalOES videoTexture;\n\
    varying highp vec2 uvVarying;\n\
    void main() {\n\
        gl_FragColor = texture2D( videoTexture, uvVarying );\n\
    }" );
                                 */

    pShader->m_iFlags |= AGK_SHADER_IS_DEFAULT;

    InternalCompileShader( pShader, AGK_VK_SHADER_TYPE_NO_LAYOUTS );
    InternalEnumerateShaderResources( pShader );
    
    pShader->SetConstantByName( "orientation", 1,0,0,1 );

    return pShader;
}

AGKShader* VulkanRenderer::LoadDefaultShaderARiOS()
{
    AGKShader *pShader = new AGKShader();

    pShader->m_sVSFilename.SetStr( "AR Fullscreen" );
    pShader->m_sPSFilename.SetStr( "AR Texture iOS" );

    pShader->m_sVSSource.SetStr( "\
    attribute vec3 position;\n\
    varying vec2 uvVarying;\n\
    uniform vec4 orientation;\n\
    uniform float agk_invert;\n\
    void main() {\n\
        gl_Position = vec4(position.xy*vec2(1.0,agk_invert),0.5,1.0);\n\
        vec2 uv = position.xy*vec2(0.5,-0.5);\n\
        uvVarying.x = uv.x*orientation.x + uv.y*orientation.y + 0.5;\n\
        uvVarying.y = uv.x*orientation.z + uv.y*orientation.w + 0.5;\n\
    }" );

    pShader->m_sPSSource.SetStr( "\
    uniform sampler2D texture0;\n\
    uniform sampler2D texture1;\n\
    varying vec2 uvVarying;\n\
    void main() {\n\
        float y = texture2D(texture0, uvVarying).r;\n\
        vec2 uv = texture2D(texture1, uvVarying).rg;\n\
        float red = y + uv.g*1.402 - 0.701;\n\
        float green = y - uv.r*0.3441 - uv.g*0.7141 + 0.5291;\n\
        float blue = y + uv.r*1.772 - 0.886;\n\
        gl_FragColor = vec4( red, green, blue, 1.0 );\n\
    }" );

    pShader->m_iFlags |= AGK_SHADER_IS_DEFAULT;

    InternalCompileShader( pShader, AGK_VK_SHADER_TYPE_NO_LAYOUTS );
    InternalEnumerateShaderResources( pShader );
    
    pShader->SetConstantByName( "orientation", 1,0,0,1 );

    return pShader;
}

AGKShader* VulkanRenderer::LoadDefaultShaderQuad()
{
	AGKShader *pShader = new AGKShader();

	pShader->m_sVSFilename.SetStr( "Default Quad Shader" );
	pShader->m_sPSFilename.SetStr( "Default Quad Shader" );

	pShader->m_sVSSource.SetStr( GetShaderDefaultFullScreenVS() );

	pShader->m_sPSSource.SetStr( "\
	#version 110\n\
	uniform sampler2D texture0;\n\
	varying vec2 uvVarying;\n\
	\
	void main() {\n\
		gl_FragColor = texture2D(texture0, uvVarying);\n\
	}" );

	pShader->m_iFlags |= AGK_SHADER_IS_DEFAULT;

	InternalCompileShader( pShader, AGK_VK_SHADER_TYPE_NO_LAYOUTS );
	InternalEnumerateShaderResources( pShader );

	return pShader;
}

AGKShader* VulkanRenderer::LoadDefaultShaderSkyBoxSun()
{
	AGKShader *pShader = new AGKShader();

	pShader->m_sVSFilename.SetStr( "Default SkyBox Sun Shader" );
	pShader->m_sPSFilename.SetStr( "Default SkyBox Sun Shader" );

	pShader->m_sVSSource.SetStr( "\
	attribute vec3 position;\n\
	\
	varying vec2 uvVarying;\n\
	varying vec2 uvVarying2;\n\
	varying vec2 horizonVarying;\n\
	\
	uniform mat4 agk_World;\n\
	uniform mat4 agk_ViewProj;\n\
	\
	uniform vec2 sunSize;\n\
	uniform float horizonHeight;\n\
	uniform float objectScale; // 1.0 / objectScaleY \n\
	\
	void main() {\n\
	    vec4 pos = agk_World * vec4(position,1.0);\n\
	    gl_Position = agk_ViewProj * pos;\n\
	    gl_Position.z = 0; //gl_Position.w; // force it to the back, inverted depth \n\
		\
	    horizonVarying.x = (pos.y-horizonHeight)*objectScale;\n\
	    horizonVarying.y = step(position.y,0.0);\n\
		\
	    uvVarying = position.xz*sunSize.x + 0.5;\n\
	    uvVarying2 = position.xz*sunSize.y + 0.5;\n\
	}" );

	pShader->m_sPSSource.SetStr( "\
	#version 110\n\
	uniform sampler2D texture0;\n\
	\
	uniform vec3 skyColor;\n\
	uniform vec3 horizonColor;\n\
	uniform vec3 sunColor;\n\
	uniform float horizonSize;\n\
	\
	varying vec2 uvVarying;\n\
	varying vec2 uvVarying2;\n\
	varying vec2 horizonVarying;\n\
	\
	void main() {\n\
	    float horizon = 1.0 - min( horizonSize*horizonVarying.x, 1.0 );\n\
	    horizon *= horizon;\n\
	    vec3 color = mix( skyColor, horizonColor, horizon );\n\
		\
	    vec3 sunColor2 = sunColor*1.5 - color;\n\
	    sunColor2 *= horizonVarying.y;\n\
		\
	    float sunPoint = texture2D(texture0,uvVarying).r;\n\
	    color += sunColor2 * sunPoint*sunPoint;\n\
		\
	    sunPoint = texture2D(texture0,uvVarying2).r;\n\
	    color += 0.2 * sunColor2 * sunPoint;\n\
		\
	    gl_FragColor = vec4(color,1.0);\n\
	}" );

	pShader->m_iFlags |= AGK_SHADER_IS_DEFAULT;

	InternalCompileShader( pShader, AGK_VK_SHADER_TYPE_NO_LAYOUTS );
	InternalEnumerateShaderResources( pShader );

	return pShader;
}

AGKShader* VulkanRenderer::LoadDefaultShaderSkyBoxNoSun()
{
	AGKShader *pShader = new AGKShader();

	pShader->m_sVSFilename.SetStr( "Default SkyBox Shader" );
	pShader->m_sPSFilename.SetStr( "Default SkyBox Shader" );

	pShader->m_sVSSource.SetStr( "\
	attribute vec3 position;\n\
	\
	varying vec2 horizonVarying;\n\
	\
	uniform mat4 agk_World;\n\
	uniform mat4 agk_ViewProj;\n\
	\
	uniform float horizonHeight;\n\
	uniform float objectScale; // 1.0 / objectScaleY \n\
	\
	void main() {\n\
	    vec4 pos = agk_World * vec4(position,1.0);\n\
	    gl_Position = agk_ViewProj * pos;\n\
	    gl_Position.z = 0; //gl_Position.w; // force it to the back, inverted depth \n\
		\
	    horizonVarying.x = (pos.y-horizonHeight)*objectScale;\n\
	}" );

	pShader->m_sPSSource.SetStr( "\
	#version 110\n\
	uniform sampler2D texture0;\n\
	\
	uniform vec3 skyColor;\n\
	uniform vec3 horizonColor;\n\
	uniform float horizonSize;\n\
	\
	varying vec2 horizonVarying;\n\
	\
	void main() {\n\
	    float horizon = 1.0 - min( horizonSize*horizonVarying.x, 1.0 );\n\
		horizon *= horizon;\n\
		vec3 color = mix( skyColor, horizonColor, horizon );\n\
		gl_FragColor = vec4(color,1.0);\n\
	}" );

	pShader->m_iFlags |= AGK_SHADER_IS_DEFAULT;

	InternalCompileShader( pShader, AGK_VK_SHADER_TYPE_NO_LAYOUTS );
	InternalEnumerateShaderResources( pShader );

	return pShader;
}

AGKShader* VulkanRenderer::Make3DShader( cMesh *pMesh )
{
	AGKShader *pShader = new AGKShader();

	pShader->m_sVSFilename.SetStr( "Generated 3D Shader" );
	pShader->m_sPSFilename.SetStr( "Generated 3D Shader" );

	// Vertex shader
	pShader->m_sVSSource.ClearTemp();
	pShader->m_sVSSource.AppendAscii( "attribute vec3 position;\n" );
	if ( pMesh->HasNormals() && pMesh->WantsLighting() ) 
	{
		pShader->m_iFlags |= AGK_SHADER_USES_VS_LIGHTING;

		pShader->m_sVSSource.AppendAscii( "attribute vec3 normal;\n" );
		pShader->m_sVSSource.AppendAscii( "varying vec3 posVarying;\n" );
		pShader->m_sVSSource.AppendAscii( "varying vec3 normalVarying;\n" );
		pShader->m_sVSSource.AppendAscii( "varying vec3 lightVarying;\n" );
		pShader->m_sVSSource.AppendAscii( "vec3 GetVSLighting( vec3 normal, vec3 pos );\n" );
		pShader->m_sVSSource.AppendAscii( "\n");
		if ( !pMesh->HasValidBones() )
		{
			pShader->m_sVSSource.AppendAscii( "uniform mat3 agk_WorldNormal;\n" );
			pShader->m_sVSSource.AppendAscii( "uniform mat4 agk_World;\n" );
		}
		pShader->m_sVSSource.AppendAscii( "uniform mat4 agk_ViewProj;\n" );

		if ( pMesh->HasNormalMap() )
		{
			if ( pMesh->HasTangents() && pMesh->HasBiNormals() )
			{
				pShader->m_sVSSource.AppendAscii( "attribute vec3 tangent;\n" );
				pShader->m_sVSSource.AppendAscii( "attribute vec3 binormal;\n" );
			}
			pShader->m_sVSSource.AppendAscii( "varying vec3 tangentVarying;\n" );
			pShader->m_sVSSource.AppendAscii( "varying vec3 binormalVarying;\n" );
		}
	}
	else
	{
		pShader->m_sVSSource.AppendAscii( "\n");
		if ( pMesh->WantsFog() && agk::GetFogMode() ) pShader->m_sVSSource.AppendAscii( "varying vec3 posVarying;\n" );
		if ( pMesh->HasValidBones() )
		{
			pShader->m_sVSSource.AppendAscii( "uniform mat4 agk_ViewProj;\n" );
		}
		else
		{
			if ( pMesh->WantsFog() && agk::GetFogMode() )
			{
				pShader->m_sVSSource.AppendAscii( "uniform mat4 agk_World;\n" );
				pShader->m_sVSSource.AppendAscii( "uniform mat4 agk_ViewProj;\n" );
			}
			else
			{
				pShader->m_sVSSource.AppendAscii( "uniform mat4 agk_WorldViewProj;\n" );
			}
		}
	}

	if ( pMesh->HasUVs() ) 
	{
		pShader->m_sVSSource.AppendAscii( "attribute vec2 uv;\n" );
		pShader->m_sVSSource.AppendAscii( "varying vec2 uvVarying;\n" );
		pShader->m_sVSSource.AppendAscii( "uniform vec4 uvBounds0;\n" );
		pShader->m_sVSSource.AppendAscii( "\n");
	}
	
	if ( pMesh->HasUV1s() )
	{
		pShader->m_sVSSource.AppendAscii( "attribute vec2 uv1;\n" );
		pShader->m_sVSSource.AppendAscii( "varying vec2 uv1Varying;\n" );
		pShader->m_sVSSource.AppendAscii( "uniform vec4 uvBounds1;\n" );
		pShader->m_sVSSource.AppendAscii( "\n");
	}

	if ( pMesh->HasVertColors() ) 
	{
		pShader->m_sVSSource.AppendAscii( "attribute vec4 color;\n" );
		pShader->m_sVSSource.AppendAscii( "varying vec4 colorVarying;\n" );
		pShader->m_sVSSource.AppendAscii( "\n");
	}

	if ( pMesh->HasValidBones() )
	{
		char szBoneCount[ 10 ];
		sprintf( szBoneCount, "%d", pMesh->GetNumBones() );

		pShader->m_sVSSource.AppendAscii( "attribute vec4 boneweights;\n" );
		pShader->m_sVSSource.AppendAscii( "attribute vec4 boneindices;\n" );
		pShader->m_sVSSource.AppendAscii( "uniform vec4 agk_bonequats1["); pShader->m_sVSSource.AppendAscii(szBoneCount); pShader->m_sVSSource.AppendAscii( "];\n" );
		pShader->m_sVSSource.AppendAscii( "uniform vec4 agk_bonequats2["); pShader->m_sVSSource.AppendAscii(szBoneCount); pShader->m_sVSSource.AppendAscii( "];\n" );
		pShader->m_sVSSource.AppendAscii( "\n");
		pShader->m_sVSSource.AppendAscii( "vec3 transformDQ( vec3 p, vec4 q1, vec4 q2 )\n" );
		pShader->m_sVSSource.AppendAscii( "{\n" );
		pShader->m_sVSSource.AppendAscii( "    p += 2.0 * cross( q1.xyz, cross(q1.xyz, p) + q1.w*p );\n" );
		pShader->m_sVSSource.AppendAscii( "    p += 2.0 * (q1.w*q2.xyz - q2.w*q1.xyz + cross(q1.xyz,q2.xyz));\n" );
		pShader->m_sVSSource.AppendAscii( "    return p;\n" );
		pShader->m_sVSSource.AppendAscii( "}\n" );
		pShader->m_sVSSource.AppendAscii( "\n");
	}

	pShader->m_sVSSource.AppendAscii( "void main()\n" );
	pShader->m_sVSSource.AppendAscii( "{ \n" );
	if ( pMesh->HasUVs() ) 
	{
		pShader->m_sVSSource.AppendAscii( "    uvVarying = uv * uvBounds0.xy + uvBounds0.zw;\n" );
	}
	if ( pMesh->HasUV1s() ) 
	{
		pShader->m_sVSSource.AppendAscii( "    uv1Varying = uv1 * uvBounds1.xy + uvBounds1.zw;\n" );
	}
	if ( pMesh->HasVertColors() ) 
	{
		pShader->m_sVSSource.AppendAscii( "    colorVarying = color;\n" );
	}

	if ( pMesh->HasNormals() && pMesh->WantsLighting() && pMesh->HasNormalMap() )
	{
		if ( !pMesh->HasTangents() || !pMesh->HasBiNormals() )
		{
			pShader->m_sVSSource.AppendAscii( "    vec3 tangent;\n" );
			pShader->m_sVSSource.AppendAscii( "    if ( abs(normal.y) > 0.999 ) tangent = vec3( abs(normal.y),0.0,0.0 );\n" );
			pShader->m_sVSSource.AppendAscii( "    else tangent = normalize( vec3(-normal.z, 0.0, normal.x) );\n" );
			pShader->m_sVSSource.AppendAscii( "    vec3 binormal = normalize( vec3(normal.y*tangent.z, normal.z*tangent.x-normal.x*tangent.z, -normal.y*tangent.x) );\n" );
		}
	}

	if ( pMesh->HasValidBones() )
	{
		pShader->m_sVSSource.AppendAscii( "    vec4 q1 = agk_bonequats1[ int(boneindices.x) ] * boneweights.x;\n" );
		pShader->m_sVSSource.AppendAscii( "    q1 += agk_bonequats1[ int(boneindices.y) ] * boneweights.y;\n" );
		pShader->m_sVSSource.AppendAscii( "    q1 += agk_bonequats1[ int(boneindices.z) ] * boneweights.z;\n" );
		pShader->m_sVSSource.AppendAscii( "    q1 += agk_bonequats1[ int(boneindices.w) ] * boneweights.w;\n" );
		
		pShader->m_sVSSource.AppendAscii( "    vec4 q2 = agk_bonequats2[ int(boneindices.x) ] * boneweights.x;\n" );
		pShader->m_sVSSource.AppendAscii( "    q2 += agk_bonequats2[ int(boneindices.y) ] * boneweights.y;\n" );
		pShader->m_sVSSource.AppendAscii( "    q2 += agk_bonequats2[ int(boneindices.z) ] * boneweights.z;\n" );
		pShader->m_sVSSource.AppendAscii( "    q2 += agk_bonequats2[ int(boneindices.w) ] * boneweights.w;\n" );
		
		pShader->m_sVSSource.AppendAscii( "    float len = 1.0/length(q1);\n" );
		pShader->m_sVSSource.AppendAscii( "    q1 *= len;\n" );
		//pShader->m_sVSSource.AppendAscii( "    q2 *= len;\n" ); // quicker but less accurate?
		pShader->m_sVSSource.AppendAscii( "    q2 = (q2 - q1*dot(q1,q2)) * len;\n" );
		
		pShader->m_sVSSource.AppendAscii( "    vec4 pos = vec4( transformDQ(position,q1,q2), 1.0 );\n" );
		pShader->m_sVSSource.AppendAscii( "    gl_Position = agk_ViewProj * pos;\n");
		if ( pMesh->HasNormals() && pMesh->WantsLighting() )
		{
			pShader->m_sVSSource.AppendAscii( "\n");
			pShader->m_sVSSource.AppendAscii( "    normalVarying = normal + 2.0*cross( q1.xyz, cross(q1.xyz,normal) + q1.w*normal );\n");
			pShader->m_sVSSource.AppendAscii( "    posVarying = pos.xyz;\n");
			pShader->m_sVSSource.AppendAscii( "    lightVarying = GetVSLighting( normalVarying, posVarying );\n");
			if ( pMesh->HasNormalMap() )
			{
				pShader->m_sVSSource.AppendAscii( "    tangentVarying = tangent + 2.0*cross( q1.xyz, cross(q1.xyz,tangent) + q1.w*tangent );\n");
				pShader->m_sVSSource.AppendAscii( "    binormalVarying = binormal + 2.0*cross( q1.xyz, cross(q1.xyz,binormal) + q1.w*binormal );\n");
			}
		}
		else if ( pMesh->WantsFog() && agk::GetFogMode() )
		{
			pShader->m_sVSSource.AppendAscii( "    posVarying = pos.xyz;\n");
		}
	}
	else
	{
		if ( pMesh->HasNormals() && pMesh->WantsLighting() )
		{
			pShader->m_sVSSource.AppendAscii( "    vec4 pos = agk_World * vec4(position,1.0);\n");
			pShader->m_sVSSource.AppendAscii( "    gl_Position = agk_ViewProj * pos;\n");
			pShader->m_sVSSource.AppendAscii( "    vec3 norm = normalize(agk_WorldNormal * normal);\n");
			pShader->m_sVSSource.AppendAscii( "    posVarying = pos.xyz;\n");
			pShader->m_sVSSource.AppendAscii( "    normalVarying = norm;\n");
			pShader->m_sVSSource.AppendAscii( "    lightVarying = GetVSLighting( norm, posVarying );\n");
			if ( pMesh->HasNormalMap() )
			{
				pShader->m_sVSSource.AppendAscii( "    tangentVarying = normalize(agk_WorldNormal * tangent);\n");
				pShader->m_sVSSource.AppendAscii( "    binormalVarying = normalize(agk_WorldNormal * binormal);\n");
			}
		}
		else if ( pMesh->WantsFog() && agk::GetFogMode() )
		{
			pShader->m_sVSSource.AppendAscii( "    vec4 pos = agk_World * vec4(position,1.0);\n");
			pShader->m_sVSSource.AppendAscii( "    gl_Position = agk_ViewProj * pos;\n");
			pShader->m_sVSSource.AppendAscii( "    posVarying = pos.xyz;\n");
		}
		else
		{
			pShader->m_sVSSource.AppendAscii( "    gl_Position = agk_WorldViewProj * vec4(position,1.0);\n");
		}
	}
	
	pShader->m_sVSSource.AppendAscii( "}\n" );

	// Pixel shader
	pShader->m_sPSSource.ClearTemp();

	if ( pMesh->HasNormals() && pMesh->WantsLighting() )
	{
		pShader->m_iFlags |= AGK_SHADER_USES_PS_LIGHTING;

		pShader->m_sPSSource.AppendAscii( "varying vec3 posVarying;\n" );
		pShader->m_sPSSource.AppendAscii( "varying vec3 normalVarying;\n" );
		pShader->m_sPSSource.AppendAscii( "varying vec3 lightVarying;\n" );
		pShader->m_sPSSource.AppendAscii( "vec3 GetPSLighting( vec3 normal, vec3 pos );\n" );

		if ( pMesh->HasNormalMap() )
		{
			pShader->m_sPSSource.AppendAscii( "uniform sampler2D texture2;\n" );
			if ( pMesh->GetImage(2)->HasModifiedUVs() ) pShader->m_sPSSource.AppendAscii( "uniform vec4 textureBounds2;\n" );
			pShader->m_sPSSource.AppendAscii( "varying vec3 tangentVarying;\n" );
			pShader->m_sPSSource.AppendAscii( "varying vec3 binormalVarying;\n" );
			pShader->m_sPSSource.AppendAscii( "uniform vec2 agk_NormalScale;\n" );
		}
	}

	if ( pMesh->WantsFog() && agk::GetFogMode() )
	{
		pShader->m_iFlags |= AGK_SHADER_USES_FOG;

		if ( pShader->NeedsPSLighting() == 0 ) pShader->m_sPSSource.AppendAscii( "varying vec3 posVarying;\n" );
		pShader->m_sPSSource.AppendAscii( "vec3 ApplyFog( vec3 color, vec3 pointPos );\n" );
	}

	if ( pMesh->HasUVs() )
	{
		pShader->m_sPSSource.AppendAscii( "varying vec2 uvVarying;\n" );
		if ( pMesh->HasTexture0() )
		{
			pShader->m_sPSSource.AppendAscii( "uniform sampler2D texture0;\n" );
			if ( pMesh->GetImage(0)->HasModifiedUVs() ) pShader->m_sPSSource.AppendAscii( "uniform vec4 textureBounds0;\n" );
		}
	}

	if ( (pMesh->HasTexture1() || pMesh->HasNormalMap()) && pMesh->HasUV1s() )
	{
		pShader->m_sPSSource.AppendAscii( "varying vec2 uv1Varying;\n" );
	}

	if ( pMesh->HasTexture1() )
	{
		pShader->m_sPSSource.AppendAscii( "uniform sampler2D texture1;\n" );
		if ( pMesh->GetImage(1)->HasModifiedUVs() ) pShader->m_sPSSource.AppendAscii( "uniform vec4 textureBounds1;\n" );
	}

	if ( pMesh->HasVertColors() )
	{
		pShader->m_sPSSource.AppendAscii( "varying vec4 colorVarying;\n" );
	}

	
	pShader->m_sPSSource.AppendAscii( "uniform vec4 agk_MeshDiffuse;\n" );
	pShader->m_sPSSource.AppendAscii( "uniform vec4 agk_MeshEmissive;\n" );

	pShader->m_sPSSource.AppendAscii( "void main()\n" );
	pShader->m_sPSSource.AppendAscii( "{ \n" );
	if ( pMesh->HasVertColors() )
	{
		pShader->m_sPSSource.AppendAscii( "    vec4 blendTex = colorVarying;\n" );
	}
	else
	{
		pShader->m_sPSSource.AppendAscii( "    vec4 blendTex = vec4(1.0,1.0,1.0,1.0);\n" );
	}
	
	int hasLight = 0;
	if ( pMesh->HasNormals() && pMesh->WantsLighting() )
	{
		pShader->m_sPSSource.AppendAscii( "    vec3 norm = normalize(normalVarying);\n" );

		if ( pMesh->HasNormalMap() )
		{
			// if there is a separate UV channel, and it isn't being used by texture 1, then use it for normal mapping, otherwise use the base UV
			if ( pMesh->HasUV1s() && !pMesh->HasTexture1() )
			{
				if ( !pMesh->GetImage(2)->HasModifiedUVs() ) pShader->m_sPSSource.AppendAscii( "    vec2 texture2UV = uv1Varying;\n" );
				else pShader->m_sPSSource.AppendAscii( "    vec2 texture2UV = uv1Varying*textureBounds2.xy + textureBounds2.zw;\n" );
			}
			else
			{
				if ( !pMesh->GetImage(2)->HasModifiedUVs() ) pShader->m_sPSSource.AppendAscii( "    vec2 texture2UV = uvVarying;\n" );
				else pShader->m_sPSSource.AppendAscii( "    vec2 texture2UV = uvVarying*textureBounds2.xy + textureBounds2.zw;\n" );
			}

			pShader->m_sPSSource.AppendAscii( "    vec3 normalmap = texture2D(texture2, texture2UV*agk_NormalScale).xyz;\n" );
			pShader->m_sPSSource.AppendAscii( "    normalmap = normalmap * 2.0 - 1.0;\n" );

			pShader->m_sPSSource.AppendAscii( "    vec3 tangent = normalize(tangentVarying);\n" );
			pShader->m_sPSSource.AppendAscii( "    vec3 binormal = normalize(binormalVarying);\n" );
			pShader->m_sPSSource.AppendAscii( "    mat3 TBN = mat3( tangent, binormal, norm );\n" );
			pShader->m_sPSSource.AppendAscii( "    norm = TBN * normalmap;\n" );
		}

		pShader->m_sPSSource.AppendAscii( "    vec3 light = lightVarying + GetPSLighting( norm, posVarying ); \n" );
		hasLight = 1;
	}
	else 
	{
		pShader->m_sPSSource.AppendAscii( "    vec3 light = vec3(1.0,1.0,1.0);\n" );
	}
	
	if ( pMesh->HasTexture1() )
	{
		if ( pMesh->HasUV1s() ) 
		{
			if ( !pMesh->GetImage(1)->HasModifiedUVs() ) pShader->m_sPSSource.AppendAscii( "    vec4 tex1 = texture2D(texture1, uv1Varying);\n" );
			else pShader->m_sPSSource.AppendAscii( "    vec4 tex1 = texture2D(texture1, uv1Varying*textureBounds1.xy + textureBounds1.zw);\n" );
		}
		else if ( pMesh->HasUVs() ) 
		{
			if ( !pMesh->GetImage(1)->HasModifiedUVs() ) pShader->m_sPSSource.AppendAscii( "    vec4 tex1 = texture2D(texture1, uvVarying);\n" );
			else pShader->m_sPSSource.AppendAscii( "    vec4 tex1 = texture2D(texture1, uvVarying*textureBounds1.xy + textureBounds1.zw);\n" );
		}
		else pShader->m_sPSSource.AppendAscii( "    vec4 tex1 = vec4(1.0,1.0,1.0,1.0);\n" );

		// is it a light map, or a multiplied blending texture
		if ( pMesh->HasLightMap() )
		{
			if ( hasLight ) pShader->m_sPSSource.AppendAscii( "    light += tex1.rgb;\n" );
			else pShader->m_sPSSource.AppendAscii( "    light = tex1.rgb;\n" );
		}
		else
		{
			pShader->m_sPSSource.AppendAscii( "    blendTex *= tex1 * 2.0;\n" );
		}
	}

	//pShader->m_sPSSource.AppendAscii( "   light = clamp(light,0.0,1.0); \n" );
	
	if ( pMesh->HasUVs() && pMesh->HasTexture0() ) 
	{
		if ( !pMesh->GetImage(0)->HasModifiedUVs() ) pShader->m_sPSSource.AppendAscii( "    vec4 texColor = texture2D(texture0, uvVarying);\n" );
		else pShader->m_sPSSource.AppendAscii( "    vec4 texColor = texture2D(texture0, uvVarying*textureBounds0.xy + textureBounds0.zw);\n" );

		pShader->m_sPSSource.AppendAscii( "    gl_FragColor = texColor * blendTex * vec4(light,1.0) * agk_MeshDiffuse + agk_MeshEmissive;\n" );
	}
	else 
	{
		pShader->m_sPSSource.AppendAscii( "    gl_FragColor = blendTex * vec4(light,1.0) * agk_MeshDiffuse + agk_MeshEmissive;\n" );
	}

	if ( pMesh->GetObject() && pMesh->GetObject()->HasAlphaMask() )
	{
		pShader->m_sPSSource.AppendAscii( "    if ( gl_FragColor.a < 0.5 ) discard;\n" );
	}

	if ( pMesh->WantsFog() && agk::GetFogMode() )
	{
		pShader->m_sPSSource.AppendAscii( "    gl_FragColor.rgb = ApplyFog( gl_FragColor.rgb, posVarying );\n" );
	}
	
	pShader->m_sPSSource.AppendAscii( "}\n" );

	pShader->m_iFlags &= ~AGK_SHADER_IS_CUSTOM;
	if ( pShader->NeedsAdditionalCode() ) 
	{
		// if this shader uses lighting or fog it will fail compilation until it has the code generated for it
		pShader->SetValid( true );
	}
	else 
	{
		// no additional code required, good to compile
		InternalCompileShader( pShader, AGK_VK_SHADER_TYPE_NO_LAYOUTS );
		if ( !pShader->IsValid() ) 
		{
			delete pShader;
			return 0;
		}
		
		InternalEnumerateShaderResources( pShader );
		if ( !pShader->IsValid() ) 
		{
			delete pShader;
			return 0;
		}
	}

	return pShader;
}

AGKShader* VulkanRenderer::MakeFinalShader( AGKShader *pBaseShader, int sunActive, int VSLights, int PSLights, int useShadows, int normalMap )
{
	AGKShader *pShader = new AGKShader();

	pShader->m_pBaseShader = pBaseShader;

	pShader->m_sVSFilename.SetStr( "Internal Generated 3D Shader" );
	pShader->m_sPSFilename.SetStr( "Internal Generated 3D Shader" );
	
	pShader->m_sVSSource.SetStr( pBaseShader->m_sVSSource );
	pShader->m_sPSSource.SetStr( pBaseShader->m_sPSSource );
	
	pShader->m_sVSSource.AppendAscii( '\n' );

	if ( pBaseShader->NeedsVSLighting() )
	{
		pShader->m_sVSSource.AppendAscii( "uniform vec3 agk_LightAmbient;\n" );
		pShader->m_sVSSource.AppendAscii( "uniform vec3 agk_DLightDir;\n" );
		pShader->m_sVSSource.AppendAscii( "uniform vec3 agk_DLightColor;\n" );

		if ( VSLights > 0 )
		{
			pShader->m_sVSSource.AppendAscii( "uniform vec4 agk_VSLightPos[" ); pShader->m_sVSSource.AppendInt(VSLights); pShader->m_sVSSource.AppendAscii( "];\n" );
			pShader->m_sVSSource.AppendAscii( "uniform vec4 agk_VSLightColor[" ); pShader->m_sVSSource.AppendInt(VSLights); pShader->m_sVSSource.AppendAscii( "];\n" );
		}
		
		if ( useShadows )
		{
			pShader->m_sVSSource.AppendAscii( "uniform mat4 agk_ShadowProj;\n" );
			pShader->m_sVSSource.AppendAscii( "varying vec4 shadowVarying;\n" );
			pShader->m_sVSSource.AppendAscii( "varying float depth;\n" );
			if ( agk::GetShadowMappingMode() == 3 )
			{
				// cascade shadow mapping
				pShader->m_sVSSource.AppendAscii( "uniform mat4 agk_Shadow2Proj;\n" );
				pShader->m_sVSSource.AppendAscii( "uniform mat4 agk_Shadow3Proj;\n" );
				pShader->m_sVSSource.AppendAscii( "uniform mat4 agk_Shadow4Proj;\n" );

				pShader->m_sVSSource.AppendAscii( "varying vec3 shadow2Varying;\n" );
				pShader->m_sVSSource.AppendAscii( "varying vec3 shadow3Varying;\n" );
				pShader->m_sVSSource.AppendAscii( "varying vec3 shadow4Varying;\n" );
			}
		}

		// Vertex shader
		pShader->m_sVSSource.AppendAscii( "vec3 GetVSLighting( vec3 normal, vec3 pos )\n" );
		pShader->m_sVSSource.AppendAscii( "{\n" );

		if ( useShadows ) 
		{
			pShader->m_sVSSource.AppendAscii( "    shadowVarying = agk_ShadowProj * vec4(pos,1.0);\n");
			if ( agk::GetShadowMappingMode() != 2 ) // not LiSPM
			{
				pShader->m_sVSSource.AppendAscii( "    shadowVarying.xy = shadowVarying.xy*0.5 + 0.5;\n");
				//pShader->m_sVSSource.AppendAscii( "    shadowVarying.z = shadowVarying.z*0.495 + 0.505;\n");
			}
			if ( agk::GetShadowMappingMode() == 3 ) // cascade
			{
				pShader->m_sVSSource.AppendAscii( "    shadow2Varying = (agk_Shadow2Proj * vec4(pos,1.0)).xyz;\n");
				pShader->m_sVSSource.AppendAscii( "    shadow3Varying = (agk_Shadow3Proj * vec4(pos,1.0)).xyz;\n");
				pShader->m_sVSSource.AppendAscii( "    shadow4Varying = (agk_Shadow4Proj * vec4(pos,1.0)).xyz;\n");

				pShader->m_sVSSource.AppendAscii( "    shadow2Varying.xy = shadow2Varying.xy*0.5 + 0.5;\n");
				//pShader->m_sVSSource.AppendAscii( "    shadow2Varying.z = shadow2Varying.z*0.495 + 0.505;\n");

				pShader->m_sVSSource.AppendAscii( "    shadow3Varying.xy = shadow3Varying.xy*0.5 + 0.5;\n");
				//pShader->m_sVSSource.AppendAscii( "    shadow3Varying.z = shadow3Varying.z*0.495 + 0.505;\n");

				pShader->m_sVSSource.AppendAscii( "    shadow4Varying.xy = shadow4Varying.xy*0.5 + 0.5;\n");
				//pShader->m_sVSSource.AppendAscii( "    shadow4Varying.z = shadow4Varying.z*0.495 + 0.505;\n");
			}
			pShader->m_sVSSource.AppendAscii( "    depth = gl_Position.w;\n");
		}

		// start with ambient and directional light
		pShader->m_sVSSource.AppendAscii( "    vec3 light = agk_LightAmbient;\n" );
		// if shadows or normal maps are active then the directional light has to be calculated in the pixel shader
		if ( useShadows == 0 && normalMap == 0 ) pShader->m_sVSSource.AppendAscii( "    light += max(dot(-agk_DLightDir, normal),0.0) * agk_DLightColor;\n" );
		
		for ( int i = 0; i < VSLights; ++i )
		{
			if ( i == 0 ) pShader->m_sVSSource.AppendAscii( "    vec3 dir2;\n float atten;\n float lightRange;\n float intensity;\n" );
			pShader->m_sVSSource.AppendAscii( "    dir2 = (agk_VSLightPos[" ); pShader->m_sVSSource.AppendInt(i); pShader->m_sVSSource.AppendAscii( "].xyz - pos);\n" );
			pShader->m_sVSSource.AppendAscii( "    lightRange = agk_VSLightPos[" ); pShader->m_sVSSource.AppendInt(i); pShader->m_sVSSource.AppendAscii( "].w;\n" );
			pShader->m_sVSSource.AppendAscii( "    atten = max(0.0, 1.0 - dot(dir2,dir2)/lightRange);\n" ); // squared exponetial fall off
			pShader->m_sVSSource.AppendAscii( "    atten *= atten; atten *= atten;\n" ); // squared exponetial fall off
			//pShader->m_sVSSource.AppendAscii( "    atten = clamp(atten,0.0,1.0);\n" );
			pShader->m_sVSSource.AppendAscii( "    intensity = max(0.0,dot(normalize(dir2),normal));\n" );
			pShader->m_sVSSource.AppendAscii( "    light += agk_VSLightColor[" ); pShader->m_sVSSource.AppendInt(i); pShader->m_sVSSource.AppendAscii( "].xyz * intensity * atten; \n" );
		}

		pShader->m_sVSSource.AppendAscii( "    return light;\n" );
		pShader->m_sVSSource.AppendAscii( "}\n" );
	}

	// Pixel shader
	pShader->m_sPSSource.AppendAscii( '\n' );

	// fog and shadows use the directional light
	pShader->m_sPSSource.AppendAscii( "uniform vec3 agk_DLightDir;\n" );
	pShader->m_sPSSource.AppendAscii( "uniform vec3 agk_DLightColor;\n" );

	if ( pBaseShader->NeedsPSLighting() )
	{
		if ( useShadows ) 
		{
			pShader->m_sPSSource.AppendAscii( "uniform sampler2D shadowMap;\n" );
			pShader->m_sPSSource.AppendAscii( "varying vec4 shadowVarying;\n" );
			pShader->m_sPSSource.AppendAscii( "varying float depth;\n" );
			pShader->m_sPSSource.AppendAscii( "uniform vec4 agk_ShadowParams;\n" );
			
			if ( agk::GetShadowMappingMode() == 3 ) // cascade shadows
			{
				pShader->m_sPSSource.AppendAscii( "uniform sampler2D shadowMap2;\n" );
				pShader->m_sPSSource.AppendAscii( "uniform sampler2D shadowMap3;\n" );
				pShader->m_sPSSource.AppendAscii( "uniform sampler2D shadowMap4;\n" );

				pShader->m_sPSSource.AppendAscii( "varying vec3 shadow2Varying;\n" );
				pShader->m_sPSSource.AppendAscii( "varying vec3 shadow3Varying;\n" );
				pShader->m_sPSSource.AppendAscii( "varying vec3 shadow4Varying;\n" );

				pShader->m_sPSSource.AppendAscii( "uniform vec4 agk_ShadowParams2;\n" );
			}
		}

		if ( PSLights > 0 )
		{
			pShader->m_sPSSource.AppendAscii( "uniform vec4 agk_PSLightPos[" ); pShader->m_sPSSource.AppendInt(PSLights); pShader->m_sPSSource.AppendAscii( "];\n" );
			pShader->m_sPSSource.AppendAscii( "uniform vec4 agk_PSLightColor[" ); pShader->m_sPSSource.AppendInt(PSLights); pShader->m_sPSSource.AppendAscii( "];\n" );
		}
				
		pShader->m_sPSSource.AppendAscii( "vec3 GetPSLighting( vec3 normal, vec3 pos )\n" );
		pShader->m_sPSSource.AppendAscii( "{\n" );
		pShader->m_sPSSource.AppendAscii( "    float scale = 0.01;\n" );
		pShader->m_sPSSource.AppendAscii( "    float scale2 = 0.0001;\n" );
		pShader->m_sPSSource.AppendAscii( "    vec3 light = vec3(0.0,0.0,0.0);\n" );

		if ( useShadows ) 
		{
			// calculate directional light and its shadow
			pShader->m_sPSSource.AppendAscii( "    float dotp = dot(-agk_DLightDir, normal);\n" );
			pShader->m_sPSSource.AppendAscii( "    if ( dotp > 0.0 ) {\n" );
			pShader->m_sPSSource.AppendAscii( "    vec3 dirLight = dotp * agk_DLightColor;\n" );
			//pShader->m_sPSSource.AppendAscii( "    float bias = min(agk_ShadowParams.y*tan(acos(dotp)),0.01);\n" );
			pShader->m_sPSSource.AppendAscii( "    dotp = 1.0-dotp;\n" );
			pShader->m_sPSSource.AppendAscii( "    dotp = dotp*dotp; dotp = dotp*dotp;\n" );
			pShader->m_sPSSource.AppendAscii( "    float bias = agk_ShadowParams.y*10.0*dotp + agk_ShadowParams.y;\n" );

			if ( agk::GetShadowSmoothing() == 1 ) // fixed multisample
			{
				pShader->m_sPSSource.AppendAscii( "    int samples = 4; float sampleFactor = 0.25;\n" );
				pShader->m_sPSSource.AppendAscii( "    vec2 pd[4];\n" );
				pShader->m_sPSSource.AppendAscii( "    pd[0] = vec2(0.5785557, 0.3818373);\n" );
				pShader->m_sPSSource.AppendAscii( "    pd[1] = vec2(-0.5812719, -0.451035);\n" );
				pShader->m_sPSSource.AppendAscii( "    pd[2] = vec2(-0.4778019, 0.4936384);\n" );
				pShader->m_sPSSource.AppendAscii( "    pd[3] = vec2(0.5724188, -0.4114415);\n" );
			}
			else if ( agk::GetShadowSmoothing() == 2 ) // random multisample
			{
				pShader->m_sPSSource.AppendAscii( "    int samples = 4; float sampleFactor = 0.25;\n" );
				pShader->m_sPSSource.AppendAscii( "    vec2 pd[4];\n" );
				pShader->m_sPSSource.AppendAscii( "    float base = dot(floor(pos*1000.0),vec3(0.0129898,0.078233,0.045164));\n" );
				pShader->m_sPSSource.AppendAscii( "    for( int i = 0; i < 4; i++ ) {\n" );
				pShader->m_sPSSource.AppendAscii( "        float random = sin(float(i)*94.673 + base);\n" );
				//pShader->m_sPSSource.AppendAscii( "        float random = dot(vec4(gl_FragCoord.xyy,i), vec4(12.9898,78.233,45.164,94.673));\n" );
				pShader->m_sPSSource.AppendAscii( "        float randomX = fract(random * 43758.5453);\n" );
				pShader->m_sPSSource.AppendAscii( "        float randomY = fract(random * 55204.6122);\n" );
				pShader->m_sPSSource.AppendAscii( "        pd[i].x = randomX*2.0 - 1.0;\n" );
				pShader->m_sPSSource.AppendAscii( "        pd[i].y = randomY*2.0 - 1.0;\n" );
				pShader->m_sPSSource.AppendAscii( "    }\n" );
			}
			else if ( agk::GetShadowSmoothing() == 3 ) // 16 fixed sample
			{
				pShader->m_sPSSource.AppendAscii( "    int samples = 16; float sampleFactor = 0.0625;\n" );
				pShader->m_sPSSource.AppendAscii( "    vec2 pd[16];\n" );
				pShader->m_sPSSource.AppendAscii( "    pd[0] = vec2( -0.94201624, -0.39906216 );\n" ); 
				pShader->m_sPSSource.AppendAscii( "    pd[1] = vec2( 0.94558609, -0.76890725 );\n" ); 
				pShader->m_sPSSource.AppendAscii( "    pd[2] = vec2( -0.094184101, -0.92938870 );\n" ); 
				pShader->m_sPSSource.AppendAscii( "    pd[3] = vec2( 0.34495938, 0.29387760 );\n" ); 
				pShader->m_sPSSource.AppendAscii( "    pd[4] = vec2( -0.91588581, 0.45771432 );\n" ); 
				pShader->m_sPSSource.AppendAscii( "    pd[5] = vec2( -0.81544232, -0.87912464 );\n" ); 
				pShader->m_sPSSource.AppendAscii( "    pd[6] = vec2( -0.38277543, 0.27676845 );\n" ); 
				pShader->m_sPSSource.AppendAscii( "    pd[7] = vec2( 0.97484398, 0.75648379 );\n" ); 
				pShader->m_sPSSource.AppendAscii( "    pd[8] = vec2( 0.44323325, -0.97511554 );\n" ); 
				pShader->m_sPSSource.AppendAscii( "    pd[9] = vec2( 0.53742981, -0.47373420 );\n" ); 
				pShader->m_sPSSource.AppendAscii( "    pd[10] = vec2( -0.26496911, -0.41893023 );\n" ); 
				pShader->m_sPSSource.AppendAscii( "    pd[11] = vec2( 0.79197514, 0.19090188 );\n" ); 
				pShader->m_sPSSource.AppendAscii( "    pd[12] = vec2( -0.24188840, 0.99706507 );\n" ); 
				pShader->m_sPSSource.AppendAscii( "    pd[13] = vec2( -0.81409955, 0.91437590 );\n" ); 
				pShader->m_sPSSource.AppendAscii( "    pd[14] = vec2( 0.19984126, 0.78641367 );\n" ); 
				pShader->m_sPSSource.AppendAscii( "    pd[15] = vec2( 0.14383161, -0.14100790 );\n" ); 
			}
			else if ( agk::GetShadowSmoothing() == 4 ) // 16 random samples
			{
				pShader->m_sPSSource.AppendAscii( "    int samples = 16; float sampleFactor = 0.0625;\n" );
				pShader->m_sPSSource.AppendAscii( "    vec2 pd[16];\n" );
				pShader->m_sPSSource.AppendAscii( "    float base = dot(floor(pos*1000.0),vec3(0.0129898,0.078233,0.045164));\n" );
				pShader->m_sPSSource.AppendAscii( "    for( int i = 0; i < samples; i++ ) {\n" );
				pShader->m_sPSSource.AppendAscii( "        float random = sin(float(i)*94.673 + base);\n" );
				pShader->m_sPSSource.AppendAscii( "        float randomX = fract(random * 43758.5453);\n" );
				pShader->m_sPSSource.AppendAscii( "        float randomY = fract(random * 55204.6122);\n" );
				pShader->m_sPSSource.AppendAscii( "        pd[i].x = randomX*4.0 - 2.0;\n" );
				pShader->m_sPSSource.AppendAscii( "        pd[i].y = randomY*4.0 - 2.0;\n" );
				pShader->m_sPSSource.AppendAscii( "    }\n" );
			}

			if ( agk::GetShadowMappingMode() == 1 || agk::GetShadowMappingMode() == 2 ) // Uniform and LiPSM
			{
				pShader->m_sPSSource.AppendAscii( "    vec3 shadowUV = shadowVarying.xyz;\n");
				if ( agk::GetShadowMappingMode() == 2 ) // LiPSM
				{
					pShader->m_sPSSource.AppendAscii( "    shadowUV.xyz = shadowUV / shadowVarying.w;\n");
					pShader->m_sPSSource.AppendAscii( "    shadowUV.xy = shadowUV.xy*0.5 + 0.5;\n");
					//pShader->m_sPSSource.AppendAscii( "    shadowUV.z = shadowUV.z*0.49995 + 0.50005;\n");
				}

				// on Vulkan depth is inverted so add bias instead of subtract
				pShader->m_sPSSource.AppendAscii( "    float pixelDepth = shadowUV.z + bias;\n" );
				if ( !agk::GetRenderer()->SupportsPSHighP() ) pShader->m_sPSSource.AppendAscii( "    pixelDepth += bias;\n" );

				if ( agk::GetShadowSmoothing() == 0 )
				{
					pShader->m_sPSSource.AppendAscii( "    float shadow = step(0.0,texture2D( shadowMap, shadowUV.xy ).r - pixelDepth);\n" );
				}
				else 
				{
					//pShader->m_sPSSource.AppendAscii( "    float dist = abs(texture2D( shadowMap, shadowUV.xy ).r - pixelDepth);\n" );
					//pShader->m_sPSSource.AppendAscii( "    dist = min( dist*1000.0, 1.0 );\n" );
					pShader->m_sPSSource.AppendAscii( "    float shadow = 0.0;\n" );
					pShader->m_sPSSource.AppendAscii( "    for( int i = 0; i < samples; i++ ) {\n" );
					pShader->m_sPSSource.AppendAscii( "        shadow += step(0.0,texture2D( shadowMap, vec2(shadowUV.xy + pd[i]*agk_ShadowParams.zw) ).r - pixelDepth);\n" );
					pShader->m_sPSSource.AppendAscii( "    }\n" );
					pShader->m_sPSSource.AppendAscii( "    shadow = shadow*sampleFactor;\n" );
				}

				pShader->m_sPSSource.AppendAscii( "    shadow *= step(0.0, agk_ShadowParams.x - depth);\n" );
			}
			else if ( agk::GetShadowMappingMode() == 3 ) // Cascade
			{
				pShader->m_sPSSource.AppendAscii( "    float shadow = 0.0;\n" );
				pShader->m_sPSSource.AppendAscii( "    if( depth < agk_ShadowParams2.w ) {\n");
				pShader->m_sPSSource.AppendAscii( "        float pixelDepth = shadow4Varying.z + bias;\n" );
				if ( agk::GetShadowSmoothing() > 0 ) 
				{
					pShader->m_sPSSource.AppendAscii( "        float dist = abs(texture2D( shadowMap4, shadow4Varying.xy ).r - pixelDepth);\n" );
					pShader->m_sPSSource.AppendAscii( "        dist = min( dist*300.0, 1.0 );\n" );
					pShader->m_sPSSource.AppendAscii( "        for( int i = 0; i < samples; i++ ) shadow += step(0.0,texture2D( shadowMap4, shadow4Varying.xy+pd[i]*agk_ShadowParams.zw*vec2(dist,dist) ).r - pixelDepth);\n" );
					pShader->m_sPSSource.AppendAscii( "        shadow = shadow*sampleFactor;\n" );
				}
				else pShader->m_sPSSource.AppendAscii( "        shadow = step(0.0,texture2D( shadowMap4, shadow4Varying.xy ).r - pixelDepth);\n" );
				pShader->m_sPSSource.AppendAscii( "    }\n");
				pShader->m_sPSSource.AppendAscii( "    else if( depth < agk_ShadowParams2.z ) {\n");
				pShader->m_sPSSource.AppendAscii( "        float pixelDepth = shadow3Varying.z + 0.5*bias;\n" );
				if ( agk::GetShadowSmoothing() > 0 ) 
				{
					pShader->m_sPSSource.AppendAscii( "        float dist = abs(texture2D( shadowMap3, shadow3Varying.xy ).r - pixelDepth);\n" );
					pShader->m_sPSSource.AppendAscii( "        dist = min( dist*300.0, 1.0 );\n" );
					pShader->m_sPSSource.AppendAscii( "        for( int i = 0; i < samples; i++ ) shadow += step(0.0,texture2D( shadowMap3, shadow3Varying.xy+pd[i]*agk_ShadowParams.zw*vec2(dist,dist) ).r - pixelDepth);\n" );
					pShader->m_sPSSource.AppendAscii( "        shadow = shadow*sampleFactor;\n" );
				}
				else pShader->m_sPSSource.AppendAscii( "        shadow = step(0.0,texture2D( shadowMap3, shadow3Varying.xy ).r - pixelDepth);\n" );
				pShader->m_sPSSource.AppendAscii( "    }\n");
				pShader->m_sPSSource.AppendAscii( "    else if( depth < agk_ShadowParams2.y ) {\n");
				pShader->m_sPSSource.AppendAscii( "        float pixelDepth = shadow2Varying.z + 0.25*bias;\n" );
				if ( agk::GetShadowSmoothing() > 0 ) 
				{
					pShader->m_sPSSource.AppendAscii( "        float dist = abs(texture2D( shadowMap2, shadow2Varying.xy ).r - pixelDepth);\n" );
					pShader->m_sPSSource.AppendAscii( "        dist = min( dist*300.0, 1.0 );\n" );
					pShader->m_sPSSource.AppendAscii( "        for( int i = 0; i < samples; i++ ) shadow += step(0.0,texture2D( shadowMap2, shadow2Varying.xy+pd[i]*agk_ShadowParams.zw*vec2(dist,dist) ).r - pixelDepth);\n" );
					pShader->m_sPSSource.AppendAscii( "        shadow = shadow*sampleFactor;\n" );
				}
				else pShader->m_sPSSource.AppendAscii( "        shadow = step(0.0,texture2D( shadowMap2, shadow2Varying.xy ).r - pixelDepth);\n" );
				pShader->m_sPSSource.AppendAscii( "    }\n");
				pShader->m_sPSSource.AppendAscii( "    else if( depth < agk_ShadowParams2.x ) {\n");
				pShader->m_sPSSource.AppendAscii( "        float pixelDepth = shadowVarying.z + bias;\n" );
				pShader->m_sPSSource.AppendAscii( "        shadow = step(0.0,texture2D( shadowMap, shadowVarying.xy ).r - pixelDepth);\n" );
				//if ( agk::GetShadowSmoothing() == 1 ) pShader->m_sPSSource.AppendAscii( "        for( int i = 0; i < 5; i++ ) shadow += step(0.0,texture2D( shadowMap, shadowVarying.xy+pd[i]*agk_ShadowParams.zw ).r - pixelDepth);\n" );
				pShader->m_sPSSource.AppendAscii( "    }\n");
				//pShader->m_sPSSource.AppendAscii( "    light += texture2D( shadowMap2, shadow2Varying.xy ).r;\n" );
			}

			/*
			// linear depth - multisampled
			if ( agk::GetShadowSmoothing() == 1 )
			{
				pShader->m_sPSSource.AppendAscii( "    vec2 uv = shadowUV.xy;\n" );
				pShader->m_sPSSource.AppendAscii( "    shadow += step(0.0,texture2D( shadowMap, vec2(uv.x-agk_ShadowParams.z*0.23550406,uv.y-agk_ShadowParams.w*0.09976554) ).r - pixelDepth);\n" );
				pShader->m_sPSSource.AppendAscii( "    shadow += step(0.0,texture2D( shadowMap, vec2(uv.x+agk_ShadowParams.z*0.23639652,uv.y-agk_ShadowParams.w*0.19222681) ).r - pixelDepth);\n" );
				pShader->m_sPSSource.AppendAscii( "    shadow += step(0.0,texture2D( shadowMap, vec2(uv.x-agk_ShadowParams.z*0.02354603,uv.y-agk_ShadowParams.w*0.23234718) ).r - pixelDepth);\n" );
				pShader->m_sPSSource.AppendAscii( "    shadow += step(0.0,texture2D( shadowMap, vec2(uv.x+agk_ShadowParams.z*0.08623985,uv.y+agk_ShadowParams.w*0.07346941) ).r - pixelDepth);\n" );
				pShader->m_sPSSource.AppendAscii( "    shadow = shadow*0.2;\n" );
			}
			*/

			/*
			// nearest depth - bilinear filtering
			if ( agk::GetShadowSmoothing() == 2 )
			{
				pShader->m_sPSSource.AppendAscii( "    vec2 uv = shadowUV.xy;\n" );
				pShader->m_sPSSource.AppendAscii( "    float shadow2 = step(0.0,texture2D( shadowMap, vec2(uv.x+agk_ShadowParams.z,uv.y) ).r - pixelDepth);\n" );
				pShader->m_sPSSource.AppendAscii( "    float shadow3 = step(0.0,texture2D( shadowMap, vec2(uv.x,uv.y+agk_ShadowParams.w) ).r - pixelDepth);\n" );
				pShader->m_sPSSource.AppendAscii( "    float shadow4 = step(0.0,texture2D( shadowMap, vec2(uv.x+agk_ShadowParams.z,uv.y+agk_ShadowParams.w) ).r - pixelDepth);\n" );
				pShader->m_sPSSource.AppendAscii( "    vec2 f = fract(shadowUV.xy/agk_ShadowParams.zw);\n" );
				pShader->m_sPSSource.AppendAscii( "    shadow = mix(shadow, shadow2, f.x);\n" );
				pShader->m_sPSSource.AppendAscii( "    shadow3 = mix(shadow3, shadow4, f.x);\n" );
				pShader->m_sPSSource.AppendAscii( "    shadow = mix(shadow, shadow3, f.y);\n" );
			}
			*/
									
			
			pShader->m_sPSSource.AppendAscii( "    light += dirLight*(1.0-shadow);\n" );
			//pShader->m_sPSSource.AppendAscii( "    float color = 1.0-shadowUV.z;\n" );
			//pShader->m_sPSSource.AppendAscii( "    light += texture2D( shadowMap, shadowVarying.xy ).r;\n" );
			pShader->m_sPSSource.AppendAscii( "    }\n" );
		}
		else if ( normalMap )
		{
			pShader->m_sPSSource.AppendAscii( "    light += max(dot(-agk_DLightDir, normal),0.0) * agk_DLightColor;\n\n" );
		}
		
		for ( int i = 0; i < PSLights; ++i )
		{
			if ( i == 0 ) pShader->m_sPSSource.AppendAscii( "    vec3 dir;\n float atten;\n float lightRange;\n float intensity;\n" );
			pShader->m_sPSSource.AppendAscii( "    dir = (agk_PSLightPos[" ); pShader->m_sPSSource.AppendInt(i); pShader->m_sPSSource.AppendAscii( "].xyz - pos) * scale;\n" );
			pShader->m_sPSSource.AppendAscii( "    lightRange = agk_PSLightPos[" ); pShader->m_sPSSource.AppendInt(i); pShader->m_sPSSource.AppendAscii( "].w * scale2;\n" );
			pShader->m_sPSSource.AppendAscii( "    atten = max(0.0, 1.0 - dot(dir,dir)/lightRange);\n" ); // squared exponetial fall off
			pShader->m_sPSSource.AppendAscii( "    atten *= atten; atten *= atten;\n" ); // squared exponetial fall off

			pShader->m_sPSSource.AppendAscii( "    intensity = max(0.0,dot(normalize(dir),normal));\n" );

			pShader->m_sPSSource.AppendAscii( "    light += agk_PSLightColor[" ); pShader->m_sPSSource.AppendInt(i); pShader->m_sPSSource.AppendAscii( "].xyz * intensity * atten; \n" );
		}

		pShader->m_sPSSource.AppendAscii( "    return light;\n" );
		pShader->m_sPSSource.AppendAscii( "}\n" );
	}

	if ( pBaseShader->NeedsFog() )
	{
		if ( agk::GetFogMode() == 0 )
		{
			pShader->m_sPSSource.AppendAscii( "vec3 ApplyFog( vec3 color, vec3 pointPos )\n" );
			pShader->m_sPSSource.AppendAscii( "{\n" );
			pShader->m_sPSSource.AppendAscii( "    return color;\n" );
			pShader->m_sPSSource.AppendAscii( "}\n" );
		}
		else
		{
			int fogColorsEqual = agk::GetFogColorsEqual();

			pShader->m_sPSSource.AppendAscii( '\n' );
			if ( pShader->m_sPSSource.FindStr("vec3 agk_CameraPos") < 0 ) pShader->m_sPSSource.AppendAscii( "uniform vec3 agk_CameraPos;\n" );
			pShader->m_sPSSource.AppendAscii( "uniform vec2 fogRange;\n" ); // = vec2( minDist, -4.0 / (maxDist-minDist) );
			pShader->m_sPSSource.AppendAscii( "uniform vec3 fogColor1;\n" ); // = vec3(0.63,0.73,0.82);
			if ( !fogColorsEqual ) pShader->m_sPSSource.AppendAscii( "uniform vec3 fogColor2;\n" ); // = vec3(1.0,0.9,0.7);
			pShader->m_sPSSource.AppendAscii( '\n' );
			pShader->m_sPSSource.AppendAscii( "vec3 ApplyFog( vec3 color, vec3 pointPos )\n" );
			pShader->m_sPSSource.AppendAscii( "{\n" );
			pShader->m_sPSSource.AppendAscii( "    vec3 viewDir = agk_CameraPos - pointPos;\n" );
			pShader->m_sPSSource.AppendAscii( "    float invDist = inversesqrt(dot(viewDir,viewDir));\n" );
			if ( !fogColorsEqual ) pShader->m_sPSSource.AppendAscii( "    float sunPoint = dot(viewDir*invDist,agk_DLightDir)*0.499 + 0.5;\n" ); // iPad 3 (and maybe others) having floating point issues if exactly 0.5 is used
			pShader->m_sPSSource.AppendAscii( "    invDist = max( 0.0, 1.0/invDist - fogRange.x );\n" );
			pShader->m_sPSSource.AppendAscii( "    invDist = exp( invDist*fogRange.y );\n" ); // variable reuse
			if ( !fogColorsEqual ) pShader->m_sPSSource.AppendAscii( "    vec3 fogColor = mix(fogColor1, fogColor2, pow(sunPoint,24.0));\n" );
			else pShader->m_sPSSource.AppendAscii( "    vec3 fogColor = fogColor1;\n" );
			pShader->m_sPSSource.AppendAscii( "    color = mix( fogColor, color, invDist );\n" );
			pShader->m_sPSSource.AppendAscii( "    return color;\n" );
			pShader->m_sPSSource.AppendAscii( "}\n" );
		}
	}

	if ( pBaseShader->m_iFlags & AGK_SHADER_IS_CUSTOM ) pShader->m_iFlags |= AGK_SHADER_IS_CUSTOM;
	else pShader->m_iFlags &= ~AGK_SHADER_IS_CUSTOM;

	InternalCompileShader( pShader, AGK_VK_SHADER_TYPE_NO_LAYOUTS );
	if ( !pShader->IsValid() ) 
	{
		delete pShader;
		return 0;
	}
	
	InternalEnumerateShaderResources( pShader );
	if ( !pShader->IsValid() ) 
	{
		delete pShader;
		return 0;
	}
	
	return pShader;
}

AGKShader* VulkanRenderer::LoadShader( const char *szVertexSource, const char *szPixelSource, const char *szVertexFile, const char *szPixelFile )
{
	if ( ((uint32_t*)szVertexSource)[0] == SpvMagicNumber || ((uint32_t*)szPixelSource)[0] == SpvMagicNumber )
	{
		AppError( "LoadShader() does not support SPIRV shaders, use LoadShaderSPIRV() instead" );
		return 0;
	}

	AGKShader *pShader = new AGKShader();
	pShader->m_sVSFilename.SetStr( szVertexFile );
	pShader->m_sPSFilename.SetStr( szPixelFile );

	pShader->m_sVSSource.SetStr( szVertexSource );
	pShader->m_sPSSource.SetStr( szPixelSource );

	if ( pShader->m_sVSSource.FindStr( "#version " ) < 0 ) pShader->m_sVSSource.Prepend( "#version 110\n" );
	if ( pShader->m_sPSSource.FindStr( "#version " ) < 0 ) pShader->m_sPSSource.Prepend( "#version 110\n" );

	int type = AGK_VK_SHADER_TYPE_NO_LAYOUTS;

	int layout = pShader->m_sVSSource.FindStr( "layout(" );
	if ( layout < 0 ) layout = pShader->m_sVSSource.FindStr( "layout (" );
	if ( layout < 0 ) layout = pShader->m_sPSSource.FindStr( "layout(" );
	if ( layout < 0 ) layout = pShader->m_sPSSource.FindStr( "layout (" );
	if ( layout >= 0 ) type = AGK_VK_SHADER_TYPE_USES_LAYOUTS;

	pShader->m_sVSSource.ReplaceStr( "mediump ", "" );
	pShader->m_sVSSource.ReplaceStr( "highp ", "" );
	pShader->m_sVSSource.ReplaceStr( "lowp ", "" );
	pShader->m_sVSSource.ReplaceStr( "precision float;", "" );

	pShader->m_sPSSource.ReplaceStr( "mediump ", "" );
	pShader->m_sPSSource.ReplaceStr( "highp ", "" );
	pShader->m_sPSSource.ReplaceStr( "lowp ", "" );
	pShader->m_sPSSource.ReplaceStr( "precision float;", "" );

	pShader->m_iFlags |= AGK_SHADER_IS_CUSTOM;

	if ( pShader->m_sVSSource.FindStr( "vec3 GetVSLighting( vec3 normal, vec3 pos );" ) >= 0 ) 
		pShader->m_iFlags |= AGK_SHADER_USES_VS_LIGHTING;

	if ( pShader->m_sPSSource.FindStr( "vec3 GetPSLighting( vec3 normal, vec3 pos );" ) >= 0 ) 
		pShader->m_iFlags |= AGK_SHADER_USES_PS_LIGHTING;

	if ( pShader->m_sPSSource.FindStr( "vec3 ApplyFog( vec3 color, vec3 pointPos );" ) >= 0 ) 
		pShader->m_iFlags |= AGK_SHADER_USES_FOG;

	// if the shader needs lighting then it can't be compiled until it has the lighting section generated
	if ( !pShader->NeedsAdditionalCode() ) 
	{
		InternalCompileShader( pShader, type );
		if ( !pShader->IsValid() ) 
		{
			delete pShader;
			return 0;
		}

		InternalEnumerateShaderResources( pShader );
		if ( !pShader->IsValid() ) 
		{
			delete pShader;
			return 0;
		}
	}
	else 
	{
		if ( type == AGK_VK_SHADER_TYPE_USES_LAYOUTS )
		{
			AppError( "Failed to load shader, defining variable layout locations and bindings is not compatible with the functions GetVSLighting, GetPSLighting, and ApplyFog. You can leave out the layout locations and bindings and AGK will define them internally" );
			delete pShader;
			return 0; 
		}

		pShader->SetValid( true ); // assume it's vaild for now, will find out later when we add the missing code
	}
	
	return pShader;
}

AGKShader* VulkanRenderer::LoadShaderSPIRV( uint32_t *pVertexSource, uint32_t sizeVS, uint32_t *pPixelSource, uint32_t sizePS )
{
	if ( pVertexSource[0] != SpvMagicNumber || pPixelSource[0] != SpvMagicNumber )
	{
		AppError( "LoadShaderSPIRV() does not support GLSL shaders, use LoadShader() instead" );
		return 0;
	}

	AGKShader *pShader = new AGKShader();

	pShader->m_sVSSource.SetStr( "SPIRV" );
	pShader->m_sPSSource.SetStr( "SPIRV" );
	
	pShader->m_iFlags |= AGK_SHADER_IS_CUSTOM;

	InternalCompileShader( pShader, AGK_VK_SHADER_TYPE_SPIRV );
	if ( !pShader->IsValid() ) 
	{
		delete pShader;
		return 0;
	}

	InternalEnumerateShaderResources( pShader );
	if ( !pShader->IsValid() ) 
	{
		delete pShader;
		return 0;
	}
		
	return pShader;
}

void VulkanRenderer::ReloadShader( AGKShader *pShader )
{
	if( pShader->NeedsAdditionalCode() ) return;
	if ( !pShader->m_pInternalResources ) return;

	// todo
}

void VulkanRenderer::DeleteShader( AGKShader *pAGKShader )
{
	if ( !pAGKShader || !pAGKShader->m_pInternalResources ) return;
	VulkanShader* pShader = (VulkanShader*) pAGKShader->m_pInternalResources;
	pShader->m_iFlags |= AGK_VK_SHADER_DELETE;

	m_globalPipelineCache.DeleteShader( pAGKShader );

	pAGKShader->m_pInternalResources = 0;
}

void VulkanRenderer::InternalCompileShader( AGKShader *pAGKShader, int type )
{
	VulkanShader *pVulkanShader = new VulkanShader();
	pAGKShader->m_pInternalResources = pVulkanShader;
	pAGKShader->SetValid( false );

	char *szVS = (char*)pAGKShader->m_sVSSource.GetStr(); 
	char *szPS = (char*)pAGKShader->m_sPSSource.GetStr();
	int deleteSource = 0;

	// convert from GLSL 110 to GLSL 450
	if ( type == AGK_VK_SHADER_TYPE_NO_LAYOUTS )
	{
		szVS = 0; szPS = 0;
		if ( !glsl450_Convert( pAGKShader->m_sVSSource.GetStr(), pAGKShader->m_sPSSource.GetStr(), &szVS, &szPS, pAGKShader->GetBaseVSFilename(), pAGKShader->GetBasePSFilename() ) )
		{
			agk::Error( "Failed to add layout definitions to shader" );
			return;
		}
		deleteSource = 1;
		type = AGK_VK_SHADER_TYPE_USES_LAYOUTS;
	}

	// convert from GLSL450 to SPIRV
	if ( type == AGK_VK_SHADER_TYPE_USES_LAYOUTS )
	{
		uint32_t **outVertSPIRV = &pVulkanShader->m_pVSSourceSPIRV;
		uint32_t **outFragSPIRV = &pVulkanShader->m_pPSSourceSPIRV;
		uint32_t* outVertSize = &pVulkanShader->m_iVSSourceSize;
		uint32_t* outFragSize = &pVulkanShader->m_iPSSourceSize;

		char *szError = 0;
		bool success = glsl450_OutputSpirV( 0, szVS, outVertSPIRV, outVertSize, &szError );
		if ( !success )
		{
			if ( m_iShaderErrorMode < 2 ) AppError( "Failed to convert vertex \"%s\" (based on \"%s\") shader to SPIRV: %s", pAGKShader->GetVSFilename(), pAGKShader->GetBaseVSFilename(), szError ? szError : "Unknown error" ); 
			if ( szError ) delete szError;
			szError = 0;
			return; 
		}
		success = glsl450_OutputSpirV( 4, szPS, outFragSPIRV, outFragSize, &szError );
		if ( !success )
		{
			if ( m_iShaderErrorMode < 2 ) AppError( "Failed to convert pixel \"%s\" (based on \"%s\") shader to SPIRV: %s", pAGKShader->GetPSFilename(), pAGKShader->GetBasePSFilename(), szError ? szError : "Unknown error" );
			if ( szError ) delete szError;
			szError = 0;
			return; 
		}

		pVulkanShader->m_iVSSourceSize *= 4;
		pVulkanShader->m_iPSSourceSize *= 4;

		type = AGK_VK_SHADER_TYPE_SPIRV;
	}

	if ( deleteSource )
	{
		delete [] szVS;
		delete [] szPS;
	}

	if ( type != AGK_VK_SHADER_TYPE_SPIRV ) 
	{ 
		if ( m_iShaderErrorMode == 0 ) AppError( "Unrecognised shader type: %d", type ); 
		return; 
	}

	// compile SPIRV to Vulkan shader module
	VkShaderModuleCreateInfo shaderInfo = {};
	shaderInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	shaderInfo.codeSize = pVulkanShader->m_iVSSourceSize;
	shaderInfo.pCode = pVulkanShader->m_pVSSourceSPIRV;

	VkResult result = vkCreateShaderModule( m_vkDevice, &shaderInfo, nullptr, &pVulkanShader->m_vkVertShader );
	if ( VK_SUCCESS != result ) 
	{
		if ( m_iShaderErrorMode < 2 ) AppError( "Failed to compile SPIRV code from vertex shader" );
		return;
	}

	shaderInfo.codeSize = pVulkanShader->m_iPSSourceSize;
	shaderInfo.pCode = pVulkanShader->m_pPSSourceSPIRV;

	result = vkCreateShaderModule( m_vkDevice, &shaderInfo, nullptr, &pVulkanShader->m_vkFragShader );
	if ( VK_SUCCESS != result ) 
	{
		if ( m_iShaderErrorMode < 2 ) AppError( "Failed to compile SPIRV code from pixel shader" );
		return;
	}

	pAGKShader->SetValid( true );
}

void VulkanRenderer::InternalEnumerateShaderResources( AGKShader *pAGKShader )
{
	if ( !pAGKShader || !pAGKShader->m_pInternalResources ) return;
	VulkanShader *pShader = (VulkanShader*) pAGKShader->m_pInternalResources;

	if ( pShader->m_iNumDescriptorSets > 0 )
	{
		agk::Error( "InternalEnumerateShaderResources called on a shader that already has descriptor data" );
		return;
	}

	uint32_t bindingAlignment = (uint32_t) m_vkGPUProperties.limits.minUniformBufferOffsetAlignment;
	for( int i = 0; i < AGK_SHADER_CONSTANT_NAME_MAX_VALUE; i++ ) pShader->m_indexByKnownName[ i ] = (uint16_t) -1;
	for( int i = 0; i < AGK_MAX_TEXTURE_STAGES; i++ ) pShader->m_indexByTexStage[ i ] = (uint16_t) -1;
	pAGKShader->m_constantBindingByName.ClearAll();
	pAGKShader->m_localConstants.Clear( true );
	
	SpvReflectShaderModule reflectModuleVS;
	SpvReflectResult refRes = spvReflectCreateShaderModule( pShader->m_iVSSourceSize, pShader->m_pVSSourceSPIRV, &reflectModuleVS );
	if ( SPV_REFLECT_RESULT_SUCCESS != refRes ) 
	{
		if ( m_iShaderErrorMode < 2 ) AppError( "SPIR-V Reflect VS module error: %d", refRes );
		pAGKShader->SetValid( false );
		return;
	}

	SpvReflectShaderModule reflectModulePS;
	refRes = spvReflectCreateShaderModule( pShader->m_iPSSourceSize, pShader->m_pPSSourceSPIRV, &reflectModulePS );
	if ( SPV_REFLECT_RESULT_SUCCESS != refRes ) 
	{
		if ( m_iShaderErrorMode < 2 ) AppError( "SPIR-V Reflect PS module error: %d", refRes );
		pAGKShader->SetValid( false );
		return;
	}

	// shader attributes
	if ( pAGKShader->m_pAttribs ) delete [] pAGKShader->m_pAttribs;
	pAGKShader->m_pAttribs = 0;
	pAGKShader->m_iNumAttribs = 0;

	if ( reflectModuleVS.input_variable_count > 0 )
	{
		pAGKShader->m_iNumAttribs = reflectModuleVS.input_variable_count;
		pAGKShader->m_pAttribs = new AGKShaderAttrib[ pAGKShader->m_iNumAttribs ];

		for( uint32_t i = 0; i < reflectModuleVS.input_variable_count; i++ )
		{
			AGKShaderAttrib *pAttrib = &(pAGKShader->m_pAttribs[ i ]);
			SpvReflectInterfaceVariable &pVSAttrib = reflectModuleVS.input_variables[ i ];

			uint32_t mask = SPV_REFLECT_TYPE_FLAG_FLOAT | SPV_REFLECT_TYPE_FLAG_ARRAY | SPV_REFLECT_TYPE_FLAG_MATRIX;
			if( (pVSAttrib.type_description->type_flags & mask) != SPV_REFLECT_TYPE_FLAG_FLOAT ) 
			{
				if ( m_iShaderErrorMode < 1 ) agk::Error( "Unsupported shader vertex input, only float, vec2, vec3, vec4 are supported" );
				pAGKShader->SetValid( false );
				return;
			}
			
			pAttrib->m_iComponents = 1;
			if( pVSAttrib.type_description->type_flags & SPV_REFLECT_TYPE_FLAG_VECTOR )
			{
				pAttrib->m_iComponents = pVSAttrib.numeric.vector.component_count;
			}

			if ( !pVSAttrib.name || !*pVSAttrib.name )
			{
				if ( m_iShaderErrorMode < 1 ) agk::Error( "SPIRV shader does not contain variable names, it cannot be used by AGK" );
				pAGKShader->SetValid( false );
				return;
			}

			pAttrib->m_iLocation = pVSAttrib.location;
			
			if ( strcmp(pVSAttrib.name, "position") == 0 ) pAttrib->m_iKnownName = AGK_SHADER_ATTRIB_NAME_POS;
			else if ( strcmp(pVSAttrib.name, "normal") == 0 ) pAttrib->m_iKnownName = AGK_SHADER_ATTRIB_NAME_NORM;
			else if ( strcmp(pVSAttrib.name, "tangent") == 0 ) pAttrib->m_iKnownName = AGK_SHADER_ATTRIB_NAME_TANGENT;
			else if ( strcmp(pVSAttrib.name, "binormal") == 0 ) pAttrib->m_iKnownName = AGK_SHADER_ATTRIB_NAME_BINORMAL;
			else if ( strcmp(pVSAttrib.name, "color") == 0 ) pAttrib->m_iKnownName = AGK_SHADER_ATTRIB_NAME_COLOR;
			else if ( strcmp(pVSAttrib.name, "uv") == 0 ) pAttrib->m_iKnownName = AGK_SHADER_ATTRIB_NAME_UV0;
			else if ( strcmp(pVSAttrib.name, "uv1") == 0 ) pAttrib->m_iKnownName = AGK_SHADER_ATTRIB_NAME_UV1;
			else if ( strcmp(pVSAttrib.name, "boneweights") == 0 ) pAttrib->m_iKnownName = AGK_SHADER_ATTRIB_NAME_BONE_WEIGHT;
			else if ( strcmp(pVSAttrib.name, "boneindices") == 0 ) pAttrib->m_iKnownName = AGK_SHADER_ATTRIB_NAME_BONE_INDEX;
			else 
			{
				pAttrib->m_szName = new char[ strlen(pVSAttrib.name)+1 ];
				memcpy( (void*)pAttrib->m_szName, pVSAttrib.name, strlen(pVSAttrib.name)+1 );
			}
		}
	}

	// push constants
	if ( reflectModuleVS.push_constant_block_count > 0 || reflectModulePS.push_constant_block_count > 0 )
	{
		if ( m_iShaderErrorMode == 0 ) agk::Error( "Shader push constants are not currently supported" );
		pAGKShader->SetValid( false );
		return;
	}

	// uniforms

	uint32_t indexVS = 0;
	uint32_t indexPS = 0;
	uint32_t setCount = 0;

	// check sets
	while( indexVS < reflectModuleVS.descriptor_set_count && indexPS < reflectModulePS.descriptor_set_count )
	{
		int diff = (int)reflectModuleVS.descriptor_sets[ indexVS ].set - (int)reflectModulePS.descriptor_sets[ indexPS ].set;
		if ( diff == 0 )
		{
			indexVS++;
			indexPS++;
		}
		else if ( diff > 0 )
		{
			indexPS++;
		}
		else
		{
			indexVS++;
		}

		setCount++;
	}

	if ( indexVS < reflectModuleVS.descriptor_set_count ) setCount += (reflectModuleVS.descriptor_set_count - indexVS);
	else setCount += (reflectModulePS.descriptor_set_count - indexPS);

	// allocate sets
	pShader->m_iNumDescriptorSets = setCount;
	if ( setCount > 0 ) pShader->m_pDescriptorSetStatic = new VulkanDescriptorSetStatic[ setCount ];
	else pShader->m_pDescriptorSetStatic = 0;

	uint32_t iTotalTextures = 0;
	uint32_t iTotalUniforms = 0;

	// check bindings
	indexVS = 0; indexPS = 0;
	for( uint32_t i = 0; i < setCount; i++ )
	{
		VulkanDescriptorSetStatic &pCurrSet = pShader->m_pDescriptorSetStatic[ i ];

		SpvReflectDescriptorSet *pVSSet = 0;
		SpvReflectDescriptorSet *pPSSet = 0;

		if ( indexVS < reflectModuleVS.descriptor_set_count ) pVSSet = &(reflectModuleVS.descriptor_sets[ indexVS ]);
		if ( indexPS < reflectModulePS.descriptor_set_count ) pPSSet = &(reflectModulePS.descriptor_sets[ indexPS ]);

		int diff;
		if ( pVSSet && pPSSet ) diff = (int)pVSSet->set - (int)pPSSet->set;
		else if ( pVSSet ) diff = -1;
		else diff = 1;

		SpvReflectDescriptorBinding **pBindings = 0;
		VkDescriptorSetLayoutBinding *pBindingLayout = 0;

		uint32_t indexBindingVS = 0;
		uint32_t indexBindingPS = 0;
		uint32_t bindingCount = 0;

		if ( diff == 0 )
		{
			// merge bindings from both sets
			pBindings = new SpvReflectDescriptorBinding*[ pVSSet->binding_count + pPSSet->binding_count ]; // overestimate size
			pBindingLayout = new VkDescriptorSetLayoutBinding[ pVSSet->binding_count + pPSSet->binding_count ];

			while( indexBindingVS < pVSSet->binding_count && indexBindingPS < pPSSet->binding_count )
			{
				int diffBinding = (int)pVSSet->bindings[ indexBindingVS ]->binding - (int)pPSSet->bindings[ indexBindingPS ]->binding;
				if ( diffBinding == 0 )
				{
					pBindingLayout[ bindingCount ].stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
					pBindings[ bindingCount ] = pVSSet->bindings[ indexBindingVS ];
					indexBindingVS++;
					indexBindingPS++;
				}
				else if ( diffBinding > 0 )
				{
					pBindingLayout[ bindingCount ].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
					pBindings[ bindingCount ] = pPSSet->bindings[ indexBindingPS ];
					indexBindingPS++;
				}
				else
				{
					pBindingLayout[ bindingCount ].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
					pBindings[ bindingCount ] = pVSSet->bindings[ indexBindingVS ];
					indexBindingVS++;
				}

				bindingCount++;
			}

			// one set finished, use the code below to handle the one remaining set
			if ( indexBindingVS < pVSSet->binding_count ) diff = -1;
			else diff = 1;
		}

		// only dealing with one set, put everything in the vertex variables
		uint32_t flag = VK_SHADER_STAGE_VERTEX_BIT;
		if ( diff > 0 )
		{
			flag = VK_SHADER_STAGE_FRAGMENT_BIT;
			pVSSet = pPSSet;
			indexBindingVS = indexBindingPS;
		}
		
		// may not have been allocated if diff was never 0
		if ( pBindings == 0 ) 
		{
			pBindings = new SpvReflectDescriptorBinding*[ pVSSet->binding_count ];
			pBindingLayout = new VkDescriptorSetLayoutBinding[ pVSSet->binding_count ];
		}

		while( indexBindingVS < pVSSet->binding_count )
		{
			pBindingLayout[ bindingCount ].stageFlags = flag;
			pBindings[ bindingCount ] = pVSSet->bindings[ indexBindingVS ];
			indexBindingVS++;
			bindingCount++;
		}

		pCurrSet.m_iSetID = pVSSet->set;

		// bindings from both sets now in pBindings
		uint32_t uniformCount = 0;
		uint32_t locationCount = 0;
		uint32_t textureCount = 0;
		for( uint32_t j = 0; j < bindingCount; j++ )
		{
			switch( pBindings[ j ]->descriptor_type )
			{
				case SPV_REFLECT_DESCRIPTOR_TYPE_UNIFORM_BUFFER:
				{
					uniformCount++;
					locationCount += pBindings[ j ]->block.member_count;
					break;
				}
				case SPV_REFLECT_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER:
				{
					textureCount++;
					break;
				}
			}
		}
		
		// allocate bindings
		pCurrSet.m_iNumUniformBindings = uniformCount;
		pCurrSet.m_pUniformBindings = new VulkanDescriptorSetBindingStatic[ uniformCount ];
		iTotalUniforms += uniformCount;

		pCurrSet.m_iNumLocations = locationCount;
		pCurrSet.m_pLocations = new VulkanDescriptorSetLocationStatic[ locationCount ];

		pCurrSet.m_iNumTextures = textureCount;
		pCurrSet.m_pTextureBindings = new uint16_t[ textureCount ];
		iTotalTextures += textureCount;
		
		uint32_t bindingOffset = 0;
		uniformCount = 0;
		locationCount = 0;
		textureCount = 0;
		for( uint32_t j = 0; j < bindingCount; j++ )
		{
			SpvReflectDescriptorBinding *pCurrBinding = pBindings[ j ];
			
			pBindingLayout[ j ].pImmutableSamplers = nullptr;
			pBindingLayout[ j ].binding = pCurrBinding->binding;
			pBindingLayout[ j ].descriptorCount = 1;

			switch( pCurrBinding->descriptor_type )
			{
				case SPV_REFLECT_DESCRIPTOR_TYPE_UNIFORM_BUFFER:
				{
					pBindingLayout[ j ].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;

					VulkanDescriptorSetBindingStatic &pCurrUniform = pCurrSet.m_pUniformBindings[ uniformCount ];
					pCurrUniform.m_iBindingID = pCurrBinding->binding;
					if ( pBindingLayout[ j ].stageFlags & VK_SHADER_STAGE_VERTEX_BIT ) pCurrUniform.m_iFlags |= AGK_VK_DESCRIPTOR_BINDING_VERTEX;
					if ( pBindingLayout[ j ].stageFlags & VK_SHADER_STAGE_FRAGMENT_BIT ) pCurrUniform.m_iFlags |= AGK_VK_DESCRIPTOR_BINDING_FRAGMENT;

					pCurrUniform.m_iOffset = bindingOffset;
					pCurrUniform.m_iSize = pCurrBinding->block.size;

					for( uint32_t m = 0; m < pCurrBinding->block.member_count; m++ )
					{
						SpvReflectBlockVariable &pCurrMember = pCurrBinding->block.members[ m ];
						VulkanDescriptorSetLocationStatic &pCurrLocation = pCurrSet.m_pLocations[ locationCount ];
						pCurrLocation.m_iOffset = bindingOffset;
						if( (pCurrMember.type_description->type_flags & SPV_REFLECT_TYPE_FLAG_FLOAT) == 0 && m_iShaderErrorMode < 2 ) agk::Error( "Unsupported shader variable type, only float, vec2, vec3, vec4, mat2, mat3, and mat4 are supported" );
						else
						{
							if( pCurrMember.type_description->type_flags & SPV_REFLECT_TYPE_FLAG_MATRIX )
							{
								pCurrLocation.m_iStride = pCurrMember.numeric.matrix.stride;
								if ( pCurrMember.numeric.matrix.column_count != pCurrMember.numeric.matrix.row_count && m_iShaderErrorMode < 2 ) agk::Error( "Unsupported shader variable type, only square matrices are supported" );
								else
								{
									switch( pCurrMember.numeric.matrix.column_count )
									{
										case 2: pCurrLocation.m_iVarType = AGK_SHADER_CONSTANT_TYPE_MAT2; break;
										case 3: pCurrLocation.m_iVarType = AGK_SHADER_CONSTANT_TYPE_MAT3; break;
										case 4: pCurrLocation.m_iVarType = AGK_SHADER_CONSTANT_TYPE_MAT4; break;
									}
								}
							}
							else if( pCurrMember.type_description->type_flags & SPV_REFLECT_TYPE_FLAG_VECTOR )
							{
								switch( pCurrMember.numeric.vector.component_count )
								{
									case 2: pCurrLocation.m_iVarType = AGK_SHADER_CONSTANT_TYPE_VEC2; break;
									case 3: pCurrLocation.m_iVarType = AGK_SHADER_CONSTANT_TYPE_VEC3; break;
									case 4: pCurrLocation.m_iVarType = AGK_SHADER_CONSTANT_TYPE_VEC4; break;
								}
							}
							else pCurrLocation.m_iVarType = AGK_SHADER_CONSTANT_TYPE_FLOAT;
						}

						if( pCurrMember.type_description->type_flags & SPV_REFLECT_TYPE_FLAG_ARRAY )
						{
							pCurrLocation.m_iArrayStride = pCurrMember.array.stride;
							pCurrLocation.m_iArraySize = pCurrMember.array.dims[0];
						}
						else pCurrLocation.m_iArraySize = 1;

						uint16_t locationIndex = (pCurrSet.m_iSetID << 12) | locationCount;

						const char *szUniformName = pCurrMember.name;

						// check for known uniform names
						if ( strcmp(szUniformName, "fogColor1") == 0 )
						{
							pCurrLocation.m_pDefaultValue = &AGKShader::g_varFogColor1;
							pShader->m_indexByKnownName[ AGK_SHADER_CONSTANT_NAME_FOG_COLOR1 ] = locationIndex;
						}
						else if ( strcmp(szUniformName, "fogColor2") == 0 ) 			
						{
							pCurrLocation.m_pDefaultValue = &AGKShader::g_varFogColor2;
							pShader->m_indexByKnownName[ AGK_SHADER_CONSTANT_NAME_FOG_COLOR2 ] = locationIndex;
						}
						else if ( strcmp(szUniformName, "fogRange") == 0 )				pCurrLocation.m_pDefaultValue = &AGKShader::g_varFogRange;
						else if ( strcmp(szUniformName, "agk_invert") == 0 ) 			pCurrLocation.m_pDefaultValue = &AGKShader::g_varInvert;
						else if ( strcmp(szUniformName, "agk_time") == 0 ) 				pCurrLocation.m_pDefaultValue = &AGKShader::g_varTime;
						else if ( strcmp(szUniformName, "agk_sintime") == 0 ) 			pCurrLocation.m_pDefaultValue = &AGKShader::g_varSinTime;
						else if ( strcmp(szUniformName, "agk_resolution") == 0 ) 		pCurrLocation.m_pDefaultValue = &AGKShader::g_varResolution;
						else if ( strcmp(szUniformName, "agk_ShadowParams") == 0 ) 		pCurrLocation.m_pDefaultValue = &AGKShader::g_varShadowParams;
						else if ( strcmp(szUniformName, "agk_ShadowParams2") == 0 ) 	pCurrLocation.m_pDefaultValue = &AGKShader::g_varShadowParams2;
						else if ( strcmp(szUniformName, "agk_Ortho") == 0 ) 			pCurrLocation.m_pDefaultValue = &AGKShader::g_varOrtho;
						else if ( strcmp(szUniformName, "agk_ShadowProj") == 0 ) 		pCurrLocation.m_pDefaultValue = &AGKShader::g_varShadowProj;
						else if ( strcmp(szUniformName, "agk_Shadow2Proj") == 0 ) 		pCurrLocation.m_pDefaultValue = &AGKShader::g_varShadowProj2;
						else if ( strcmp(szUniformName, "agk_Shadow3Proj") == 0 ) 		pCurrLocation.m_pDefaultValue = &AGKShader::g_varShadowProj3;
						else if ( strcmp(szUniformName, "agk_Shadow4Proj") == 0 ) 		pCurrLocation.m_pDefaultValue = &AGKShader::g_varShadowProj4;
						else if ( strcmp(szUniformName, "agk_DLightDir") == 0 ) 		pCurrLocation.m_pDefaultValue = &AGKShader::g_varDirLightDir;
						else if ( strcmp(szUniformName, "agk_DLightColor") == 0 ) 		pCurrLocation.m_pDefaultValue = &AGKShader::g_varDirLightColor;
						else if ( strcmp(szUniformName, "agk_LightAmbient") == 0 ) 		pCurrLocation.m_pDefaultValue = &AGKShader::g_varLightAmbient;

						else if ( strcmp(szUniformName, "agk_bonequats1") == 0 )		pShader->m_indexByKnownName[ AGK_SHADER_CONSTANT_NAME_OBJ_BONES1 ] = locationIndex;		
						else if ( strcmp(szUniformName, "agk_bonequats2") == 0 ) 		pShader->m_indexByKnownName[ AGK_SHADER_CONSTANT_NAME_OBJ_BONES2 ] = locationIndex;		
						else if ( strcmp(szUniformName, "agk_MeshDiffuse") == 0 ) 		pShader->m_indexByKnownName[ AGK_SHADER_CONSTANT_NAME_MESH_DIFFUSE ] = locationIndex;
						else if ( strcmp(szUniformName, "agk_MeshEmissive") == 0 ) 		pShader->m_indexByKnownName[ AGK_SHADER_CONSTANT_NAME_MESH_EMISSIVE ] = locationIndex;
						else if ( strcmp(szUniformName, "agk_NormalScale") == 0 ) 		pShader->m_indexByKnownName[ AGK_SHADER_CONSTANT_NAME_NORMAL_SCALE ] = locationIndex;
						else if ( strcmp(szUniformName, "agk_CameraPos") == 0 ) 		pShader->m_indexByKnownName[ AGK_SHADER_CONSTANT_NAME_CAMERA_POS ] = locationIndex;
						else if ( strcmp(szUniformName, "agk_spritepos") == 0 ) 		pShader->m_indexByKnownName[ AGK_SHADER_CONSTANT_NAME_SPRITE_POS ] = locationIndex;
						else if ( strcmp(szUniformName, "agk_spritesize") == 0 ) 		pShader->m_indexByKnownName[ AGK_SHADER_CONSTANT_NAME_SPRITE_SIZE ] = locationIndex;
																						
						else if ( strcmp(szUniformName, "agk_World") == 0 ) 			pShader->m_indexByKnownName[ AGK_SHADER_CONSTANT_NAME_WORLD_MAT ] = locationIndex;
						else if ( strcmp(szUniformName, "agk_WorldNormal") == 0 ) 		pShader->m_indexByKnownName[ AGK_SHADER_CONSTANT_NAME_WORLD_NORMAL ] = locationIndex;
						else if ( strcmp(szUniformName, "agk_View") == 0 ) 				pShader->m_indexByKnownName[ AGK_SHADER_CONSTANT_NAME_VIEW ] = locationIndex;
						else if ( strcmp(szUniformName, "agk_Proj") == 0 ) 				pShader->m_indexByKnownName[ AGK_SHADER_CONSTANT_NAME_PROJ ] = locationIndex;
																								
						else if ( strcmp(szUniformName, "agk_ViewProj") == 0 ) 			pShader->m_indexByKnownName[ AGK_SHADER_CONSTANT_NAME_VIEW_PROJ ] = locationIndex;
						else if ( strcmp(szUniformName, "agk_WorldViewProj") == 0 ) 	pShader->m_indexByKnownName[ AGK_SHADER_CONSTANT_NAME_WORLD_VIEW_PROJ ] = locationIndex;
						else if ( strcmp(szUniformName, "agk_ShadowWorldProj") == 0 ) 	pShader->m_indexByKnownName[ AGK_SHADER_CONSTANT_NAME_SHADOW_WORLD_PROJ ] = locationIndex;
																						
						else if ( strcmp(szUniformName, "agk_VSLightPos") == 0 ) 		pShader->m_indexByKnownName[ AGK_SHADER_CONSTANT_NAME_VS_LIGHT_POS ] = locationIndex;
						else if ( strcmp(szUniformName, "agk_VSLightColor") == 0 ) 		pShader->m_indexByKnownName[ AGK_SHADER_CONSTANT_NAME_VS_LIGHT_COLOR ] = locationIndex;
						else if ( strcmp(szUniformName, "agk_PSLightPos") == 0 ) 		pShader->m_indexByKnownName[ AGK_SHADER_CONSTANT_NAME_PS_LIGHT_POS ] = locationIndex;
						else if ( strcmp(szUniformName, "agk_PSLightColor") == 0 ) 		pShader->m_indexByKnownName[ AGK_SHADER_CONSTANT_NAME_PS_LIGHT_COLOR ] = locationIndex;
																										
						else if ( strcmp(szUniformName, "uvBounds0") == 0 ) 			
						{
							pCurrLocation.m_pDefaultValue = &AGKShader::g_varDefaultUVBounds;
							pShader->m_indexByKnownName[ AGK_SHADER_CONSTANT_NAME_UV_BOUNDS_0 ] = locationIndex;
						}
						else if ( strcmp(szUniformName, "uvBounds1") == 0 )
						{
							pCurrLocation.m_pDefaultValue = &AGKShader::g_varDefaultUVBounds;
							pShader->m_indexByKnownName[ AGK_SHADER_CONSTANT_NAME_UV_BOUNDS_1 ] = locationIndex;
				
						}
						else if ( strcmp(szUniformName, "uvBounds2") == 0 ) 
						{
							pCurrLocation.m_pDefaultValue = &AGKShader::g_varDefaultUVBounds;
							pShader->m_indexByKnownName[ AGK_SHADER_CONSTANT_NAME_UV_BOUNDS_2 ] = locationIndex;
						}
						else if ( strcmp(szUniformName, "uvBounds3") == 0 ) 
						{
							pCurrLocation.m_pDefaultValue = &AGKShader::g_varDefaultUVBounds;
							pShader->m_indexByKnownName[ AGK_SHADER_CONSTANT_NAME_UV_BOUNDS_3 ] = locationIndex;				
						}
						else if ( strcmp(szUniformName, "uvBounds4") == 0 )
						{
							pCurrLocation.m_pDefaultValue = &AGKShader::g_varDefaultUVBounds;
							pShader->m_indexByKnownName[ AGK_SHADER_CONSTANT_NAME_UV_BOUNDS_4 ] = locationIndex;				
						}
						else if ( strcmp(szUniformName, "uvBounds5") == 0 )
						{
							pCurrLocation.m_pDefaultValue = &AGKShader::g_varDefaultUVBounds;
							pShader->m_indexByKnownName[ AGK_SHADER_CONSTANT_NAME_UV_BOUNDS_5 ] = locationIndex;				
						}
						else if ( strcmp(szUniformName, "uvBounds6") == 0 )
						{
							pCurrLocation.m_pDefaultValue = &AGKShader::g_varDefaultUVBounds;
							pShader->m_indexByKnownName[ AGK_SHADER_CONSTANT_NAME_UV_BOUNDS_6 ] = locationIndex;				
						}
						else if ( strcmp(szUniformName, "uvBounds7") == 0 )
						{
							pCurrLocation.m_pDefaultValue = &AGKShader::g_varDefaultUVBounds;
							pShader->m_indexByKnownName[ AGK_SHADER_CONSTANT_NAME_UV_BOUNDS_7 ] = locationIndex;				
						}
																							
						else if ( strcmp(szUniformName, "textureBounds0") == 0 )
						{
							pCurrLocation.m_pDefaultValue = &AGKShader::g_varDefaultUVBounds;
							pShader->m_indexByKnownName[ AGK_SHADER_CONSTANT_NAME_TEX_BOUNDS_0 ] = locationIndex;				
						}
						else if ( strcmp(szUniformName, "textureBounds1") == 0 ) 
						{
							pCurrLocation.m_pDefaultValue = &AGKShader::g_varDefaultUVBounds;
							pShader->m_indexByKnownName[ AGK_SHADER_CONSTANT_NAME_TEX_BOUNDS_1 ] = locationIndex;
						}
						else if ( strcmp(szUniformName, "textureBounds2") == 0 ) 
						{
							pCurrLocation.m_pDefaultValue = &AGKShader::g_varDefaultUVBounds;
							pShader->m_indexByKnownName[ AGK_SHADER_CONSTANT_NAME_TEX_BOUNDS_2 ] = locationIndex;
						}
						else if ( strcmp(szUniformName, "textureBounds3") == 0 ) 
						{
							pCurrLocation.m_pDefaultValue = &AGKShader::g_varDefaultUVBounds;
							pShader->m_indexByKnownName[ AGK_SHADER_CONSTANT_NAME_TEX_BOUNDS_3 ] = locationIndex;
						}
						else if ( strcmp(szUniformName, "textureBounds4") == 0 )
						{
							pCurrLocation.m_pDefaultValue = &AGKShader::g_varDefaultUVBounds;
							pShader->m_indexByKnownName[ AGK_SHADER_CONSTANT_NAME_TEX_BOUNDS_4 ] = locationIndex;
						}
						else if ( strcmp(szUniformName, "textureBounds5") == 0 ) 
						{
							pCurrLocation.m_pDefaultValue = &AGKShader::g_varDefaultUVBounds;
							pShader->m_indexByKnownName[ AGK_SHADER_CONSTANT_NAME_TEX_BOUNDS_5 ] = locationIndex;
						}
						else if ( strcmp(szUniformName, "textureBounds6") == 0 ) 
						{
							pCurrLocation.m_pDefaultValue = &AGKShader::g_varDefaultUVBounds;
							pShader->m_indexByKnownName[ AGK_SHADER_CONSTANT_NAME_TEX_BOUNDS_6 ] = locationIndex;
						}
						else if ( strcmp(szUniformName, "textureBounds7") == 0 ) 
						{
							pCurrLocation.m_pDefaultValue = &AGKShader::g_varDefaultUVBounds;
							pShader->m_indexByKnownName[ AGK_SHADER_CONSTANT_NAME_TEX_BOUNDS_7 ] = locationIndex;	
						}
						else if ( strcmp(szUniformName, "textureBounds8") == 0 ) 
						{
							pCurrLocation.m_pDefaultValue = &AGKShader::g_varDefaultUVBounds;
							pShader->m_indexByKnownName[ AGK_SHADER_CONSTANT_NAME_TEX_BOUNDS_8 ] = locationIndex;	
						}
						else if ( strcmp(szUniformName, "textureBounds9") == 0 ) 
						{
							pCurrLocation.m_pDefaultValue = &AGKShader::g_varDefaultUVBounds;
							pShader->m_indexByKnownName[ AGK_SHADER_CONSTANT_NAME_TEX_BOUNDS_9 ] = locationIndex;	
						}
						else if ( strcmp(szUniformName, "textureBounds10") == 0 ) 
						{
							pCurrLocation.m_pDefaultValue = &AGKShader::g_varDefaultUVBounds;
							pShader->m_indexByKnownName[ AGK_SHADER_CONSTANT_NAME_TEX_BOUNDS_10 ] = locationIndex;	
						}
						else if ( strcmp(szUniformName, "textureBounds11") == 0 ) 
						{
							pCurrLocation.m_pDefaultValue = &AGKShader::g_varDefaultUVBounds;
							pShader->m_indexByKnownName[ AGK_SHADER_CONSTANT_NAME_TEX_BOUNDS_11 ] = locationIndex;	
						}
						else if ( strcmp(szUniformName, "textureBounds12") == 0 ) 
						{
							pCurrLocation.m_pDefaultValue = &AGKShader::g_varDefaultUVBounds;
							pShader->m_indexByKnownName[ AGK_SHADER_CONSTANT_NAME_TEX_BOUNDS_12 ] = locationIndex;	
						}
						else if ( strcmp(szUniformName, "textureBounds13") == 0 ) 
						{
							pCurrLocation.m_pDefaultValue = &AGKShader::g_varDefaultUVBounds;
							pShader->m_indexByKnownName[ AGK_SHADER_CONSTANT_NAME_TEX_BOUNDS_13 ] = locationIndex;	
						}
						else if ( strcmp(szUniformName, "textureBounds14") == 0 ) 
						{
							pCurrLocation.m_pDefaultValue = &AGKShader::g_varDefaultUVBounds;
							pShader->m_indexByKnownName[ AGK_SHADER_CONSTANT_NAME_TEX_BOUNDS_14 ] = locationIndex;	
						}
						else if ( strcmp(szUniformName, "textureBounds15") == 0 ) 
						{
							pCurrLocation.m_pDefaultValue = &AGKShader::g_varDefaultUVBounds;
							pShader->m_indexByKnownName[ AGK_SHADER_CONSTANT_NAME_TEX_BOUNDS_15 ] = locationIndex;	
						}

						else
						{
							pAGKShader->m_constantBindingByName.AddItem( locationIndex+1, szUniformName ); // +1 to avoid adding zero which is a null element
						}
						
						bindingOffset += pCurrMember.padded_size;
						locationCount++;
					}

					// bindingAlignment may be quite large on Nvidia devices, e.g. 256 bytes
					uint32_t alignedOffset = bindingOffset / bindingAlignment;
					alignedOffset = alignedOffset * bindingAlignment;
					if ( alignedOffset != bindingOffset ) bindingOffset = alignedOffset + bindingAlignment;

					uniformCount++;
					break;
				}
				case SPV_REFLECT_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER:
				{
					pBindingLayout[ j ].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
					pCurrSet.m_pTextureBindings[ textureCount ] = pCurrBinding->binding;
					uint16_t textureIndex = (pCurrSet.m_iSetID << 12) | textureCount;

					const char *szUniformName = pCurrBinding->name;

					if ( strcmp(szUniformName, "texture0") == 0 )	    { pShader->m_indexByTexStage[  0 ] = textureIndex; }
					else if ( strcmp(szUniformName, "texture1") == 0 )  { pShader->m_indexByTexStage[  1 ] = textureIndex; }
					else if ( strcmp(szUniformName, "texture2") == 0 )  { pShader->m_indexByTexStage[  2 ] = textureIndex; }
					else if ( strcmp(szUniformName, "texture3") == 0 )  { pShader->m_indexByTexStage[  3 ] = textureIndex; }
					else if ( strcmp(szUniformName, "texture4") == 0 )  { pShader->m_indexByTexStage[  4 ] = textureIndex; }
					else if ( strcmp(szUniformName, "texture5") == 0 )  { pShader->m_indexByTexStage[  5 ] = textureIndex; }
					else if ( strcmp(szUniformName, "texture6") == 0 )  { pShader->m_indexByTexStage[  6 ] = textureIndex; }
					else if ( strcmp(szUniformName, "texture7") == 0 )  { pShader->m_indexByTexStage[  7 ] = textureIndex; }
					else if ( strcmp(szUniformName, "texture8") == 0 )  { pShader->m_indexByTexStage[  8 ] = textureIndex; }
					else if ( strcmp(szUniformName, "texture9") == 0 )  { pShader->m_indexByTexStage[  9 ] = textureIndex; }
					else if ( strcmp(szUniformName, "texture10") == 0 ) { pShader->m_indexByTexStage[ 10 ] = textureIndex; }
					else if ( strcmp(szUniformName, "texture11") == 0 ) { pShader->m_indexByTexStage[ 11 ] = textureIndex; }
					else if ( strcmp(szUniformName, "texture12") == 0 ) { pShader->m_indexByTexStage[ 12 ] = textureIndex; }
					else if ( strcmp(szUniformName, "texture13") == 0 ) { pShader->m_indexByTexStage[ 13 ] = textureIndex; }
					else if ( strcmp(szUniformName, "texture14") == 0 ) { pShader->m_indexByTexStage[ 14 ] = textureIndex; }
					else if ( strcmp(szUniformName, "texture15") == 0 ) { pShader->m_indexByTexStage[ 15 ] = textureIndex; }

					else if ( strcmp(szUniformName, "cubeMap0") == 0 )  { pShader->m_indexByTexStage[  0 ] = textureIndex; }
					else if ( strcmp(szUniformName, "cubeMap1") == 0 )  { pShader->m_indexByTexStage[  1 ] = textureIndex; }
					else if ( strcmp(szUniformName, "cubeMap2") == 0 )  { pShader->m_indexByTexStage[  2 ] = textureIndex; }
					else if ( strcmp(szUniformName, "cubeMap3") == 0 )  { pShader->m_indexByTexStage[  3 ] = textureIndex; }
					else if ( strcmp(szUniformName, "cubeMap4") == 0 )  { pShader->m_indexByTexStage[  4 ] = textureIndex; }
					else if ( strcmp(szUniformName, "cubeMap5") == 0 )  { pShader->m_indexByTexStage[  5 ] = textureIndex; }
					else if ( strcmp(szUniformName, "cubeMap6") == 0 )  { pShader->m_indexByTexStage[  6 ] = textureIndex; }
					else if ( strcmp(szUniformName, "cubeMap7") == 0 )  { pShader->m_indexByTexStage[  7 ] = textureIndex; }
					else if ( strcmp(szUniformName, "cubeMap8") == 0 )  { pShader->m_indexByTexStage[  8 ] = textureIndex; }
					else if ( strcmp(szUniformName, "cubeMap9") == 0 )  { pShader->m_indexByTexStage[  9 ] = textureIndex; }
					else if ( strcmp(szUniformName, "cubeMap10") == 0 ) { pShader->m_indexByTexStage[ 10 ] = textureIndex; }
					else if ( strcmp(szUniformName, "cubeMap11") == 0 ) { pShader->m_indexByTexStage[ 11 ] = textureIndex; }
					else if ( strcmp(szUniformName, "cubeMap12") == 0 ) { pShader->m_indexByTexStage[ 12 ] = textureIndex; }
					else if ( strcmp(szUniformName, "cubeMap13") == 0 ) { pShader->m_indexByTexStage[ 13 ] = textureIndex; }
					else if ( strcmp(szUniformName, "cubeMap14") == 0 ) { pShader->m_indexByTexStage[ 14 ] = textureIndex; }
					else if ( strcmp(szUniformName, "cubeMap15") == 0 ) { pShader->m_indexByTexStage[ 15 ] = textureIndex; }
			
					else if ( strcmp(szUniformName, "shadowMap") == 0 ) 
					{ 
						if ( pShader->m_indexByTexStage[ 7 ] != (uint16_t)-1 && m_iShaderErrorMode < 2 ) agk::Error( "Cannot use texture stage 7, it is overriden by shadow mapping" );
						pShader->m_indexByTexStage[ 7 ] = textureIndex; 
					}
					else if ( strcmp(szUniformName, "shadowMap2") == 0 ) 
					{ 
						if ( pShader->m_indexByTexStage[ 6 ] != (uint16_t)-1 && m_iShaderErrorMode < 2 ) agk::Error( "Cannot use texture stage 6, it is overriden by cascade shadow mapping" );
						pShader->m_indexByTexStage[ 6 ] = textureIndex; 
					}
					else if ( strcmp(szUniformName, "shadowMap3") == 0 ) 
					{ 
						if ( pShader->m_indexByTexStage[ 5 ] != (uint16_t)-1 && m_iShaderErrorMode < 2 ) agk::Error( "Cannot use texture stage 5, it is overriden by cascade shadow mapping" );
						pShader->m_indexByTexStage[ 5 ] = textureIndex; 
					}
					else if ( strcmp(szUniformName, "shadowMap4") == 0 ) 
					{ 
						if ( pShader->m_indexByTexStage[ 4 ] != (uint16_t)-1 && m_iShaderErrorMode < 2 ) agk::Error( "Cannot use texture stage 4, it is overriden by cascade shadow mapping" );
						pShader->m_indexByTexStage[ 4 ] = textureIndex; 
					}

					textureCount++;
				}
			}
		}

		pCurrSet.m_iBufferSize = bindingOffset;

		// create layout
		VkDescriptorSetLayoutCreateInfo layoutInfo = {};
		layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		layoutInfo.bindingCount = bindingCount;
		layoutInfo.pBindings = pBindingLayout;

		int result = vkCreateDescriptorSetLayout( m_vkDevice, &layoutInfo, nullptr, &pCurrSet.m_layout );
		if ( VK_SUCCESS != result ) AppError( "Failed to create descriptor layout: %d", result );
	
		delete [] pBindingLayout;
		delete [] pBindings;
	}

	// create pipeline layout
	VkDescriptorSetLayout *pDescriptorSetLayouts = 0;
	if ( pShader->m_iNumDescriptorSets > 0 ) 
	{
		pDescriptorSetLayouts = new VkDescriptorSetLayout[ pShader->m_iNumDescriptorSets ];
		for( uint32_t i = 0; i < pShader->m_iNumDescriptorSets; i++ )
		{
			pDescriptorSetLayouts[ i ] = pShader->m_pDescriptorSetStatic[ i ].m_layout;
		}
	}

	VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount = pShader->m_iNumDescriptorSets;
	pipelineLayoutInfo.pSetLayouts = pDescriptorSetLayouts;
	pipelineLayoutInfo.pushConstantRangeCount = 0;
	pipelineLayoutInfo.pPushConstantRanges = nullptr;

	VkResult res = vkCreatePipelineLayout( m_vkDevice, &pipelineLayoutInfo, nullptr, &pShader->m_vkPipelineLayout );
	if ( res != VK_SUCCESS) 
	{
		AppError( "Failed to create pipeline layout: %d", res );
		pAGKShader->SetValid( true );
		return;
	}

	if ( pDescriptorSetLayouts ) delete [] pDescriptorSetLayouts;

	pShader->m_iTotalTextures = iTotalTextures;
	if ( iTotalUniforms == 0 && iTotalTextures == 0 ) pShader->m_iFlags |= AGK_VK_SHADER_NO_CONSTANTS;
}

void VulkanRenderer::CleanUpShaders()
{
	VulkanShaderConstants *pShaderConstants = VulkanShaderConstants::g_pAllShaderConstants;
	while( pShaderConstants )
	{
		VulkanShaderConstants *pNext = pShaderConstants->m_pNextConstantObject;

		pShaderConstants->m_pShader->SetHasConstants();

		int anyActive = 0;
		for( uint32_t d = 0; d < pShaderConstants->m_iNumDescriptorSets; d++ )
		{
			VulkanDescriptorSet &currSet = pShaderConstants->m_pDescriptorSets[ d ];
			if ( currSet.m_pActiveDescriptorData.NumItems() > 0 ) anyActive = 1;
			for( int i = 0; i < (int)currSet.m_pActiveDescriptorData.NumItems(); i++ )
			{
				VulkanDescriptorSetData *pData = currSet.m_pActiveDescriptorData[ i ];
				uint32_t newFlags = pData->m_iFlags & ~(AGK_VK_SHADER_DATA_AWAITING_USE | AGK_VK_SHADER_DATA_IN_USE);
				if ( pData->m_iFlags & AGK_VK_SHADER_DATA_AWAITING_USE )
				{
					pData->m_iFlags = newFlags | AGK_VK_SHADER_DATA_IN_USE;
				}
				else if ( pData->m_iFlags & AGK_VK_SHADER_DATA_IN_USE ) 
				{
					currSet.m_pActiveDescriptorData.RemoveIndex( i );
					i--;
					currSet.m_pInactiveDescriptorData.PushItem( pData );
				}
			}
		}

		if ( pShaderConstants->ShouldDelete() && !anyActive )
		{
			ForceDeleteShaderConstants( pShaderConstants );
		}

		pShaderConstants = pNext;
	}

	VulkanShader *pShader = VulkanShader::g_pAllShaderObjects;
	while( pShader )
	{
		VulkanShader *pNext = pShader->m_pNextShader;
		
		if ( pShader->ShouldDelete() && !pShader->HasConstants() )
		{
			ForceDeleteShader( pShader );
		}
		else
		{
			pShader->ClearHasConstants();
		}
		
		pShader = pNext;
	}
}

void VulkanRenderer::ForceDeleteShaderConstants( VulkanShaderConstants *pConstants )
{
	for( uint32_t d = 0; d < pConstants->m_iNumDescriptorSets; d++ )
	{
		VulkanDescriptorSet &currSet = pConstants->m_pDescriptorSets[ d ];
		for( int i = 0; i < (int)currSet.m_pActiveDescriptorData.NumItems(); i++ )
		{
			currSet.m_pActiveDescriptorData[ i ]->Reset();
			pConstants->m_pShader->m_pDescriptorSetStatic[ d ].m_pInactiveDescriptorData.PushItem( currSet.m_pActiveDescriptorData[ i ] );
		}
		for( int i = 0; i < (int)currSet.m_pInactiveDescriptorData.NumItems(); i++ )
		{
			currSet.m_pInactiveDescriptorData[ i ]->Reset();
			pConstants->m_pShader->m_pDescriptorSetStatic[ d ].m_pInactiveDescriptorData.PushItem( currSet.m_pInactiveDescriptorData[ i ] );
		}
	}

	delete pConstants;
}

void VulkanRenderer::ForceDeleteShader( VulkanShader *pShader )
{
	for( uint32_t d = 0; d < pShader->m_iNumDescriptorSets; d++ )
	{
		VulkanDescriptorSetStatic &currSet = pShader->m_pDescriptorSetStatic[ d ];
			
		for( uint32_t i = 0; i < currSet.m_pInactiveDescriptorData.NumItems(); i++ )
		{
			VulkanDescriptorSetData *pData = currSet.m_pInactiveDescriptorData[ i ];
			if ( pData->m_pMappedData ) vmaUnmapMemory( m_vkAllocator, pData->m_vkBufferMemory );
			if ( pData->m_vkUniformBuffer ) vmaDestroyBuffer( m_vkAllocator, pData->m_vkUniformBuffer, pData->m_vkBufferMemory );
			delete pData;
		}

		for( uint32_t i = 0; i < currSet.m_pDescriptorPools.NumItems(); i++ )
		{
			vkDestroyDescriptorPool( m_vkDevice, currSet.m_pDescriptorPools[ i ], nullptr );
		}

		vkDestroyDescriptorSetLayout( m_vkDevice, currSet.m_layout, nullptr );
	}

	vkDestroyPipelineLayout( m_vkDevice, pShader->m_vkPipelineLayout, nullptr );
	vkDestroyShaderModule( m_vkDevice, pShader->m_vkVertShader, nullptr );
	vkDestroyShaderModule( m_vkDevice, pShader->m_vkFragShader, nullptr );

	delete pShader;
}

VulkanShaderConstants* VulkanShaderConstants::g_pAllShaderConstants = 0;

void VulkanShaderConstants::RemoveConstantObject() 
{
	if ( !m_pNextConstantObject && !m_pPrevConstantObject && g_pAllShaderConstants != this ) return;

	if ( m_pPrevConstantObject ) m_pPrevConstantObject->m_pNextConstantObject = m_pNextConstantObject;
	else g_pAllShaderConstants = m_pNextConstantObject;
	if ( m_pNextConstantObject ) m_pNextConstantObject->m_pPrevConstantObject = m_pPrevConstantObject;
	m_pNextConstantObject = 0;
	m_pPrevConstantObject = 0;
}

void VulkanShaderConstants::AddConstantObject()
{
	if ( m_pNextConstantObject || m_pPrevConstantObject || g_pAllShaderConstants == this ) return;

	if ( g_pAllShaderConstants ) g_pAllShaderConstants->m_pPrevConstantObject = this;
	m_pPrevConstantObject = 0;
	m_pNextConstantObject = g_pAllShaderConstants;
	g_pAllShaderConstants = this;
}

VulkanShaderConstants* VulkanRenderer::CreateShaderConstants( AGKShader *pAGKShader )
{
	if ( !pAGKShader || !pAGKShader->m_pInternalResources ) return 0;
	VulkanShader *pVulkanShader = (VulkanShader*) pAGKShader->m_pInternalResources;

	VulkanShaderConstants *pConstants = new VulkanShaderConstants();
	pConstants->m_pShader = pVulkanShader;
	pConstants->m_iShaderRef = pAGKShader->GetCreated();
	pConstants->m_iNumDescriptorSets = pVulkanShader->m_iNumDescriptorSets;
	pConstants->m_pDescriptorSets = new VulkanDescriptorSet[ pConstants->m_iNumDescriptorSets ];

	return pConstants;
}

VulkanDescriptorSetData* VulkanRenderer::CreateDescriptorData( VulkanShader *pShader, uint32_t setIndex )
{
	if ( setIndex >= pShader->m_iNumDescriptorSets ) return 0;

	VulkanDescriptorSetStatic &currSet = pShader->m_pDescriptorSetStatic[ setIndex ];

	// check if current pool has free space
	if ( currSet.m_iCurrPoolNumSets >= AGK_SHADER_DESCRIPTORS_PER_POOL || currSet.m_pDescriptorPools.NumItems() == 0 )
	{
		// create new pool
		int numSizes = 0;
		VkDescriptorPoolSize poolSize[2];

		if ( currSet.m_iNumUniformBindings > 0 )
		{
			poolSize[numSizes] = {};
			poolSize[numSizes].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			poolSize[numSizes].descriptorCount = currSet.m_iNumUniformBindings * AGK_SHADER_DESCRIPTORS_PER_POOL;
			numSizes++;
		}

		if ( currSet.m_iNumTextures > 0 )
		{
			poolSize[numSizes] = {};
			poolSize[numSizes].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			poolSize[numSizes].descriptorCount = currSet.m_iNumTextures * AGK_SHADER_DESCRIPTORS_PER_POOL;
			numSizes++;
		}

		VkDescriptorPoolCreateInfo poolInfo = {};
		poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		poolInfo.poolSizeCount = numSizes;
		poolInfo.pPoolSizes = poolSize;
		poolInfo.maxSets = AGK_SHADER_DESCRIPTORS_PER_POOL;
		poolInfo.flags = 0;

		VkDescriptorPool newPool;
		VkResult res = vkCreateDescriptorPool( m_vkDevice, &poolInfo, nullptr, &newPool );
		if ( VK_SUCCESS != res ) { AppError( "Failed to create descriptor pool: %d", res ); return 0; }

		currSet.m_pDescriptorPools.AddItem( newPool );
		currSet.m_iCurrPoolNumSets = 0;
	}

	// create new descriptor data
	VulkanDescriptorSetData *pData = new VulkanDescriptorSetData( &currSet );
	
	// create uniform buffer
	VkBufferCreateInfo bufferInfo = {};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = currSet.m_iBufferSize;
	bufferInfo.usage = AGK_BUFFER_UNIFORM;
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	VmaAllocationCreateInfo allocInfo = {};
	allocInfo.requiredFlags = AGK_MEM_STAGING;

	VmaAllocationInfo allocationInfo;
	VkResult res = vmaCreateBuffer( m_vkAllocator, &bufferInfo, &allocInfo, &pData->m_vkUniformBuffer, &pData->m_vkBufferMemory, &allocationInfo );
	if ( VK_SUCCESS != res ) { AppError( "Failed to create uniform buffer: %d", res ); return 0; }
    
    if ( allocationInfo.offset % m_vkGPUProperties.limits.minUniformBufferOffsetAlignment != 0 )
    {
        AppError( "Uniform offset %d is not aligned to %d bytes", allocationInfo.offset, m_vkGPUProperties.limits.minUniformBufferOffsetAlignment );
        return 0;
    }

	// map it
	pData->m_iFlags |= AGK_VK_SHADER_DATA_CAN_MAP;
	res = vmaMapMemory( m_vkAllocator, pData->m_vkBufferMemory, &pData->m_pMappedData );
	if ( VK_SUCCESS != res ) { AppError( "Failed to map uniform buffer: %d", res ); return 0; }
	memset( pData->m_pMappedData, 0, currSet.m_iBufferSize );

	// create descriptor set
	VkDescriptorSetAllocateInfo allocSetInfo = {};
	allocSetInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocSetInfo.descriptorPool = currSet.m_pDescriptorPools[ currSet.m_pDescriptorPools.NumItems()-1 ];
	allocSetInfo.descriptorSetCount = 1;
	allocSetInfo.pSetLayouts = &currSet.m_layout;

	res = vkAllocateDescriptorSets( m_vkDevice, &allocSetInfo, &pData->m_vkDescriptorSet );
	if ( VK_SUCCESS != res ) { AppError( "Failed to create descriptor set: %d", res ); return 0; }

	currSet.m_iCurrPoolNumSets++;

	// update descriptor set details
	if ( currSet.m_iNumUniformBindings > 0 )
	{
		VkDescriptorBufferInfo *bindings = new VkDescriptorBufferInfo[ currSet.m_iNumUniformBindings ];
		VkWriteDescriptorSet *updates = new VkWriteDescriptorSet[ currSet.m_iNumUniformBindings ];
		for( uint32_t i = 0; i < currSet.m_iNumUniformBindings; i++ )
		{
			bindings[ i ].range = currSet.m_pUniformBindings[ i ].m_iSize;
			bindings[ i ].offset = currSet.m_pUniformBindings[ i ].m_iOffset;
			bindings[ i ].buffer = pData->m_vkUniformBuffer;

			updates[ i ] = {};
			updates[ i ].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			updates[ i ].dstSet = pData->m_vkDescriptorSet;
			updates[ i ].dstBinding = currSet.m_pUniformBindings[ i ].m_iBindingID;
			updates[ i ].dstArrayElement = 0;
			updates[ i ].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			updates[ i ].descriptorCount = 1;
			updates[ i ].pBufferInfo = &bindings[ i ];
		}

		vkUpdateDescriptorSets( m_vkDevice, currSet.m_iNumUniformBindings, updates, 0, nullptr );

		delete [] updates;
		delete [] bindings;
	}

	if ( currSet.m_iNumTextures > 0 )
	{
		VkWriteDescriptorSet *updates = new VkWriteDescriptorSet[ currSet.m_iNumTextures ];
		VkDescriptorImageInfo *images = new VkDescriptorImageInfo[ currSet.m_iNumTextures ];

		for( uint32_t i = 0; i < currSet.m_iNumTextures; i++ )
		{
			images[ i ].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			images[ i ].imageView = m_pNullImage2D->GetCurrentView();
			images[ i ].sampler = InternalGetImageSampler( m_pNullImage2D );

			updates[ i ] = {};
			updates[ i ].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			updates[ i ].dstSet = pData->m_vkDescriptorSet;
			updates[ i ].dstBinding = currSet.m_pTextureBindings[ i ];
			updates[ i ].dstArrayElement = 0;
			updates[ i ].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			updates[ i ].descriptorCount = 1;
			updates[ i ].pImageInfo = &images[ i ];
		}

		vkUpdateDescriptorSets( m_vkDevice, currSet.m_iNumTextures, updates, 0, nullptr );

		delete [] images;
		delete [] updates;
	}

	return pData;
}

void VulkanRenderer::SetShaderConstant( AGKShader *pAGKShader, uint32_t index, AGKShaderConstantValue *pConstant )
{
	if ( !pAGKShader ) return;
	VulkanShader *pVulkanShader = (VulkanShader*) (pAGKShader->m_pInternalResources);
	if ( !pVulkanShader ) return;

	uint32_t set = (index >> 12) & 0xF;
	uint32_t locationIndex = index & 0xFFF;

	if ( set >= pVulkanShader->m_iNumDescriptorSets ) { AppError("Invalid set index: %d", set); return; }
	VulkanDescriptorSetStatic &pSet = pVulkanShader->m_pDescriptorSetStatic[ set ];

	if ( locationIndex >= pSet.m_iNumLocations ) { AppError("Invalid location index: %d for set: %d", locationIndex, set); return; }
	VulkanDescriptorSetLocationStatic &pLocation = pSet.m_pLocations[ locationIndex ];
	
	pLocation.m_pDefaultValue = pConstant;
}

int VulkanRenderer::BindShaderConstants( void *pCommandBuffer, AGKShader *pShader, 
										 AGKUnorderedArray<AGKShaderConstantValue*> *pConstants, 
										 AGKUnorderedArray<AGKShaderConstantValue*> *pAdditionalConstants, 
										 AGKDynamicArray<cImage*> *pTextures, 
										 void **inoutConstants )
{
	if ( !pShader || !pShader->m_pInternalResources ) return APP_ERROR_GENERAL;
	VulkanShader *pVulkanShader = (VulkanShader*) pShader->m_pInternalResources;
	if ( pVulkanShader->NoConstants() ) return APP_SUCCESS;

	VulkanShaderConstants *pVulkanConstants = 0;
	if ( inoutConstants ) 
	{
		pVulkanConstants = (VulkanShaderConstants*) *inoutConstants;

		if ( pVulkanConstants && pVulkanConstants->m_iShaderRef != pShader->GetCreated() )
		{
			DeleteShaderConstants( pVulkanConstants );
			pVulkanConstants = 0;
		}
	}

	// if inoutConstants is null then constants bound in this function must be immediately marked for deletion or they will leak memory
	bool deleteConstants = false;
	if ( !pVulkanConstants ) pVulkanConstants = CreateShaderConstants( pShader );
	if ( inoutConstants ) *inoutConstants = pVulkanConstants;
	else deleteConstants = true;

	// do some setup
	int hasConstants = 0;
	for( uint32_t i = 0; i < pVulkanConstants->m_iNumDescriptorSets; i++ )
	{
		if ( pVulkanShader->m_pDescriptorSetStatic[ i ].m_iNumTextures == 0 && pVulkanShader->m_pDescriptorSetStatic[ i ].m_iNumLocations == 0 ) continue;
		hasConstants = 1;

		VulkanDescriptorSet &pCurrSet = pVulkanConstants->m_pDescriptorSets[ i ];

		// get a free descriptor data object, first check local array, then shader inactive array, then create one
		pCurrSet.pCurrData = pCurrSet.m_pInactiveDescriptorData.PopItem();
		if ( !pCurrSet.pCurrData )
		{
			pCurrSet.pCurrData = pVulkanConstants->m_pShader->m_pDescriptorSetStatic[ i ].m_pInactiveDescriptorData.PopItem();
			if ( !pCurrSet.pCurrData ) pCurrSet.pCurrData = CreateDescriptorData( pVulkanShader, i );
			if ( !pCurrSet.pCurrData )
			{
				AppError( "Failed to create new desciptor set data" );
				return APP_ERROR_GENERAL;
			}
		}
		pCurrSet.m_pActiveDescriptorData.AddItem( pCurrSet.pCurrData );
		pCurrSet.pCurrData->DidBind();

		// reset 
		for( uint32_t j = 0; j < pCurrSet.pCurrData->m_iNumLocations; j++ )
		{
			pCurrSet.pCurrData->m_pLocations[ j ].UnSet();
			if ( pShader->m_iFlags & AGK_SHADER_RELOAD_UNIFORMS ) pCurrSet.pCurrData->m_pLocations[ j ].Reload();
		}
	}

	// texture updates
	if ( pVulkanShader->m_iTotalTextures > 0 )
	{
		// temporary buffers to store any texture updates across all sets
		VkWriteDescriptorSet *m_pTextureUpdates = new VkWriteDescriptorSet[ pVulkanShader->m_iTotalTextures ]; 
		VkDescriptorImageInfo *m_pSetTextureUpdates = new VkDescriptorImageInfo[ pVulkanShader->m_iTotalTextures ];
		uint32_t numUpdates = 0;

		for( int i = 0; i < AGK_MAX_TEXTURE_STAGES; i++ )
		{
			if ( pVulkanShader->m_indexByTexStage[ i ] == (uint16_t)-1 ) continue;

			uint16_t set = pVulkanShader->m_indexByTexStage[ i ] >> 12;
			uint16_t index = pVulkanShader->m_indexByTexStage[ i ] & 0xFFF;

			VulkanImage *pVulkanImage = m_pNullImage2D;
			cImage *pImage = 0;
			if ( pTextures ) pImage = pTextures->GetItem( i );
			if ( pImage ) 
			{
				pVulkanImage = (VulkanImage*) pImage->GetInternalResource();
				if ( !pVulkanImage || pVulkanImage->ShouldDelete() || pVulkanImage->IsUndefined() ) pVulkanImage = m_pNullImage2D;
				if ( pVulkanImage->GetCurrImageData()->m_iCurrentMode != AGK_VK_IMAGE_MODE_READ )
				{
					if ( pVulkanImage->GetCurrImageData()->m_iCurrentMode == AGK_VK_IMAGE_MODE_DRAW ) AppError( "Image %d cannot be used as a texture whilst it is currently being used with SetRenderToImage", pImage->m_iID );
					else AppError( "Failed to use image %d as a texture, it is not ready for reading", pImage->m_iID );
					return APP_ERROR_GENERAL;
				}
			}

			if ( pVulkanConstants->m_pDescriptorSets[ set ].pCurrData->m_pCurrentTextures[ index ] != pVulkanImage->GetCurrImageData()->m_iCreatedID )
			{
				// update texture
				pVulkanConstants->m_pDescriptorSets[ set ].pCurrData->m_pCurrentTextures[ index ] = pVulkanImage->GetCurrImageData()->m_iCreatedID;

				m_pSetTextureUpdates[ numUpdates ].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
				m_pSetTextureUpdates[ numUpdates ].imageView = pVulkanImage->GetCurrentView();
				m_pSetTextureUpdates[ numUpdates ].sampler = InternalGetImageSampler( pVulkanImage );

				m_pTextureUpdates[ numUpdates ] = {};
				m_pTextureUpdates[ numUpdates ].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
				m_pTextureUpdates[ numUpdates ].dstSet = pVulkanConstants->m_pDescriptorSets[ set ].pCurrData->m_vkDescriptorSet;
				m_pTextureUpdates[ numUpdates ].dstBinding = pVulkanShader->m_pDescriptorSetStatic[ set ].m_pTextureBindings[ index ];
				m_pTextureUpdates[ numUpdates ].dstArrayElement = 0;
				m_pTextureUpdates[ numUpdates ].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
				m_pTextureUpdates[ numUpdates ].descriptorCount = 1;
				m_pTextureUpdates[ numUpdates ].pImageInfo = &m_pSetTextureUpdates[ numUpdates ];

				numUpdates++;
			}

			pVulkanImage->WillBind();

			// check for texture bounds
			uint16_t boundsIndex = pVulkanShader->m_indexByKnownName[ AGK_SHADER_CONSTANT_NAME_TEX_BOUNDS_0 + i ];
			if ( boundsIndex != (uint16_t) -1 && pImage )
			{
				uint16_t boundsSet = boundsIndex >> 12;
				boundsIndex &= 0xFFF;

				VulkanDescriptorSetLocation &pCurrLocation = pVulkanConstants->m_pDescriptorSets[ boundsSet ].pCurrData->m_pLocations[ boundsIndex ];
				pCurrLocation.Set();
				if ( pCurrLocation.m_pCurrValue != pImage->GetShaderVarTexBounds() ) 
				{
					pCurrLocation.m_pCurrValue = pImage->GetShaderVarTexBounds();
					pCurrLocation.Reload();
				}
			}
		}

		if ( numUpdates > 0 ) vkUpdateDescriptorSets( m_vkDevice, numUpdates, m_pTextureUpdates, 0, nullptr );

		delete [] m_pSetTextureUpdates;
		delete [] m_pTextureUpdates;
	}

	// uniform updates

	// first set of constants
	AGKShaderConstantValue *pWorldConstant = 0;
	if ( pConstants )
	{
		for( uint32_t i = 0; i < pConstants->NumItems(); i++ )
		{
			AGKShaderConstantValue *pConstant = pConstants->GetItem( i );
			uint16_t index = (uint16_t) -1;
			switch( pConstant->GetNameType() )
			{
				case AGK_SHADER_CONSTANT_NAME_TYPE_KNOWN:
				{
					index = pVulkanShader->m_indexByKnownName[ pConstant->GetKnownName() ];
					if ( pConstant->GetKnownName() == AGK_SHADER_CONSTANT_NAME_WORLD_MAT && !pWorldConstant ) pWorldConstant = pConstant;
					break;
				}
				case AGK_SHADER_CONSTANT_NAME_TYPE_USER:
				{
					index = (uint16_t) pShader->GetBindingForName( pConstant->GetName() );
					break;
				}
				case AGK_SHADER_CONSTANT_NAME_TYPE_BINDING:
				{
					index = (uint16_t) pConstant->GetBindingName();
					break;
				}
			}

			if ( index == (uint16_t) -1 ) continue;

			uint16_t set = index >> 12;
			index &= 0xFFF;
			VulkanDescriptorSetData *pData = pVulkanConstants->m_pDescriptorSets[ set ].pCurrData;

			if ( pData->m_pLocations[ index ].IsSet() ) continue;
			pData->m_pLocations[ index ].Set();
			if ( pData->m_pLocations[ index ].m_pCurrValue != pConstant ) 
			{
				pData->m_pLocations[ index ].m_pCurrValue = pConstant;
				pData->m_pLocations[ index ].Reload();
			}
			// check type?
		}
	}

	// second set of constants
	if ( pAdditionalConstants )
	{
		for( uint32_t i = 0; i < pAdditionalConstants->NumItems(); i++ )
		{
			AGKShaderConstantValue *pConstant = pAdditionalConstants->GetItem( i );
			uint16_t index = (uint16_t) -1;
			switch( pConstant->GetNameType() )
			{
				case AGK_SHADER_CONSTANT_NAME_TYPE_KNOWN:
				{
					index = pVulkanShader->m_indexByKnownName[ pConstant->GetKnownName() ];
					if ( pConstant->GetKnownName() == AGK_SHADER_CONSTANT_NAME_WORLD_MAT && !pWorldConstant ) pWorldConstant = pConstant;
					break;
				}
				case AGK_SHADER_CONSTANT_NAME_TYPE_USER:
				{
					index = (uint16_t) pShader->GetBindingForName( pConstant->GetName() );
					break;
				}
				case AGK_SHADER_CONSTANT_NAME_TYPE_BINDING:
				{
					index = (uint16_t) pConstant->GetBindingName();
					break;
				}
			}

			if ( index == (uint16_t) -1 ) continue;

			uint16_t set = index >> 12;
			index &= 0xFFF;
			VulkanDescriptorSetData *pData = pVulkanConstants->m_pDescriptorSets[ set ].pCurrData;

			if ( pData->m_pLocations[ index ].IsSet() ) continue;
			pData->m_pLocations[ index ].Set();
			if ( pData->m_pLocations[ index ].m_pCurrValue != pConstant ) 
			{
				pData->m_pLocations[ index ].m_pCurrValue = pConstant;
				pData->m_pLocations[ index ].Reload();
			}
			// check type?
		}
	}

	// camera constants
	cCamera *pCamera = agk::GetCurrentCamera();

	// camera view matrix
	uint16_t index = pVulkanShader->m_indexByKnownName[ AGK_SHADER_CONSTANT_NAME_VIEW ];
	if ( index != (uint16_t) -1 )
	{
		uint16_t set = index >> 12;
		index &= 0xFFF;
		VulkanDescriptorSetData *pData = pVulkanConstants->m_pDescriptorSets[ set ].pCurrData;
		if ( !pData->m_pLocations[ index ].IsSet() ) 
		{
			pData->m_pLocations[ index ].Set();
			AGKShaderConstantValue *pConstantView = pCamera->GetShaderView();
			if ( pData->m_pLocations[ index ].m_pCurrValue != pConstantView ) 
			{
				pData->m_pLocations[ index ].m_pCurrValue = pConstantView;
				pData->m_pLocations[ index ].Reload();
			}
		}
	}

	// camera proj matrix
	index = pVulkanShader->m_indexByKnownName[ AGK_SHADER_CONSTANT_NAME_PROJ ];
	if ( index != (uint16_t) -1 )
	{
		uint16_t set = index >> 12;
		index &= 0xFFF;
		VulkanDescriptorSetData *pData = pVulkanConstants->m_pDescriptorSets[ set ].pCurrData;
		if ( !pData->m_pLocations[ index ].IsSet() ) 
		{
			pData->m_pLocations[ index ].Set();
			AGKShaderConstantValue *pConstantProj = pCamera->GetShaderProj();
			if ( pData->m_pLocations[ index ].m_pCurrValue != pConstantProj ) 
			{
				pData->m_pLocations[ index ].m_pCurrValue = pConstantProj;
				pData->m_pLocations[ index ].Reload();
			}
		}
	}

	// camera viewProj matrix
	index = pVulkanShader->m_indexByKnownName[ AGK_SHADER_CONSTANT_NAME_VIEW_PROJ ];
	if ( index != (uint16_t) -1 )
	{
		uint16_t set = index >> 12;
		index &= 0xFFF;
		VulkanDescriptorSetData *pData = pVulkanConstants->m_pDescriptorSets[ set ].pCurrData;
		if ( !pData->m_pLocations[ index ].IsSet() ) 
		{
			pData->m_pLocations[ index ].Set();
			AGKShaderConstantValue *pConstantViewProj = pCamera->GetShaderViewProj();
			if ( pData->m_pLocations[ index ].m_pCurrValue != pConstantViewProj ) 
			{
				pData->m_pLocations[ index ].m_pCurrValue = pConstantViewProj;
				pData->m_pLocations[ index ].Reload();
			}
		}
	}

	// camera pos
	index = pVulkanShader->m_indexByKnownName[ AGK_SHADER_CONSTANT_NAME_CAMERA_POS ];
	if ( index != (uint16_t) -1 )
	{
		uint16_t set = index >> 12;
		index &= 0xFFF;
		VulkanDescriptorSetData *pData = pVulkanConstants->m_pDescriptorSets[ set ].pCurrData;
		if ( !pData->m_pLocations[ index ].IsSet() ) 
		{
			pData->m_pLocations[ index ].Set();
			AGKShaderConstantValue *pConstantCamPos = pCamera->GetShaderPos();
			if ( pData->m_pLocations[ index ].m_pCurrValue != pConstantCamPos ) 
			{
				pData->m_pLocations[ index ].m_pCurrValue = pConstantCamPos;
				pData->m_pLocations[ index ].Reload();
			}
		}
	}
	
	// calculate WVP matrix
	index = pVulkanShader->m_indexByKnownName[ AGK_SHADER_CONSTANT_NAME_WORLD_VIEW_PROJ ];
	if ( index != (uint16_t) -1 && pWorldConstant ) 
	{
		uint16_t set = index >> 12;
		index &= 0xFFF;
		VulkanDescriptorSetData *pData = pVulkanConstants->m_pDescriptorSets[ set ].pCurrData;
		if ( !pData->m_pLocations[ index ].IsSet() ) 
		{
			pData->m_pLocations[ index ].Set();
			
			AGKShaderConstantValue *pConstantViewProj = pCamera->GetShaderViewProj();
			AGKMatrix4 matWVP, matViewProj;
			matWVP.Set( pWorldConstant->GetValues() );
			matViewProj.Set( pConstantViewProj->GetValues() );
			matWVP.Mult( matViewProj );

			AGKShaderConstantValue *pCurrValue = pData->m_pLocations[ index ].GetLocalValue();
			pCurrValue->SetMatrix4( matWVP.GetFloatPtr() );
		}
	}

	// calculate WorldShadow matrix
	index = pVulkanShader->m_indexByKnownName[ AGK_SHADER_CONSTANT_NAME_SHADOW_WORLD_PROJ ];
	if ( index != (uint16_t) -1 && pWorldConstant ) 
	{
		uint16_t set = index >> 12;
		index &= 0xFFF;
		VulkanDescriptorSetData *pData = pVulkanConstants->m_pDescriptorSets[ set ].pCurrData;
		if ( !pData->m_pLocations[ index ].IsSet() ) 
		{
			pData->m_pLocations[ index ].Set();

			AGKMatrix4 matWS, matShadowProj;
			matWS.Set( pWorldConstant->GetValues() );
			matShadowProj.Set( AGKShader::g_varShadowProj.GetValues() );
			matWS.Mult( matShadowProj );

			AGKShaderConstantValue *pCurrValue = pData->m_pLocations[ index ].GetLocalValue();
			pCurrValue->SetMatrix4( matWS.GetFloatPtr() );
		}
	}

	// set locations
	for( uint32_t s = 0; s < pVulkanConstants->m_iNumDescriptorSets; s++ )
	{
		VulkanDescriptorSetStatic &pDataStatic = pVulkanShader->m_pDescriptorSetStatic[ s ];
		VulkanDescriptorSetData *pData = pVulkanConstants->m_pDescriptorSets[ s ].pCurrData;
		float *pBufferF = (float*)pData->m_pMappedData;

		for( uint32_t i = 0; i < pData->m_iNumLocations; i++ )
		{	
			VulkanDescriptorSetLocationStatic &pCurrLocationStatic = pDataStatic.m_pLocations[ i ];
			VulkanDescriptorSetLocation &pCurrLocation = pData->m_pLocations[ i ];
			
			if ( !pCurrLocation.IsSet() )
			{
				if ( pCurrLocation.m_pCurrValue != pCurrLocationStatic.m_pDefaultValue )
				{
					pCurrLocation.m_pCurrValue = pCurrLocationStatic.m_pDefaultValue;
					pCurrLocation.Reload();
				}
			}

			if ( !pCurrLocation.m_pCurrValue ) continue;

			if ( pCurrLocation.ShouldReload()
			  || pCurrLocation.m_iChangeVersion != pCurrLocation.m_pCurrValue->GetChangeVersion() )
			{
				pCurrLocation.m_iChangeVersion = pCurrLocation.m_pCurrValue->GetChangeVersion();
				pCurrLocation.Reloaded();

				uint32_t floatOffset = pCurrLocationStatic.m_iOffset / 4;
				uint32_t elements = 1;
				if ( pCurrLocation.m_pCurrValue->IsArray() )
				{
					AGKShaderConstantArray *pConstantArray = (AGKShaderConstantArray*) pCurrLocation.m_pCurrValue;
					elements = pConstantArray->m_iNumElements;
				}
				if ( elements > pCurrLocationStatic.m_iArraySize ) elements = pCurrLocationStatic.m_iArraySize;

				switch( pCurrLocationStatic.m_iVarType )
				{
					case AGK_SHADER_CONSTANT_TYPE_FLOAT:
					{
						if ( elements == 1 ) 
						{
							pBufferF[ floatOffset ] = pCurrLocation.m_pCurrValue->GetValues()[ 0 ];
						}
						else if ( pCurrLocationStatic.m_iArrayStride == sizeof(float) )
						{
							memcpy( pBufferF + floatOffset, pCurrLocation.m_pCurrValue->GetValues(), elements*sizeof(float) );
						}
						else
						{
							uint32_t floatStride = pCurrLocationStatic.m_iArrayStride / 4;
							float *pSrc = pCurrLocation.m_pCurrValue->GetValues();
							for( uint32_t j = 0; j < elements; j++ )
							{
								pBufferF[ floatOffset ] = pSrc[ j ];
								floatOffset += floatStride;
							}
						}
						break;
					}
					case AGK_SHADER_CONSTANT_TYPE_VEC2:
					{
						if ( elements == 1 ) 
						{
							pBufferF[ floatOffset ] = pCurrLocation.m_pCurrValue->GetValues()[ 0 ];
							pBufferF[ floatOffset + 1 ] = pCurrLocation.m_pCurrValue->GetValues()[ 1 ];
						}
						else if ( pCurrLocationStatic.m_iArrayStride == sizeof(float)*2 )
						{
							memcpy( pBufferF + floatOffset, pCurrLocation.m_pCurrValue->GetValues(), elements*sizeof(float)*2 );
						}
						else
						{
							uint32_t floatStride = pCurrLocationStatic.m_iArrayStride / 4;
							float *pSrc = pCurrLocation.m_pCurrValue->GetValues();
							for( uint32_t j = 0; j < elements; j++ )
							{
								pBufferF[ floatOffset ] = pSrc[ j*2 ];
								pBufferF[ floatOffset + 1 ] = pSrc[ j*2 + 1 ];
								floatOffset += floatStride;
							}
						}
						break;
					}
					case AGK_SHADER_CONSTANT_TYPE_VEC3:
					{
						if ( elements == 1 ) 
						{
							pBufferF[ floatOffset ] = pCurrLocation.m_pCurrValue->GetValues()[ 0 ];
							pBufferF[ floatOffset + 1 ] = pCurrLocation.m_pCurrValue->GetValues()[ 1 ];
							pBufferF[ floatOffset + 2 ] = pCurrLocation.m_pCurrValue->GetValues()[ 2 ];
						}
						else if ( pCurrLocationStatic.m_iArrayStride == sizeof(float)*3 )
						{
							memcpy( pBufferF + floatOffset, pCurrLocation.m_pCurrValue->GetValues(), elements*sizeof(float)*3 );
						}
						else
						{
							uint32_t floatStride = pCurrLocationStatic.m_iArrayStride / 4;
							float *pSrc = pCurrLocation.m_pCurrValue->GetValues();
							for( uint32_t j = 0; j < elements; j++ )
							{
								pBufferF[ floatOffset ] = pSrc[ j*3 ];
								pBufferF[ floatOffset + 1 ] = pSrc[ j*3 + 1 ];
								pBufferF[ floatOffset + 2 ] = pSrc[ j*3 + 2 ];
								floatOffset += floatStride;
							}
						}
						break;
					}
					case AGK_SHADER_CONSTANT_TYPE_VEC4:
					{
						if ( elements == 1 || pCurrLocationStatic.m_iArrayStride == sizeof(float)*4 ) 
						{
							memcpy( pBufferF + floatOffset, pCurrLocation.m_pCurrValue->GetValues(), elements*sizeof(float)*4 );
						}
						else
						{
							uint32_t floatStride = pCurrLocationStatic.m_iArrayStride / 4;
							float *pSrc = pCurrLocation.m_pCurrValue->GetValues();
							for( uint32_t j = 0; j < elements; j++ )
							{
								memcpy( pBufferF + floatOffset, pSrc + j*4, elements*sizeof(float)*4 );
								floatOffset += floatStride;
							}
						}
						break;
					}
					case AGK_SHADER_CONSTANT_TYPE_MAT2:
					{
						uint32_t matrixStride = pCurrLocationStatic.m_iStride / 4;
						if ( matrixStride == 0 ) matrixStride = 4; // std140 default
						if ( elements == 1 ) 
						{
							if ( matrixStride == 2 ) memcpy( pBufferF + floatOffset, pCurrLocation.m_pCurrValue->GetValues(), sizeof(float)*4 );
							else
							{
								pBufferF[ floatOffset ] = pCurrLocation.m_pCurrValue->GetValues()[ 0 ];
								pBufferF[ floatOffset + 1 ] = pCurrLocation.m_pCurrValue->GetValues()[ 1 ];
								pBufferF[ floatOffset + matrixStride ] = pCurrLocation.m_pCurrValue->GetValues()[ 2 ];
								pBufferF[ floatOffset + matrixStride + 1 ] = pCurrLocation.m_pCurrValue->GetValues()[ 3 ];
							}
						}
						else if ( pCurrLocationStatic.m_iArrayStride == sizeof(float)*4 && matrixStride == 2 )
						{
							memcpy( pBufferF + floatOffset, pCurrLocation.m_pCurrValue->GetValues(), elements*sizeof(float)*4 );
						}
						else
						{
							uint32_t floatStride = pCurrLocationStatic.m_iArrayStride / 4;
							float *pSrc = pCurrLocation.m_pCurrValue->GetValues();
							for( uint32_t j = 0; j < elements; j++ )
							{
								pBufferF[ floatOffset ] = pCurrLocation.m_pCurrValue->GetValues()[ j*4 ];
								pBufferF[ floatOffset + 1 ] = pCurrLocation.m_pCurrValue->GetValues()[ j*4 + 1 ];
								pBufferF[ floatOffset + matrixStride ] = pCurrLocation.m_pCurrValue->GetValues()[ j*4 + 2 ];
								pBufferF[ floatOffset + matrixStride + 1 ] = pCurrLocation.m_pCurrValue->GetValues()[ j*4 + 3 ];
								floatOffset += floatStride;
							}
						}
						break;
					}
					case AGK_SHADER_CONSTANT_TYPE_MAT3:
					{
						uint32_t matrixStride = pCurrLocationStatic.m_iStride / 4;
						if ( matrixStride == 0 ) matrixStride = 4; // std140 default
						if ( elements == 1 ) 
						{
							if ( matrixStride == 3 ) memcpy( pBufferF + floatOffset, pCurrLocation.m_pCurrValue->GetValues(), sizeof(float)*9 );
							else
							{
								memcpy( pBufferF + floatOffset, pCurrLocation.m_pCurrValue->GetValues(), sizeof(float)*3 );
								memcpy( pBufferF + floatOffset + matrixStride, pCurrLocation.m_pCurrValue->GetValues() + 3, sizeof(float)*3 );
								memcpy( pBufferF + floatOffset + matrixStride*2, pCurrLocation.m_pCurrValue->GetValues() + 6, sizeof(float)*3 );
							}
						}
						else if ( pCurrLocationStatic.m_iArrayStride == sizeof(float)*9 && matrixStride == 3 )
						{
							memcpy( pBufferF + floatOffset, pCurrLocation.m_pCurrValue->GetValues(), elements*sizeof(float)*9 );
						}
						else
						{
							uint32_t floatStride = pCurrLocationStatic.m_iArrayStride / 4;
							float *pSrc = pCurrLocation.m_pCurrValue->GetValues();
							for( uint32_t j = 0; j < elements; j++ )
							{
								memcpy( pBufferF + floatOffset, pSrc + j*9, sizeof(float)*3 );
								memcpy( pBufferF + floatOffset + matrixStride, pSrc + j*9 + 3, sizeof(float)*3 );
								memcpy( pBufferF + floatOffset + matrixStride*2, pSrc + j*9 + 6, sizeof(float)*3 );
								floatOffset += floatStride;
							}
						}
						break;
					}
					case AGK_SHADER_CONSTANT_TYPE_MAT4:
					{
						uint32_t matrixStride = pCurrLocationStatic.m_iStride / 4;
						if ( matrixStride == 0 ) matrixStride = 4; // std140 default
						if ( elements == 1 ) 
						{
							if ( matrixStride == 4 ) memcpy( pBufferF + floatOffset, pCurrLocation.m_pCurrValue->GetValues(), sizeof(float)*16 );
							else
							{
								memcpy( pBufferF + floatOffset, pCurrLocation.m_pCurrValue->GetValues(), sizeof(float)*4 );
								memcpy( pBufferF + floatOffset + matrixStride, pCurrLocation.m_pCurrValue->GetValues() + 4, sizeof(float)*4 );
								memcpy( pBufferF + floatOffset + matrixStride*2, pCurrLocation.m_pCurrValue->GetValues() + 8, sizeof(float)*4 );
								memcpy( pBufferF + floatOffset + matrixStride*3, pCurrLocation.m_pCurrValue->GetValues() + 12, sizeof(float)*4 );
							}
						}
						else if ( pCurrLocationStatic.m_iArrayStride == sizeof(float)*16 && matrixStride == 4 )
						{
							memcpy( pBufferF + floatOffset, pCurrLocation.m_pCurrValue->GetValues(), elements*sizeof(float)*16 );
						}
						else
						{
							uint32_t floatStride = pCurrLocationStatic.m_iArrayStride / 4;
							float *pSrc = pCurrLocation.m_pCurrValue->GetValues();
							for( uint32_t j = 0; j < elements; j++ )
							{
								memcpy( pBufferF + floatOffset, pSrc + j*16, sizeof(float)*4 );
								memcpy( pBufferF + floatOffset + matrixStride, pSrc + j*16 + 4, sizeof(float)*4 );
								memcpy( pBufferF + floatOffset + matrixStride*2, pSrc + j*16 + 8, sizeof(float)*4 );
								memcpy( pBufferF + floatOffset + matrixStride*3, pSrc + j*16 + 12, sizeof(float)*4 );
								floatOffset += floatStride;
							}
						}
						break;
					}
					default: AppError( "Unrecognised shader constant type" );
				}
			}
		}

		// bind this set 
		vkCmdBindDescriptorSets( m_vkCommandBuffer[m_iCurrentFrame], VK_PIPELINE_BIND_POINT_GRAPHICS, pVulkanShader->m_vkPipelineLayout, pDataStatic.m_iSetID, 1, &pData->m_vkDescriptorSet, 0, 0 );
	}

	if ( pShader->m_iFlags & AGK_SHADER_RELOAD_UNIFORMS ) pShader->m_iFlags &= ~AGK_SHADER_RELOAD_UNIFORMS;

	if ( deleteConstants ) DeleteShaderConstants( pVulkanConstants );
	
	return APP_SUCCESS;
}

int VulkanRenderer::BindShaderConstantUpdates( void *pCommandBuffer, AGKShader *pShader, AGKUnorderedArray<AGKShaderConstantValue*> *pConstants, AGKDynamicArray<cImage*> *pTextures, void **inoutConstants )
{
	agk::Error( "BindShaderConstantUpdates currently not used" );
	return APP_ERROR_UNKNOWN;
}

int VulkanRenderer::DeleteShaderConstants( void *pConstants ) 
{ 
	if ( !pConstants ) return APP_SUCCESS;

	VulkanShaderConstants *pVulkanConstants = (VulkanShaderConstants*) pConstants;
	pVulkanConstants->m_iFlags |= AGK_VK_SHADER_CONSTANTS_DELETE;

	return APP_SUCCESS; 
}
			

#endif

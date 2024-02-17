#include "agk.h"

using namespace AGK;

uint32_t AGKShaderConstantValue::g_iGlobalChangedVersion = 0;

AGKShaderConstantValue AGKShader::g_varTime;
AGKShaderConstantValue AGKShader::g_varSinTime;
AGKShaderConstantValue AGKShader::g_varInvert;
AGKShaderConstantValue AGKShader::g_varResolution;
AGKShaderConstantValue AGKShader::g_varOrtho;
AGKShaderConstantValue AGKShader::g_varShadowProj;
AGKShaderConstantValue AGKShader::g_varShadowProj2;
AGKShaderConstantValue AGKShader::g_varShadowProj3;
AGKShaderConstantValue AGKShader::g_varShadowProj4;
AGKShaderConstantValue AGKShader::g_varShadowParams;
AGKShaderConstantValue AGKShader::g_varShadowParams2;
AGKShaderConstantValue AGKShader::g_varFogRange;
AGKShaderConstantValue AGKShader::g_varFogColor1;
AGKShaderConstantValue AGKShader::g_varFogColor2;

AGKShaderConstantValue AGKShader::g_varDirLightDir;
AGKShaderConstantValue AGKShader::g_varDirLightColor;
AGKShaderConstantValue AGKShader::g_varLightAmbient;

AGKShaderConstantValue AGKShader::g_varDefaultUVBounds;

AGKShader* AGKShader::g_pShaderColor = 0;
AGKShader* AGKShader::g_pShaderTexColor = 0;
AGKShader* AGKShader::g_pShaderFont = 0;
AGKShader* AGKShader::g_pShaderQuad = 0;
AGKShader* AGKShader::g_pShaderShadow = 0;
AGKShader* AGKShader::g_pShaderShadowAlpha = 0;
AGKShader* AGKShader::g_pShaderShadowBone = 0;
AGKShader* AGKShader::g_pShaderShadowBoneAlpha = 0;
AGKShader* AGKShader::g_pShader3DParticlesTex = 0;
AGKShader* AGKShader::g_pShader3DParticlesColor = 0;

AGKShader* AGKShader::g_pAllShaders = 0;
AGKShader* AGKShader::g_pLastShader = 0;
uint64_t AGKShader::g_iLastCreated = 1;

int AGKShader::g_iNumShadowBones = 0;
int AGKShader::g_iNumShadowBonesAlpha = 0;

void AGKShader::CreateDefaultShaders()
{
	Renderer *pRenderer = agk::GetRenderer();

	AGKMatrix4 identity;
	float currtime = agk::Timer();
	g_varTime.SetFloat( currtime );
	g_varSinTime.SetFloat( agk::Sin(currtime) );
	g_varInvert.SetFloat( pRenderer->IsTopLeftOrigin() ? -1.0f : 1.0f ); // 1 is normal, -1 is inverted
	g_varResolution.SetVector2( (float)agk::GetDeviceWidth(), (float)agk::GetDeviceHeight() );
	g_varOrtho.SetMatrix4( identity.GetFloatPtr() );
	g_varShadowProj.SetMatrix4( identity.GetFloatPtr() );
	g_varShadowProj2.SetMatrix4( identity.GetFloatPtr() );
	g_varShadowProj3.SetMatrix4( identity.GetFloatPtr() );
	g_varShadowProj4.SetMatrix4( identity.GetFloatPtr() );
	g_varShadowParams.SetVector4( 1000.0f, agk::m_fShadowBias, 1.0f/agk::m_iShadowMapWidth, 1.0f/agk::m_iShadowMapHeight );
	g_varShadowParams2.SetVector4( 0, 0, 0, 0 );
	g_varFogRange.SetVector2( 50, 700 );
	g_varFogColor1.SetVector3( 0.63f, 0.72f, 0.82f );
	g_varFogColor2.SetVector3( 1.0f, 0.9f, 0.7f );

	g_varDirLightDir.SetVector3( 0,0,0 );
	g_varDirLightColor.SetVector3( 0,0,0 );
	g_varLightAmbient.SetVector3( 0,0,0 );

	g_varDefaultUVBounds.SetVector4( 1, 1, 0, 0 ); // scale first then offset

	if ( !g_pShaderColor ) g_pShaderColor = pRenderer->LoadDefaultShaderColor();
	if ( !g_pShaderTexColor ) g_pShaderTexColor = pRenderer->LoadDefaultShaderTexColor();
	if ( !g_pShaderFont ) g_pShaderFont = pRenderer->LoadDefaultShaderFont();
	if ( !g_pShaderQuad ) g_pShaderQuad = pRenderer->LoadDefaultShaderQuad();
	if ( !g_pShader3DParticlesTex ) g_pShader3DParticlesTex = pRenderer->LoadDefaultShader3DParticlesTex();
	if ( !g_pShader3DParticlesColor ) g_pShader3DParticlesColor = pRenderer->LoadDefaultShader3DParticlesColor();
	// don't create shadow shaders until requested
}

void AGKShader::DeleteDefaultShaders()
{
	Renderer *pRenderer = agk::GetRenderer();

	if ( g_pShaderColor ) 
	{
		pRenderer->DeleteShader( g_pShaderColor );
		delete g_pShaderColor;
		g_pShaderColor = 0;
	}
	if ( g_pShaderTexColor ) 
	{
		pRenderer->DeleteShader( g_pShaderTexColor );
		delete g_pShaderTexColor;
		g_pShaderTexColor = 0;
	}
	if ( g_pShaderFont ) 
	{
		pRenderer->DeleteShader( g_pShaderFont );
		delete g_pShaderFont;
		g_pShaderFont = 0;
	}
	if ( g_pShaderQuad ) 
	{
		pRenderer->DeleteShader( g_pShaderQuad );
		delete g_pShaderQuad;
		g_pShaderQuad = 0;
	}
	if ( g_pShader3DParticlesTex ) 
	{
		pRenderer->DeleteShader( g_pShader3DParticlesTex );
		delete g_pShader3DParticlesTex;
		g_pShader3DParticlesTex = 0;
	}
	if ( g_pShader3DParticlesColor ) 
	{
		pRenderer->DeleteShader( g_pShader3DParticlesColor );
		delete g_pShader3DParticlesColor;
		g_pShader3DParticlesColor = 0;
	}
	if ( g_pShaderShadow )
	{
		pRenderer->DeleteShader( g_pShaderShadow );
		delete g_pShaderShadow;
		g_pShaderShadow = 0;
	}
	if ( g_pShaderShadowAlpha )
	{
		pRenderer->DeleteShader( g_pShaderShadowAlpha );
		delete g_pShaderShadowAlpha;
		g_pShaderShadowAlpha = 0;
	}
	if ( g_pShaderShadowBone )
	{
		pRenderer->DeleteShader( g_pShaderShadowBone );
		delete g_pShaderShadowBone;
		g_pShaderShadowBone = 0;
	}
	if ( g_pShaderShadowBoneAlpha )
	{
		pRenderer->DeleteShader( g_pShaderShadowBoneAlpha );
		delete g_pShaderShadowBoneAlpha;
		g_pShaderShadowBoneAlpha = 0;
	}
}

AGKShader* AGKShader::GetShadowShader( int numBones, int alphamask )
{
	if ( numBones > 0 )
	{
		if ( alphamask )
		{
			if ( g_pShaderShadowBoneAlpha && g_iNumShadowBonesAlpha < numBones ) 
			{
				delete g_pShaderShadowBoneAlpha;
				g_pShaderShadowBoneAlpha = 0;
			}
			
			if ( !g_pShaderShadowBoneAlpha ) 
			{
				g_pShaderShadowBoneAlpha = agk::GetRenderer()->LoadDefaultShaderShadowBoneAlpha( numBones );
				g_iNumShadowBonesAlpha = numBones;
			}
			
			return g_pShaderShadowBoneAlpha;
		}
		else
		{
			if ( g_pShaderShadowBone && g_iNumShadowBones < numBones ) 
			{
				delete g_pShaderShadowBone;
				g_pShaderShadowBone = 0;
			}
			
			if ( !g_pShaderShadowBone )
			{
				g_pShaderShadowBone = agk::GetRenderer()->LoadDefaultShaderShadowBone( numBones );
				g_iNumShadowBones = numBones;
			}

			return g_pShaderShadowBone;
		}
	}
	else
	{
		if ( alphamask )
		{
			if ( g_pShaderShadowAlpha ) return g_pShaderShadowAlpha;
			else
			{
				g_pShaderShadowAlpha = agk::GetRenderer()->LoadDefaultShaderShadowAlpha();
				return g_pShaderShadowAlpha;
			}
		}
		else
		{
			if ( g_pShaderShadow ) return g_pShaderShadow;
			else
			{
				g_pShaderShadow = agk::GetRenderer()->LoadDefaultShaderShadow();
				return g_pShaderShadow;
			}
		}
	}
}

void AGKShader::ReloadAll()
{
	Renderer *pRenderer = agk::GetRenderer();

	// delete
	AGKShader *pShader = g_pAllShaders;
	while( pShader )
	{
		pRenderer->ReloadShader( pShader );
		pShader = pShader->m_pNextShader;
	}
}

AGKShader::AGKShader() : m_baseConstants(16), m_constantBindingByName(16), m_cDerivedShaders(2)
{ 
	// add to list of all shaders
	m_pNextShader = g_pAllShaders;
	m_pPrevShader = 0;
	if ( g_pAllShaders ) g_pAllShaders->m_pPrevShader = this;
	g_pAllShaders = this;
	if ( !g_pLastShader ) g_pLastShader = this;

	m_iCreated = g_iLastCreated;
	g_iLastCreated++;
}

AGKShader::~AGKShader() 
{ 
	m_iFlags |= AGK_SHADER_DELETING;
	// remove from list of all shaders
	if ( m_pNextShader ) m_pNextShader->m_pPrevShader = m_pPrevShader;
	else g_pLastShader = m_pPrevShader;
	
	if ( m_pPrevShader ) m_pPrevShader->m_pNextShader = m_pNextShader;
	else g_pAllShaders = m_pNextShader;
	
	if ( !m_pBaseShader )
	{
		// this is a base shader, so others may exist that were based on it, need to delete them too
		for( uint32_t i = 0; i < m_cDerivedShaders.NumItems(); i++ ) delete m_cDerivedShaders[ i ];
	}
	else
	{
		m_pBaseShader->RemoveDerived( this );
	}

	if ( m_pAttribs ) delete [] m_pAttribs;
	m_localConstants.Clear( true );
	agk::GetRenderer()->DeleteShader( this );
}

void AGKShader::AddDerived( AGKShader *pShader )
{
	if ( m_iFlags & AGK_SHADER_DELETING ) return;
	m_cDerivedShaders.AddItem( pShader );
}

void AGKShader::RemoveDerived( AGKShader *pShader )
{
	if ( m_iFlags & AGK_SHADER_DELETING ) return;
	for( uint32_t i = 0; i < m_cDerivedShaders.NumItems(); i++ ) 
	{
		if ( m_cDerivedShaders[ i ] == pShader ) 
		{
			m_cDerivedShaders.RemoveIndex( i );
			i--;
		}
	}
}

void AGKShader::AddRef() 
{ 
	m_iMeshUseCount++;

	if ( g_pAllShaders == this ) return;

	// remove from list of all shaders
	if ( m_pNextShader ) m_pNextShader->m_pPrevShader = m_pPrevShader;
	else g_pLastShader = m_pPrevShader;
	
	if ( m_pPrevShader ) m_pPrevShader->m_pNextShader = m_pNextShader;
	else g_pAllShaders = m_pNextShader;

	// re-add at head of the list so we can keep track of most recently used
	m_pNextShader = g_pAllShaders;
	m_pPrevShader = 0;
	if ( g_pAllShaders ) g_pAllShaders->m_pPrevShader = this;
	g_pAllShaders = this;
	if ( !g_pLastShader ) g_pLastShader = this;
}

void AGKShader::RemoveRef() 
{ 
	m_iMeshUseCount--; 
	if ( m_iMeshUseCount < 0 ) agk::Error("Shader released too many times"); 
}

void AGKShader::SetConstantByName( const char* szName, float f1, float f2, float f3, float f4 )
{
	if ( !szName ) return;

	AGKShaderConstantValue *pConstant = 0;
	if ( NeedsAdditionalCode() )
	{
		// shader is a base for others, has no bindings of its own
		pConstant = m_baseConstants.GetItem( szName );
		if ( !pConstant ) 
		{
			pConstant = new AGKShaderConstantValue();
			pConstant->SetName( szName );
			m_baseConstants.AddItem( pConstant, szName );

			// tell any derived shaders
			for( uint32_t i = 0; i < m_cDerivedShaders.NumItems(); i++ ) 
			{
				uint32_t binding = m_cDerivedShaders[i]->GetBindingForName( szName );
				if ( binding != (uint32_t) -1 ) agk::GetRenderer()->SetShaderConstant( m_cDerivedShaders[i], binding, pConstant );
			}
		}
	}
	else
	{
		// shader is standalone
		uint32_t binding = GetBindingForName( szName );
		if ( binding == (uint32_t) -1 ) return;

		pConstant = m_localConstants.Find( binding );
		if ( !pConstant )
		{
			pConstant = new AGKShaderConstantValue();
			pConstant->SetBindingName( binding );
			m_localConstants.Add( pConstant );
			agk::GetRenderer()->SetShaderConstant( this, binding, pConstant );
		}
	}

	pConstant->SetVector4( f1, f2, f3, f4 );
}

void AGKShader::SetConstantMatrixByName( const char* szName, int numValues, const float* values )
{
	if ( !szName ) return;
	if ( numValues != 4 && numValues != 9 && numValues != 16 ) return;

	AGKShaderConstantValue *pConstant = 0;
	if ( NeedsAdditionalCode() )
	{
		// shader is a base for others, has no bindings of its own
		pConstant = m_baseConstants.GetItem( szName );
		if ( !pConstant ) 
		{
			pConstant = new AGKShaderConstantValue();
			pConstant->SetName( szName );
			m_baseConstants.AddItem( pConstant, szName );

			// tell any derived shaders
			for( uint32_t i = 0; i < m_cDerivedShaders.NumItems(); i++ ) 
			{
				uint32_t binding = m_cDerivedShaders[i]->GetBindingForName( szName );
				if ( binding != (uint32_t) -1 ) agk::GetRenderer()->SetShaderConstant( m_cDerivedShaders[i], binding, pConstant );
			}
		}
	}
	else
	{
		// shader is standalone
		uint32_t binding = GetBindingForName( szName );
		if ( binding == (uint32_t) -1 ) return;

		pConstant = m_localConstants.Find( binding );
		if ( !pConstant )
		{
			pConstant = new AGKShaderConstantValue();
			pConstant->SetBindingName( binding );
			m_localConstants.Add( pConstant );
			agk::GetRenderer()->SetShaderConstant( this, binding, pConstant );
		}
	}

	if ( numValues == 16 ) pConstant->SetMatrix4( values );
	else if ( numValues == 9 ) pConstant->SetMatrix3( values );
	else if ( numValues == 4 ) pConstant->SetMatrix2( values );
}

void AGKShader::SetConstantArrayByName( const char* szName, uint32_t index, uint32_t numValues, float f1, float f2, float f3, float f4 )
{
	if ( !szName ) return;

	AGKShaderConstantValue *pConstant = 0;
	if ( NeedsAdditionalCode() )
	{
		// shader is a base for others, has no bindings of its own
		pConstant = m_baseConstants.GetItem( szName );
		if ( !pConstant ) 
		{
			pConstant = new AGKShaderConstantArray();
			pConstant->SetName( szName );
			m_baseConstants.AddItem( pConstant, szName );

			// tell any derived shaders
			for( uint32_t i = 0; i < m_cDerivedShaders.NumItems(); i++ ) 
			{
				uint32_t binding = m_cDerivedShaders[i]->GetBindingForName( szName );
				if ( binding != (uint32_t) -1 ) agk::GetRenderer()->SetShaderConstant( m_cDerivedShaders[i], binding, pConstant );
			}
		}
	}
	else
	{
		// shader is standalone
		uint32_t binding = GetBindingForName( szName );
		if ( binding == (uint32_t) -1 ) return;

		pConstant = m_localConstants.Find( binding );
		if ( !pConstant )
		{
			pConstant = new AGKShaderConstantArray();
			pConstant->SetBindingName( binding );
			m_localConstants.Add( pConstant );
			agk::GetRenderer()->SetShaderConstant( this, binding, pConstant );
		}
	}

	if ( !pConstant->IsArray() ) return;
	switch( numValues )
	{
		case 1: ((AGKShaderConstantArray*)pConstant)->SetArrayFloat( index, f1 ); break;
		case 2: ((AGKShaderConstantArray*)pConstant)->SetArrayVector2( index, f1, f2 ); break;
		case 3: ((AGKShaderConstantArray*)pConstant)->SetArrayVector3( index, f1, f2, f3 ); break;
		case 4: ((AGKShaderConstantArray*)pConstant)->SetArrayVector4( index, f1, f2, f3, f4 ); break;
	}
}

void AGKShader::SetConstantArrayMatrixByName( const char* szName, uint32_t index, int numValues, const float* values )
{
	if ( !szName ) return;

	AGKShaderConstantValue *pConstant = 0;
	if ( NeedsAdditionalCode() )
	{
		// shader is a base for others, has no bindings of its own
		pConstant = m_baseConstants.GetItem( szName );
		if ( !pConstant ) 
		{
			pConstant = new AGKShaderConstantArray();
			pConstant->SetName( szName );
			m_baseConstants.AddItem( pConstant, szName );

			// tell any derived shaders
			for( uint32_t i = 0; i < m_cDerivedShaders.NumItems(); i++ ) 
			{
				uint32_t binding = m_cDerivedShaders[i]->GetBindingForName( szName );
				if ( binding != (uint32_t) -1 ) agk::GetRenderer()->SetShaderConstant( m_cDerivedShaders[i], binding, pConstant );
			}
		}
	}
	else
	{
		// shader is standalone
		uint32_t binding = GetBindingForName( szName );
		if ( binding == (uint32_t) -1 ) return;

		pConstant = m_localConstants.Find( binding );
		if ( !pConstant )
		{
			pConstant = new AGKShaderConstantArray();
			pConstant->SetBindingName( binding );
			m_localConstants.Add( pConstant );
			agk::GetRenderer()->SetShaderConstant( this, binding, pConstant );
		}
	}

	if ( !pConstant->IsArray() ) return;
	AGKShaderConstantArray *pConstantArray = (AGKShaderConstantArray*) pConstant;
	if ( numValues == 16 ) pConstantArray->SetArrayMatrix4( index, values );
	else if ( numValues == 9 ) pConstantArray->SetArrayMatrix3( index, values );
	else if ( numValues == 4 ) pConstantArray->SetArrayMatrix2( index, values );
}

uint32_t AGKShader::GetMeshShaderHash( cMesh *pMesh )
{
	// hash must never be 0 or it will match an uninitialised shader
	uint32_t hash = 1;
	if ( pMesh->HasNormals() && pMesh->WantsLighting() )	
	{
		hash |= 0x0002;
		if ( pMesh->HasNormalMap() ) 
		{
			hash |= 0x10000;
			if ( pMesh->GetImage(2)->HasModifiedUVs() ) hash |= 0x400000;
		}
	}
	if ( pMesh->HasUVs() )			hash |= 0x0004; 
	if ( pMesh->HasUV1s() )			hash |= 0x0008;
	if ( pMesh->HasValidBones() )	hash |= 0x0010;
	if ( pMesh->HasLightMap() )		hash |= 0x0020;
	if ( pMesh->GetObject() && pMesh->GetObject()->HasAlphaMask() ) hash |= 0x0040;
	if ( pMesh->WantsFog() && agk::GetFogMode() )	hash |= 0x0080;

	if ( pMesh->HasValidBones() )
	{
		hash |= (pMesh->GetNumBones() & 0xff) << 8;
	}

	if ( pMesh->HasVertColors() )	hash |= 0x20000;
	if ( pMesh->HasTexture0() )		
	{
		hash |= 0x40000; 
		if ( pMesh->GetImage(0)->HasModifiedUVs() ) hash |= 0x80000;
	}
	if ( pMesh->HasTexture1() )		
	{
		hash |= 0x100000;
		if ( pMesh->GetImage(1)->HasModifiedUVs() ) hash |= 0x200000;
	}

	return hash;
}

uint32_t AGKShader::GetFinalShaderHash( int sunActive, int VSLights, int PSLights, int useShadows )
{
	if ( sunActive == 0 ) useShadows = 0;
	if ( agk::GetShadowMappingMode() == 0 ) useShadows = 0;

	// hash must never be 0 or it will match an uninitialised shader
	uint32_t hash = ((VSLights+1) & 0xff) | ((PSLights & 0xff) << 8);
	if ( agk::GetFogMode() ) hash |= 0x00010000;
	if ( agk::GetFogColorsEqual() ) hash |= 0x00020000;
	if ( useShadows )
	{
		if ( agk::GetShadowMappingMode() == 1 ) hash |= 0x00040000; // Uniform shadows
		else if ( agk::GetShadowMappingMode() == 2 ) hash |= 0x00080000; // LiPSM shadows
		else if ( agk::GetShadowMappingMode() == 3 ) hash |= 0x000C0000; // Cascade shadows

		if ( agk::GetShadowSmoothing() == 1 ) hash |= 0x00100000; // 4 fixed sample shadows
		else if ( agk::GetShadowSmoothing() == 2 ) hash |= 0x00200000; // 4 random sample shadows
		else if ( agk::GetShadowSmoothing() == 3 ) hash |= 0x00300000; // 16 fixed sample shadows
		else if ( agk::GetShadowSmoothing() == 4 ) hash |= 0x00400000; // 16 random sample shadows
	}
	
	return hash;
}

AGKShader* AGKShader::Make3DShader( cMesh *pMesh )
{
	uint32_t hash = GetMeshShaderHash( pMesh );

	AGKShader *pShader = g_pAllShaders;
	while( pShader )
	{
		if ( pShader->m_pBaseShader == 0 && pShader->m_iShaderHash == hash ) return pShader;
		pShader = pShader->m_pNextShader;
	}

	// not found in cache, generate a new one
	pShader = agk::GetRenderer()->Make3DShader( pMesh );
	if ( !pShader ) return 0;

	pShader->m_iShaderHash = hash;
	return pShader;
}

AGKShader* AGKShader::MakeFinalShader( AGKShader *pBaseShader, int sunActive, int VSLights, int PSLights, int useShadows, int normalMap )
{
	if ( VSLights > AGK_MAX_VERTEX_LIGHTS ) VSLights = AGK_MAX_VERTEX_LIGHTS;
	if ( PSLights > AGK_MAX_PIXEL_LIGHTS ) PSLights = AGK_MAX_PIXEL_LIGHTS;

	if ( useShadows > 1 ) useShadows = 1;
	if ( sunActive == 0 ) useShadows = 0;
	if ( agk::GetShadowMappingMode() == 0 ) useShadows = 0;

	// look in the shader cache
	uint32_t hash = GetFinalShaderHash( sunActive, VSLights, PSLights, useShadows );
	
	AGKShader *pShader = g_pAllShaders;
	while( pShader )
	{
		if ( pShader->m_pBaseShader == pBaseShader && pShader->m_iShaderHash == hash ) return pShader;
		pShader = pShader->m_pNextShader;
	}

	// not found in cache, generate a new one
	pShader = agk::GetRenderer()->MakeFinalShader( pBaseShader, sunActive, VSLights, PSLights, useShadows, normalMap );
	if ( !pShader ) return 0;
	
	pShader->m_iShaderHash = hash;
	pBaseShader->AddDerived( pShader );
	
	// set uniforms from base shader
	AGKShaderConstantValue *pConstant = pBaseShader->m_baseConstants.GetFirst();
	while( pConstant )
	{
		uint32_t binding = pShader->GetBindingForName( pConstant->GetName() );
		if ( binding != (uint32_t) -1 ) agk::GetRenderer()->SetShaderConstant( pShader, binding, pConstant );
		
		pConstant = pBaseShader->m_baseConstants.GetNext();
	}

	return pShader;
}

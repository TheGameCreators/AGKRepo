
// Data stuctures
enum glsl450tokentypes
{
	glsl450none,
	glsl450tab,
	glsl450stuff,
	glsl450startspace,
	glsl450token_version,
	glsl450token_attribute,
	glsl450token_varying,
	glsl450token_uniform,
	glsl450token_uniformsampler,
	glsl450token_uniformend,
	glsl450token_uniformgroup,
	glsl450token_precisionhighp,
	glsl450token_highp,
	glsl450token_mediump,
	glsl450token_lowp,
	glsl450token_fragmain,
	glsl450token_gl_FragColor,
	glsl450token_texture2D,
	glsl450token_texture2Dlod,
	glsl450token_mat2,
	glsl450token_mat3,
	glsl450token_mat4,
	glsl450newline
};

struct glsl450tokenstruct
{
	glsl450tokentypes eTokenType;
	char* pTokenData;
};

// Header functions
bool glsl450_Convert ( const char* pVert, const char* pFrag, char** ppVert450, char** ppFrag450, const char* szVSFilename=0, const char* szPSFilename=0 );

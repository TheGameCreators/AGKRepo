//
// GLSLLANG Code
//

// Includes
//#include "stdafx.h"
//#include "targetver.h"
#include "glslang_main.h"
#include <vector>
#include <map>

// Globals
std::vector<std::string> Processes;
TBuiltInResource Resources = glslang::DefaultTBuiltInResource;
const char* shaderStageName = nullptr;
const char* binaryFileName = nullptr;

// Target environment
int ClientInputSemanticsVersion = 100;
glslang::EShClient Client = glslang::EShClientVulkan;
glslang::EShTargetClientVersion ClientVersion = glslang::EShTargetVulkan_1_0;
glslang::EShTargetLanguage TargetLanguage = glslang::EShTargetSpv;
glslang::EShTargetLanguageVersion TargetVersion = glslang::EShTargetSpv_1_0;

// Binding Activity
int uniformBase = 0;

// Globally track if any compile or link failure.
bool CompileFailed = false;
bool LinkFailed = false;

// Functions
bool glsl450_OutputSpirV( unsigned int stage, const char* szInput, unsigned int** outputSPIRV, unsigned int* outputSPIRVLength, char** pszError )
{
	// takes string data for GLSL 4.5 shader, outputs string data with length for SPIR-V binary
	if ( 1 )
	{
		// initialise glslang process
		static bool initDone = false;
		if ( !initDone ) 
		{
			glslang::InitializeProcess();
			initDone = true;
		}

		// unit to convert to SPIR-V
		glslang::TWorkItem* workItem = new glslang::TWorkItem();
		workItem->name = "glsl";
		ShaderCompUnit compUnit(FindLanguage(workItem->name));
		compUnit.addString(workItem->name, szInput);
		compUnit.stage = (EShLanguage)stage;

		// keep track of what to free
		std::list<glslang::TShader*> shaders;
		EShMessages messages = EShMsgDefault;

		// Compile..
		glslang::TShader* shader = new glslang::TShader(compUnit.stage);
		shader->setStringsWithLengthsAndNames(compUnit.text, NULL, compUnit.fileNameList, compUnit.count);
		shader->addProcesses(Processes);
		shader->setUniformLocationBase(uniformBase);
		shader->setEnvInput( glslang::EShSourceGlsl, compUnit.stage, Client, ClientInputSemanticsVersion);
		shader->setEnvClient(Client, ClientVersion);
		shader->setEnvTarget(TargetLanguage, TargetVersion);
		shaders.push_back(shader);
		const int defaultVersion = 100;
		DirStackFileIncluder includer;

		CompileFailed = false;
		if (! shader->parse(&Resources, defaultVersion, false, messages, includer))
		{
			CompileFailed = true;
			if ( pszError ) 
			{
				*pszError = new char[ strlen(shader->getInfoLog()) + 1 ];
				strcpy( *pszError, shader->getInfoLog() );
			}
			while (shaders.size() > 0) 
			{
				delete shaders.back();
				shaders.pop_back();
			}
			return false;
		}

		// add shader to program object
		glslang::TProgram& program = *new glslang::TProgram;
		program.addShader(shader);

		// process linkage step on program
		LinkFailed = false;
		if (!program.link(messages))
			LinkFailed = true;

		// process mapIO step on program
		if (!program.mapIO())
			LinkFailed = true;

		// Dump SPIR-V
		if (CompileFailed || LinkFailed)
		{
			if ( pszError ) 
			{
				*pszError = new char[ strlen(shader->getInfoLog()) + 1 ];
				strcpy( *pszError, shader->getInfoLog() );				
			}
			delete &program;
			while (shaders.size() > 0) 
			{
				delete shaders.back();
				shaders.pop_back();
			}
			return false;
		}
		else 
		{
			if (program.getIntermediate((EShLanguage)stage)) 
			{
				// intermediate to spir-v data
				std::vector<unsigned int> spirv;
				spv::SpvBuildLogger logger;
				glslang::SpvOptions spvOptions;
				spvOptions.disableOptimizer = 0;
				spvOptions.optimizeSize = 0;
				spvOptions.disassemble = false;
				spvOptions.validate = false;
				glslang::GlslangToSpv(*program.getIntermediate((EShLanguage)stage), spirv, &logger, &spvOptions);

				// now dump into output buffer
				if ( outputSPIRV && outputSPIRVLength )
				{
					// create buffer and copy CPV to it
					*outputSPIRVLength = spirv.size();
					*outputSPIRV = new unsigned int[*outputSPIRVLength];
					memcpy( *outputSPIRV, spirv.data(), spirv.size()*sizeof(unsigned int) );
				}
			}
		}

		// free usages
		delete &program;
		while (shaders.size() > 0) 
		{
			delete shaders.back();
			shaders.pop_back();
		}

		// freed outside of this call (closer to its creation point)
		// FreeFileData(const_cast<char*>(compUnit.text[0]));

		// finalise glslang process
		//glslang::FinalizeProcess();
	}

	// success
	return true;
}

//
//   Deduce the language from the filename.  Files must end in one of the
//   following extensions:
//
//   .vert = vertex
//   .tesc = tessellation control
//   .tese = tessellation evaluation
//   .geom = geometry
//   .frag = fragment
//   .comp = compute
//   .rgen = ray generation
//   .rint = ray intersection
//   .rahit = ray any hit
//   .rchit = ray closest hit
//   .rmiss = ray miss
//   .rcall = ray callable
//   .mesh  = mesh
//   .task  = task
//   Additionally, the file names may end in .<stage>.glsl and .<stage>.hlsl
//   where <stage> is one of the stages listed above.
//
EShLanguage FindLanguage(const std::string& name, bool parseStageName)
{
    std::string stageName;
    if (shaderStageName)
        stageName = shaderStageName;
    else if (parseStageName) {
        // Note: "first" extension means "first from the end", i.e.
        // if the file is named foo.vert.glsl, then "glsl" is first,
        // "vert" is second.
        size_t firstExtStart = name.find_last_of(".");
        bool hasFirstExt = firstExtStart != std::string::npos;
        size_t secondExtStart = hasFirstExt ? name.find_last_of(".", firstExtStart - 1) : std::string::npos;
        bool hasSecondExt = secondExtStart != std::string::npos;
        std::string firstExt = name.substr(firstExtStart + 1, std::string::npos);
        bool usesUnifiedExt = hasFirstExt && (firstExt == "glsl" || firstExt == "hlsl");
        //if (usesUnifiedExt && firstExt == "hlsl")
        //    Options |= EOptionReadHlsl;
        if (hasFirstExt && !usesUnifiedExt)
            stageName = firstExt;
        else if (usesUnifiedExt && hasSecondExt)
            stageName = name.substr(secondExtStart + 1, firstExtStart - secondExtStart - 1);
        else 
		{
            //usage();
            return EShLangVertex;
        }
    } else
        stageName = name;

    if (stageName == "vert")
        return EShLangVertex;
    else if (stageName == "tesc")
        return EShLangTessControl;
    else if (stageName == "tese")
        return EShLangTessEvaluation;
    else if (stageName == "geom")
        return EShLangGeometry;
    else if (stageName == "frag")
        return EShLangFragment;
    else if (stageName == "comp")
        return EShLangCompute;
#ifdef NV_EXTENSIONS
    else if (stageName == "rgen")
        return EShLangRayGenNV;
    else if (stageName == "rint")
        return EShLangIntersectNV;
    else if (stageName == "rahit")
        return EShLangAnyHitNV;
    else if (stageName == "rchit")
        return EShLangClosestHitNV;
    else if (stageName == "rmiss")
        return EShLangMissNV;
    else if (stageName == "rcall")
        return EShLangCallableNV;
    else if (stageName == "mesh")
        return EShLangMeshNV;
    else if (stageName == "task")
        return EShLangTaskNV;
#endif

    //usage();
    return EShLangVertex;
}

//
//   Malloc a string of sufficient size and read a string into it.
//
char* ReadFileData(const char* fileName)
{
    FILE *in = fopen(fileName, "r");
    if (in == nullptr)
		return NULL;//MessageBox ( NULL, "unable to open input file", "error", MB_OK ); //    Error("unable to open input file");

    int count = 0;
    while (fgetc(in) != EOF)
        count++;

    fseek(in, 0, SEEK_SET);

    char* return_data = (char*)malloc(count + 1);  // freed in FreeFileData()
    if ((int)fread(return_data, 1, count, in) != count) 
	{
        free(return_data);
        //Error("can't read input file");
    }

    return_data[count] = '\0';
    fclose(in);

    return return_data;
}

void FreeFileData(char* data)
{
    free(data);
}

const char* GetBinaryName(EShLanguage stage)
{
	//
	// Create the default name for saving a binary if -o is not provided.
	//
    const char* name;
    if (binaryFileName == nullptr) {
        switch (stage) {
        case EShLangVertex:          name = "vert.spv";    break;
        case EShLangTessControl:     name = "tesc.spv";    break;
        case EShLangTessEvaluation:  name = "tese.spv";    break;
        case EShLangGeometry:        name = "geom.spv";    break;
        case EShLangFragment:        name = "frag.spv";    break;
        case EShLangCompute:         name = "comp.spv";    break;
#ifdef NV_EXTENSIONS
        case EShLangRayGenNV:        name = "rgen.spv";    break;
        case EShLangIntersectNV:     name = "rint.spv";    break;
        case EShLangAnyHitNV:        name = "rahit.spv";   break;
        case EShLangClosestHitNV:    name = "rchit.spv";   break;
        case EShLangMissNV:          name = "rmiss.spv";   break;
        case EShLangCallableNV:      name = "rcall.spv";   break;
        case EShLangMeshNV:          name = "mesh.spv";    break;
        case EShLangTaskNV:          name = "task.spv";    break;
#endif
        default:                     name = "unknown";     break;
        }
    } else
        name = binaryFileName;

    return name;
}

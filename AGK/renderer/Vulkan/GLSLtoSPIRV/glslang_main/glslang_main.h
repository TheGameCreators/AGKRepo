//
// GLSLLANG Header
//

// Includes
#include "ResourceLimits.h"
#include "Worklist.h"
#include "DirStackFileIncluder.h"
#include "./../glslang/Public/ShaderLang.h"
#include "../SPIRV/GlslangToSpv.h"
#include "../SPIRV/GLSL.std.450.h"
#include "../SPIRV/doc.h"
#include "../SPIRV/disassemble.h"

// Structures
// Simple bundling of what makes a compilation unit for ease in passing around,
// and separation of handling file IO versus API (programmatic) compilation.
struct ShaderCompUnit 
{
    EShLanguage stage;
    static const int maxCount = 1;
    int count;                          // live number of strings/names
    const char* text[maxCount];         // memory owned/managed externally
    std::string fileName[maxCount];     // hold's the memory, but...
    const char* fileNameList[maxCount]; // downstream interface wants pointers

    ShaderCompUnit(EShLanguage stage) : stage(stage), count(0) { }

    ShaderCompUnit(const ShaderCompUnit& rhs)
    {
        stage = rhs.stage;
        count = rhs.count;
        for (int i = 0; i < count; ++i) {
            fileName[i] = rhs.fileName[i];
            text[i] = rhs.text[i];
            fileNameList[i] = rhs.fileName[i].c_str();
        }
    }

    void addString(std::string& ifileName, const char* itext)
    {
        //assert(count < maxCount);
        fileName[count] = ifileName;
        text[count] = itext;
        fileNameList[count] = fileName[count].c_str();
        ++count;
    }
};

// Functions
bool glsl450_OutputSpirV( unsigned int stage, const char* szInput, unsigned int** outputSPIRV, unsigned int* outputSPIRVLength, char** pszError );
EShLanguage FindLanguage(const std::string& name, bool parseStageName = true);
char* ReadFileData(const char* fileName);
void FreeFileData(char* data);
const char* GetBinaryName(EShLanguage stage);

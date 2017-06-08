//Log library
#include <iostream>

//CTexture.h
#define DEBUG_TEXTURE
#define WARNINGS_TEXTURE
#define EVENTS_TEXTURE
//CUniformTexture.h
#define DEBUG_UNIFORMTEXTURE

//CMeshInstanceLoader.h
#define DEBUG_MESHINSTANCELOADER
//CSimpleMesh.h
#define DEBUG_SIMPLEMESH

//CShader.h
#define DEBUG_SHADER
//CUniforms.h
#define DEBUG_UNIFORMS
//#define WARNINGS_UNIFORMS
//CUniformStruct.h
#define DEBUG_UNIFORMSTRUCT
//CUniformStructCollection.h
#define DEBUG_UNIFORMSTRUCTCOLLECTION
//CUniformMatrix.h
#define DEBUG_UNIFORMMATRIX
//CUniformVec3.h
#define DEBUG_UNIFORMVEC3
//CUniformNumber.h
#define DEBUG_UNIFORMNUMBER

//CCamera.h
#define DEBUG_CAMERA

//HDebugLight.h
#define DEBUG_LIGHT

//HDebugMaterial.h
#define DEBUG_MATERIAL

//Log output
#ifndef DEBUG_OUT
	#define DEBUG_OUT std::cout
#endif
#ifndef DEBUG_NEXT_LINE
	#define DEBUG_NEXT_LINE std::endl
#endif
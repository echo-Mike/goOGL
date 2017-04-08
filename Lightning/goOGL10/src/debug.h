//Log library
#include <iostream>
//Ctexture.h
#define DEBUG_TEXTURE
//CSimpleModel.h
#define DEBUG_SIMPLEMODEL
//CShader.h
#define DEBUG_SHADER
//CCamera.h
#define DEBUG_CAMERA
//CUniforms.h
#define DEBUG_UNIFORMS
//CUniformMatrix.h
#define DEBUG_UNIFORMMATRIX
//CUniformVec3.h
#define DEBUG_UNIFORMVEC3
//CUniformTexture.h
#define DEBUG_UNIFORMTEXTURE
//CCamera.h
#define DEBUG_CAMERA
//CInstanceLoader.h
#define DEBUG_INSTANCELOADER

//Log output
#ifndef DEBUG_OUT
	#define DEBUG_OUT std::cout
#endif
#ifndef DEBUG_NEXT_LINE
	#define DEBUG_NEXT_LINE std::endl
#endif
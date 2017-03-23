//Log library
#include <iostream>
//Ctexture.h
#define DEBUG_TEXTURE
//CSimpleModel.h
#define GEBUG_SIMPLEMODEL
//CShader.h
#define GEBUG_SHADER
//CCamera.h
#define DEBUG_CAMERA
//Log output
#ifndef DEBUG_OUT
	#define DEBUG_OUT std::cout
#endif
#ifndef DEBUG_NEXT_LINE
	#define DEBUG_NEXT_LINE std::endl
#endif
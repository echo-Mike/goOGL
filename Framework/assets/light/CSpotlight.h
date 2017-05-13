#ifndef SPOTLIGHT_H
#define SPOTLIGHT_H "[0.0.4@CSpotlight.h]"
/*
*	DESCRIPTION:
*		Module contains implementation of spotlight properties structure
*		for in-shader loading.
*	AUTHOR:
*		Mikhail Demchenko
*		mailto:dev.echo.mike@gmail.com
*		https://github.com/echo-Mike
*/
//STD
#include <string>
#include <type_traits>
//GLEW
#include <GL/glew.h>
//GLM
#include <GLM/glm.hpp>
//OUR
#include "assets\shader\CUniformStruct.h"
#include "assets\shader\CShader.h"
//DEBUG
#ifdef DEBUG_SPOTLIGHT
	#ifndef DEBUG_OUT
		#define DEBUG_OUT std::cout
	#endif
	#ifndef DEBUG_NEXT_LINE
		#define DEBUG_NEXT_LINE std::endl
	#endif
#endif

#endif
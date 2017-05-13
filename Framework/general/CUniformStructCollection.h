#ifndef UNIFORMSTRUCTCOLLECTION_H
#define UNIFORMSTRUCTCOLLECTION_H "[0.0.4@CUniformStructCollection.h]"
/*
*	DESCRIPTION:
*		Module contains implementation of common structures handlers 
*		for in-shader structs of uniform values.
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
#include "assets\shader\CUniforms.h"
#include "assets\shader\CUniformStruct.h"
#include "assets\shader\CShader.h"
//DEBUG
#ifdef DEBUG_UNIFORMSTRUCTCOLLECTION
	#ifndef DEBUG_OUT
		#define DEBUG_OUT std::cout
	#endif
	#ifndef DEBUG_NEXT_LINE
		#define DEBUG_NEXT_LINE std::endl
	#endif
#endif

#endif
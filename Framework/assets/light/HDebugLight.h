#ifndef DEBUGLIGHT_H
#define DEBUGLIGHT_H "[multi@HDebugLight.h]"
/*
*	DESCRIPTION:
*		Module contains debug defines for light folder of assets
*	AUTHOR:
*		Mikhail Demchenko
*		mailto:dev.echo.mike@gmail.com
*		https://github.com/echo-Mike
*/
#ifdef DEBUG_LIGHT
	#define DEBUG_DIRECTIONALLIGHT
	#define DEBUG_POINTLIGHT
	#define DEBUG_SPOTLIGHT
#endif
#include "CDirectionalLight.h"
#include "CPointLight.h"
#include "CSpotlight.h"
#endif
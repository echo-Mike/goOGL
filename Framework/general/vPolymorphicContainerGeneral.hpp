#ifndef POLYMORPHICCONTAINERGENERAL_H
#define POLYMORPHICCONTAINERGENERAL_H "[multy@vPolymorphicContainerGeneral.hpp]"
/*
*	DESCRIPTION:
*		DESCRIPTION
*	AUTHOR:
*		Mikhail Demchenko
*		mailto:dev.echo.mike@gmail.com
*		https://github.com/echo-Mike
*/
//STD
#include <memory>
//OUR
#include "vs2013tweaks.h"
#include "mConcepts.hpp"
//DEBUG
#ifdef DEBUG_POLYMORPHICCONTAINER
	#define DEBUG_POLYMORPHICMAP
	#define DEBUG_STRICTPOLYMORPHICMAP
#endif // DEBUG_POLYMORPHICCONTAINER

/**
*	Allocate strategy enum defines constants that specifies
*	what strategy to be used to allocate object in polymorphic containers.
**/
enum class allocateStrategy {
	//Strategy used in copy or move cases, 
	//defines that we must use allocation strategy of copied/moved object.
	NON,
	//Allocate resource using std::make_shared - suitable for SMALL objects.
	SMALL,
	//Allocate resource using std::shared_ptr(new ResType)  - suitable for BIG objects.
	BIG,
	//Small startegy is default (faster)
	DEFAULT = SMALL
};
#endif
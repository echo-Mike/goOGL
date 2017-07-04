#ifndef DEBUG_H
#define DEBUG_H "[multy@mDebug.h]"
/*
*	DESCRIPTION:
*		DESCRIPTION
*	AUTHOR:
*		Mikhail Demchenko
*		mailto:dev.echo.mike@gmail.com
*		https://github.com/echo-Mike
*/
//STD
#include <iostream>

//Log output
//Output stream
#ifndef DEBUG_OUT
	#define DEBUG_OUT std::cout
#endif
//New line
#ifndef DEBUG_NEXT_LINE
	#define DEBUG_NEXT_LINE "\n"
#endif
//End line and flush
#ifndef DEBUG_END_MESSAGE
	#define DEBUG_END_MESSAGE DEBUG_OUT << std::endl;
#endif

//Debug write macro
//Allow to output one value to debug stream with new line afterwards.
#define DEBUG_WRITE1(x) DEBUG_OUT << x << DEBUG_NEXT_LINE
//Allow to output two values to debug stream with new line afterwards.
#define DEBUG_WRITE2(x,y) DEBUG_OUT << x << y << DEBUG_NEXT_LINE
//Allow to output three values to debug stream with new line afterwards.
#define DEBUG_WRITE3(x,y,z) DEBUG_OUT << x << y << z << DEBUG_NEXT_LINE
//Allow to output four values to debug stream with new line afterwards.
#define DEBUG_WRITE4(x,y,z,w) DEBUG_OUT << x << y << z << w << DEBUG_NEXT_LINE

//First line
#ifndef DEBUG_NEW_MESSAGE
	//First line of new massege
	#define DEBUG_NEW_MESSAGE(x) DEBUG_OUT << x << DEBUG_NEXT_LINE;
#endif
#endif
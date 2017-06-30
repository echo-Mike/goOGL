#ifndef RESOURCEGENERAL_H
#define RESOURCEGENERAL_H "[0.0.5@vResourceGeneral.h]"
/*
*	DESCRIPTION:
*		Module contains general information of resource handling engine.
*	AUTHOR:
*		Mikhail Demchenko
*		mailto:dev.echo.mike@gmail.com
*		https://github.com/echo-Mike
*/

namespace resources {
	//Enumiration of all types of handled resources.
	enum class ResourceType : int {
		UNKNOWN,
		SHADER,
		TEXTURE,
		MESH,
		MATERIAL,
		LIGHT,
		AUDIO,
		CAMERA,

		TEXT,
		VALUE,

		MODEL,
		ANIMATION,
		SCENE,

		GROUP,
		ENGINE,

		GOUTPUT,
		INPUT,
		AOUTPUT,

		SCRIPT,
		FONT,

		LAST = FONT
	};

	//Unsigned integer type used as resources identificator.
	typedef unsigned int ResourceID;
}
#endif
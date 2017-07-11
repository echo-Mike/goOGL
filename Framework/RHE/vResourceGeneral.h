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
		//Unknown resource type
		UNKNOWN,
		//Shader resource type
		SHADER,
		//Texture resource type
		TEXTURE,
		//Mesh resource type
		MESH,
		//Material resource type
		MATERIAL,
		//Light resource type
		LIGHT,
		//Audio resource type
		AUDIO,
		//Camera resource type
		CAMERA,

		//Text resource type
		TEXT,
		//Resource type for handled single values
		VALUE,

		//Model resource type
		MODEL,
		//Animation resource type
		ANIMATION,
		//Scene resource type
		SCENE,

		//Resource type for many object manipulation
		GROUP,
		//Resource type for engines data
		ENGINE,

		//Resource type for graphical output stream
		GOUTPUT,
		//Resource type for input command stream
		INPUT,
		//Resource type for audio output stream
		AOUTPUT,

		//Lua script resource type
		SCRIPT,
		//Graphical font resource type
		FONT,

		//End of bits indicator
		MAX = FONT
	};

	//Unsigned integer type used as resources identificator.
	typedef unsigned int ResourceID;

	#ifndef RHE_GLOBAL_MAX_RESOURCES
		/**
		*	DESCRIPTION
		**/
		#define RHE_GLOBAL_MAX_RESOURCES ((ResourceID)0xF4240)
	#endif

	#ifndef RHE_ALLOCATION_BANDWIDTH
		/**
		*	DESCRIPTION
		**/
		#define RHE_ALLOCATION_BANDWIDTH ((unsigned int)0x9C4)
	#endif
}
#endif
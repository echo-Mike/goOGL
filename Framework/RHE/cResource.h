#ifndef RESOURCE_H
#define RESOURCE_H "[0.0.5@cResource.h]"
/*
*	DESCRIPTION:
*		Module contains implementation of resource base class.
*	AUTHOR:
*		Mikhail Demchenko
*		mailto:dev.echo.mike@gmail.com
*		https://github.com/echo-Mike
*/
//STD
#include <string>
#include <iostream>
#include <fstream>
//ASSIMP

//OUR
#include "RHE\vResourceGeneral.h"
//DEBUG
#ifdef DEBUG_RESOURCE
	#ifndef DEBUG_OUT
		#define DEBUG_OUT std::cout
	#endif
	#ifndef DEBUG_NEXT_LINE
		#define DEBUG_NEXT_LINE std::endl
	#endif
#endif

namespace resources {

	#ifdef RHE_USE_RESOURCE_NAMES
		/**
		*	Runtime identification of naming of resources.
		**/
		const bool __RHEUseNames = true;
	#else
		/**
		*	Runtime identification of naming of resources.
		**/
		const bool __RHEUseNames = false;
	#endif

	/**
	*	Base class for all resources.
	*	Class definition: Resource
	**/
	class Resource {
		friend class ResourceHandler;
		//Type information of resource
		ResourceType type;
		//Resource status information
		int status;
	protected:
		/**
		*	Cache flag for derived classes.
		*	This flag must be set, if resorce can be cached to file and restored back.
		*	Derived class must implement Cache() and Restore() functions.
		**/
		bool canBeCached = false;
		#ifdef RHE_USE_RESOURCE_NAMES
			//String identificator of resource.
			std::string __resourceName;
		#endif
	public:
		//Enumiration of all possible resource states.
		enum ResourceStatus : int {
			UNDEFINED	= 0x0000,
			//Resource have: Type, Id
			DEFINED		= 0x0001,
			//Load/Reload was called on resource 
			LOADED		= 0x0002,
			//Cache function was called on resource
			CACHED		= 0x0004,
			//Resource was copied from other resource
			COPIED		= 0x0100,
			//Resource was moved from other resource
			MOVED		= 0x0200
		};

		Resource() : type(resources::ResourceType::UNKNOWN),
					 #ifdef RHE_USERESOURCENAMES
						 __resourceName(""),
					 #endif
					 status(ResourceStatus::UNDEFINED) {}

		Resource(	
					#ifdef RHE_USE_RESOURCE_NAMES
						std::string _name,
					#endif
					ResourceType _type) :
					type(_type),
					#ifdef RHE_USE_RESOURCE_NAMES
						__resourceName(_name), 
					#endif
					status(0)
		{
			status |= ResourceStatus::DEFINED;
		}

		Resource(const Resource& other) : type(other.type),
										  #ifdef RHE_USE_RESOURCE_NAMES
											  __resourceName(other.__resourceName), 
										  #endif
										  status(other.status)

		{
			status |= ResourceStatus::COPIED;
		}

		Resource(Resource&& other) : type(std::move(other.type)), 
									 #ifdef RHE_USE_RESOURCE_NAMES
										 __resourceName(std::move(other.__resourceName)),
									 #endif
									 status(std::move(other.status)) 
		{
			status |= ResourceStatus::MOVED;
		}
		
		virtual ~Resource();

		Resource& operator= (const Resource& other) {
			if (&other == this)
				return *this;
			#ifdef RHE_USE_RESOURCE_NAMES
				__resourceName = other.__resourceName;
			#endif
			type = other.type;
			status = other.status;
			status |= ResourceStatus::COPIED;
			return *this;
		}

		Resource& operator= (Resource&& other) {
			#ifdef RHE_USE_RESOURCE_NAMES
				__resourceName = std::move(other.__resourceName);
			#endif
			type = std::move(other.type);
			status = std::move(other.status);
			status |= ResourceStatus::MOVED;
			return *this;
		}

		//A resource dependent implementation of safe resource loading.
		virtual inline bool Load() { 
			#ifdef DEBUG_RESOURCE
				DEBUG_OUT << "ERROR::RESOURCE::Load" << DEBUG_NEXT_LINE;
				DEBUG_OUT << "\tMessage: This function must not be called." << DEBUG_NEXT_LINE;
				#ifdef RHE_USE_RESOURCE_NAMES
					DEBUG_OUT << "\tResource name: " << __resourceName << DEBUG_NEXT_LINE;
				#endif
			#endif
			return false; 
		}

		//A resource dependent implementation of safe resource reloading.
		virtual inline bool Reload() { 
			#ifdef DEBUG_RESOURCE
				DEBUG_OUT << "ERROR::RESOURCE::Reload" << DEBUG_NEXT_LINE;
				DEBUG_OUT << "\tMessage: This function must not be called." << DEBUG_NEXT_LINE;
				#ifdef RHE_USE_RESOURCE_NAMES
					DEBUG_OUT << "\tResource name: " << __resourceName << DEBUG_NEXT_LINE;
				#endif
			#endif
			return false; 
		}

		//A resource dependent implementation of safe resource unloading.
		virtual inline bool Unload() { 
			#ifdef DEBUG_RESOURCE
				DEBUG_OUT << "ERROR::RESOURCE::Unload" << DEBUG_NEXT_LINE;
				DEBUG_OUT << "\tMessage: This function must not be called." << DEBUG_NEXT_LINE;
				#ifdef RHE_USE_RESOURCE_NAMES
					DEBUG_OUT << "\tResource name: " << __resourceName << DEBUG_NEXT_LINE;
				#endif
			#endif
			return false; 
		}

		//A resource dependent implementation of safe resource caching.
		virtual inline bool Cache(std::istream* const _cacheFile) {
			#ifdef DEBUG_RESOURCE
				#ifdef WARNINGS_RHE
					DEBUG_OUT << "WARNING::RESOURCE::Cache" << DEBUG_NEXT_LINE;
					DEBUG_OUT << "\tMessage: This function must not be called if 'canBeCached' is true." << DEBUG_NEXT_LINE;
					#ifdef RHE_USE_RESOURCE_NAMES
						DEBUG_OUT << "\tResource name: " << __resourceName << DEBUG_NEXT_LINE;
					#endif
					DEBUG_OUT << "\tCache flag: " << canBeCached << DEBUG_NEXT_LINE;
				#endif
			#endif
			return !canBeCached; 
		}

		//A resource dependent implementation of safe resource restoring from cached state.
		virtual inline bool Restore(std::ostream* const _restoreFile) {
			#ifdef DEBUG_RESOURCE
				#ifdef WARNINGS_RHE
					DEBUG_OUT << "WARNING::RESOURCE::Restore" << DEBUG_NEXT_LINE;
					DEBUG_OUT << "\tMessage: This function must not be called if 'canBeCached' is true." << DEBUG_NEXT_LINE;
					#ifdef RHE_USE_RESOURCE_NAMES
						DEBUG_OUT << "\tResource name: " << __resourceName << DEBUG_NEXT_LINE;
					#endif
					DEBUG_OUT << "\tCache flag: " << canBeCached << DEBUG_NEXT_LINE;
				#endif
			#endif
			return !canBeCached; 
		}

		//A resource dependent implementation of used memory counter, 1024 bytes used by default.
		virtual inline size_t usedMemory() { return 1024; }
	};
}
#endif
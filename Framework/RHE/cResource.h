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
//TODO::DELETE AFTER TEST!!!
#define DEBUG_RESOURCE
#ifdef DEBUG_RESOURCE
	#include <iostream>
#endif 
#ifdef RHE_USE_RESOURCE_NAMES
	#include <string>
#endif
#ifdef UNUSED_V006
	#include <fstream>
#endif
//OUR
#include "RHE\vResourceGeneral.h"
#include "general\vs2013tweaks.h"
//DEBUG
#if defined(DEBUG_RESOURCE) && !defined(OTHER_DEBUG)
	#include "general\mDebug.h"		
#elif defined(DEBUG_RESOURCE) && defined(OTHER_DEBUG)
	#include OTHER_DEBUG
#endif

namespace resources {

	#ifdef RHE_USE_RESOURCE_NAMES
		/**
		*	Runtime identification of naming of resources.
		**/
		CONST_OR_CONSTEXPR bool __RHEUseNames(true);
	#else
		/**
		*	Runtime identification of naming of resources.
		**/
		CONST_OR_CONSTEXPR bool __RHEUseNames(false);
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
		#ifdef UNUSED_V006
			/**
			*	Cache flag for derived classes.
			*	This flag must be set, if resorce can be cached to file and restored back.
			*	Derived class must implement Cache() and Restore() functions.
			**/
			bool canBeCached = false;
		#endif // UNUSED_V006
		#ifdef RHE_USE_RESOURCE_NAMES
			//String identificator of resource.
			std::string __resourceName;
		#endif

		/**
		*	\brief Read access to status from derived classes.
		*	\throw nothrow
		*	\return Value of status.
		**/
		int getStatus() const NOEXCEPT { return status; }

		/**
		*	\brief Setup the INVALID flag in resource status.
		*	Must be used in derived classes to specify that resource reached 
		*	some invalid state and must be released from current handler.
		*	\param[in]	_up	Specify the state of flag (true for up, false for down).
		*	\throw nothrow
		*	\return noreturn
		**/
		inline void invalidSignal(bool _up = true) NOEXCEPT { status &= (_up ? ResourceStatus::INVALID : ~ResourceStatus::INVALID); }

		/**
		*	\brief Setup the ALLOCBIG flag in resource status.
		*	Must be used in derived classes to specify resource allocation strategy.
		*	\param[in]	_strategy	Specify the strategy.
		*	\throw nothrow
		*	\return noreturn
		**/
		inline void allocSignal(allocateStrategy _strategy = allocateStrategy::BIG) NOEXCEPT {
			if (_strategy == allocateStrategy::BIG)	{
				status |= ResourceStatus::ALLOCBIG;
			} else {
				status &= ~ResourceStatus::ALLOCBIG;
			}
		}
	public:
		//Enumiration of all possible resource state flags.
		enum ResourceStatus : int {
			UNDEFINED	= 0x00,
			//Resource have defined Type
			DEFINED		= 0x01,
			//Load/Reload was called on resource 
			LOADED		= 0x02,
			//Cache function was called on resource
			CACHED		= 0x04,
			//Use of BIG allocation strategy
			ALLOCBIG	= 0x08,
			//Resource is invalid
			INVALID		= 0x10,
			//End of bits indicator
			MAX			= INVALID
		};

#ifdef RHE_USE_RESOURCE_NAMES
		Resource()	NOEXCEPT : 
					type(resources::ResourceType::UNKNOWN),
					__resourceName(""),
					status(ResourceStatus::UNDEFINED) {}

		Resource( std::string _name, ResourceType _type) NOEXCEPT : 
														 type(_type), 
														 __resourceName(_name), 
														 status(ResourceStatus::DEFINED) {}
#else
		Resource()	NOEXCEPT : 
					type(ResourceType::UNKNOWN),
					status(ResourceStatus::UNDEFINED) {}

		Resource(ResourceType _type) NOEXCEPT :
									type(_type),
									status(ResourceStatus::DEFINED) {}
#endif	// RHE_USE_RESOURCE_NAMES

		virtual ~Resource() NOEXCEPT {};

		Resource(const Resource& other) = default;

		Resource& operator= (const Resource& other) = default;

#ifdef MOVE_GENERATION
		Resource(Resource&& other) = default;

		Resource& operator= (Resource&& other) = default;
#else
		Resource(Resource&& other)  NOEXCEPT : 
									type(std::move(other.type)),
									#ifdef RHE_USE_RESOURCE_NAMES
										__resourceName(std::move(other.__resourceName)),
									#endif
									status(std::move(other.status)) {}

		Resource& operator= (Resource&& other) NOEXCEPT 
		{
			#ifdef RHE_USE_RESOURCE_NAMES
				__resourceName = std::move(other.__resourceName);
			#endif
			type = std::move(other.type);
			status = std::move(other.status);
			return *this;
		}
#endif	// MOVE_GENERATION

		//A resource dependent implementation of used memory counter.
		virtual inline size_t usedMemory() NOEXCEPT { return sizeof(Resource); }

		virtual inline allocateStrategy getAllocStrategy() { return (status & ResourceStatus::ALLOCBIG) ? allocateStrategy::BIG : allocateStrategy::SMALL; }

		//A resource dependent implementation of safe resource loading.
		virtual inline bool Load() { 
			#ifdef DEBUG_RESOURCE
				DEBUG_NEW_MESSAGE("ERROR::RESOURCE::Load")
					DEBUG_WRITE1("\tMessage: This function must not be called.");
					#ifdef RHE_USE_RESOURCE_NAMES
						DEBUG_WRITE2("\tResource name: ", __resourceName);
					#endif
				DEBUG_END_MESSAGE
			#endif
			return false; 
		}

		//A resource dependent implementation of safe resource unloading.
		virtual inline bool Unload() { 
			#ifdef DEBUG_RESOURCE
				DEBUG_NEW_MESSAGE("ERROR::RESOURCE::Unload")
					DEBUG_WRITE1("\tMessage: This function must not be called.");
					#ifdef RHE_USE_RESOURCE_NAMES
						DEBUG_WRITE2("\tResource name: ", __resourceName);
					#endif
				DEBUG_END_MESSAGE
			#endif
			return false; 
		}

		//A resource dependent implementation of safe resource reloading.
		virtual inline bool Reload() { 
			#ifdef DEBUG_RESOURCE
				DEBUG_NEW_MESSAGE("ERROR::RESOURCE::Reload")
					DEBUG_WRITE1("\tMessage: This function must not be called.");
					#ifdef RHE_USE_RESOURCE_NAMES
						DEBUG_WRITE2("\tResource name: ", __resourceName);
					#endif
				DEBUG_END_MESSAGE
			#endif
			return false; 
		}

		#ifdef UNUSED_V006
			//A resource dependent implementation of safe resource caching.
			virtual inline bool Cache(std::istream& _cacheFile) {
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
			virtual inline bool Restore(std::ostream& _restoreFile) {
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
		#endif // UNUSED_V006
	};
}
#endif
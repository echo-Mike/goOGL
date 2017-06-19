#ifndef RHE_H
#define RHE_H "[0.0.5@cResourceHandlingEngine.h]"
/*
*	DESCRIPTION:
*		Module contains implementation of resource handling engine.
*	AUTHOR:
*		Mikhail Demchenko
*		mailto:dev.echo.mike@gmail.com
*		https://github.com/echo-Mike
*/
//STD
#include <string>
#include <vector>
#include <set>
#include <map>
#include <memory>
#include <iostream>
#include <fstream>
#include <functional>
//GLEW
#include <GL\glew.h>
//ASSIMP

//OUR
#include "general\CIndexPool.h"
//DEBUG
#ifdef DEBUG_RHE
	#ifndef DEBUG_OUT
		#define DEBUG_OUT std::cout
	#endif
	#ifndef DEBUG_NEXT_LINE
		#define DEBUG_NEXT_LINE std::endl
	#endif
#endif

namespace resources {
	//Enumiration of all types of handled resources.
	enum ResourceType : int {
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
			//String identificator of resource
			std::string __resourceName;
		#endif
	public:
		//Enumiration of all possible resource states
		enum ResourceStatus : int {
			UNDEFINED	= 0x0000,
			//Resource have: Type, Id
			DEFINED		= 0x0001,
			//Load/Reload was called on resource 
			LOADED		= 0x0002,
			//Cache function was called on resource
			CACHED		= 0x0004,
			//Resource was copied from other resource
			COPIED		= 0x0100
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
									 status(std::move(other.status)) {}
		
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
		}

		Resource& operator= (Resource&& other) {
			if (&other == this)
				return *this;
			#ifdef RHE_USE_RESOURCE_NAMES
				__resourceName = std::move(other.__resourceName);
			#endif
			type = std::move(other.type);
			status = std::move(other.status);
		}

		//A resource dependent implementation of safe resource loading
		virtual inline bool Load() { 
			#ifdef DEBUG_RHE
				DEBUG_OUT << "ERROR::RESOURCE::Load" << DEBUG_NEXT_LINE;
				DEBUG_OUT << "\tMessage: This function must not be called." << DEBUG_NEXT_LINE;
				#ifdef RHE_USE_RESOURCE_NAMES
					DEBUG_OUT << "\tResource name: " << __resourceName << DEBUG_NEXT_LINE;
				#endif
			#endif
			return false; 
		}

		//A resource dependent implementation of safe resource reloading
		virtual inline bool Reload() { 
			#ifdef DEBUG_RHE
				DEBUG_OUT << "ERROR::RESOURCE::Reload" << DEBUG_NEXT_LINE;
				DEBUG_OUT << "\tMessage: This function must not be called." << DEBUG_NEXT_LINE;
				#ifdef RHE_USE_RESOURCE_NAMES
					DEBUG_OUT << "\tResource name: " << __resourceName << DEBUG_NEXT_LINE;
				#endif
			#endif
			return false; 
		}

		//A resource dependent implementation of safe resource unloading
		virtual inline bool Unload() { 
			#ifdef DEBUG_RHE
				DEBUG_OUT << "ERROR::RESOURCE::Unload" << DEBUG_NEXT_LINE;
				DEBUG_OUT << "\tMessage: This function must not be called." << DEBUG_NEXT_LINE;
				#ifdef RHE_USE_RESOURCE_NAMES
					DEBUG_OUT << "\tResource name: " << __resourceName << DEBUG_NEXT_LINE;
				#endif
			#endif
			return false; 
		}

		//A resource dependent implementation of safe resource caching
		virtual inline bool Cache(std::istream* const _cacheFile) {
			#ifdef DEBUG_RHE
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

		//A resource dependent implementation of safe resource restoring from cached state
		virtual inline bool Restore(std::ostream* const _restoreFile) {
			#ifdef DEBUG_RHE
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
	};

	/**
	*	DESCRIPTION
	*	Structure definition: CacheFile
	**/
	struct CacheFile {
		enum CacheFileState : int {
			CLOSED		= 0x0,
			OPEND		= 0x1,
			OPENTOREAD	= 0x3,
			OPENTOWRITE	= 0x5
		};
		//Pointer to file stream
		std::fstream* stream;
		//State of cache file
		int state;
		//Path to cache file
		std::string filePath;
	};

	/**
	*	DESCRIPTION
	*	Class definition: ResourceHandler
	**/
	class ResourceHandler {
		friend class ResourceHandlingEngine;
		//Define resource storage type
		typedef std::map<ResourceID, std::shared_ptr<Resource>> Storage;
		//Resource storage
		Storage storage;
		//Dinamyc array of cache files
		std::vector<CacheFile*> cacheFiles;
		//Enumiration of all possible resource handler states
		enum ResourceHandlerStatus : int {
			UNDEFINED,
			PUBLIC,
			PRIVATE
		};
		ResourceHandlerStatus status;
		
		ResourceHandler() : status(ResourceHandlerStatus::UNDEFINED) {}

		~ResourceHandler() {}

		int Cache(int _count) {

		}

		template < class T >
		inline std::shared_ptr<T> newResource(T&& _value, ResourceID _Id) {
			try { std::shared_ptr<T> _newptr(new T(std::move(_value))); }
			catch (const std::exception& e) { throw std::logic_error(e.what()); }
			catch (...) { throw std::logic_error("ERROR::RESOURCE_HANDLER::newResource::Object creation error."); }
			storage[_Id] = _newptr;
			return std::move(_newptr);
		}

		template < class T >
		inline std::shared_ptr<T> newResource(T* _valueptr, ResourceID _Id) {
			//Result of next two lines are the same, so less actions more performance.
			/*std::shared_ptr<Resource> _newptr((Resource*)_valueptr);*/
			std::shared_ptr<T> _newptr(_valueptr);
			storage[_Id] = _newptr;
			return std::move(_newptr);
		}

		template < class T >
		inline std::shared_ptr<T> setResource(T&& _value, ResourceID _Id) {
			//Result of next two lines are the same, so less actions more performance.
			/*storage.at(_Id).reset<Resource>((Resource*)(new T(std::move(_value))));*/
			storage.at(_Id).reset<T>(new T(std::move(_value)));
			//If exception isn't thrown by previous line then it safe to use operator[]
			return std::dynamic_pointer_cast<T>(storage[_Id]);
		}

		template < class T >
		inline std::shared_ptr<T> setResource(T* _valueptr, ResourceID _Id) {
			//Result of next two lines are the same, so less actions more performance.
			/*storage.at(_Id).reset<Resource>((Resource*)_valueptr);*/
			storage.at(_Id).reset<T>(_valueptr);
			//If exception isn't thrown by previous line then it safe to use operator[]
			return std::dynamic_pointer_cast<T>(storage[_Id]);
		}

		void deleteResource(ResourceID _Id) { storage.erase(_Id); }
	};

	#ifndef RHE_GLOBAL_MAX_RESOURCES
		#define RHE_GLOBAL_MAX_RESOURCES ((ResourceID)0xF4240)
	#endif

	#ifndef RHE_ALLOCATION_BANDWIDTH
		#define RHE_ALLOCATION_BANDWIDTH ((unsigned int)0x9C4)
	#endif

	/**
	*	Main singletone object of resource handling for all engine.
	*	Class definition: ResourceHandlingEngine
	**/
	class ResourceHandlingEngine : private Resource {
		SmartSimpleIndexPool<ResourceID> indexPool;
		
		typedef std::map<Resource*const, ResourceHandler*> HandlersStorage;
		HandlersStorage handlers;
		
	public:

		ResourceHandlingEngine(	ResourceID _maxId = RHE_GLOBAL_MAX_RESOURCES, 
								unsigned int _bandwidth = RHE_ALLOCATION_BANDWIDTH) : 
								indexPool(1, _maxId, _bandwidth)
		{
			handlers[this] = new ResourceHandler();
			handlers[this]->status = ResourceHandler::PUBLIC;
		}


		template < class T >
		bool newResource(T&& _value, Resource* _owner, std::shared_ptr<T>& _result) {
			try { 
				_result = std::move(handlers.at(_owner)->newResource<T>(std::move(_value), indexPool.newIndex())); 
				return true;
			}
			catch (const std::out_of_range& e) {
				#ifdef DEBUG_RHE
					DEBUG_OUT << "ERROR::RHE::newResource" << DEBUG_NEXT_LINE;
					if (std::string(e.what()).find("SMART_SIMPLE_INDEX_POOL") == std::string::npos) {
						DEBUG_OUT << "\tMessage: Owner not found." << DEBUG_NEXT_LINE;
					} else {
						DEBUG_OUT << "\tMessage: Allocation limit reached." << DEBUG_NEXT_LINE;
					}
				#endif
				return false;
			}
			catch (const std::logic_error& e) {
				#ifdef DEBUG_RHE
					DEBUG_OUT << "ERROR::RHE::newResource" << DEBUG_NEXT_LINE;
					DEBUG_OUT << "\tMessage: Error occurred during new object move-constructing." << DEBUG_NEXT_LINE;
					DEBUG_OUT << "\tException content:" << e.what() << DEBUG_NEXT_LINE;
				#endif
				return false;
			}
		}

		template < class T >
		bool newResource(T* _valueptr, Resource* _owner, std::shared_ptr<T>& _result) {
			try { 
				_result = std::move(handlers.at(_owner)->newResource<T>(_valueptr, indexPool.newIndex()));
				return true;
			}
			catch (const std::out_of_range& e) {
				#ifdef DEBUG_RHE
					DEBUG_OUT << "ERROR::RHE::newResource" << DEBUG_NEXT_LINE;
					if (std::string(e.what()).find("SMART_SIMPLE_INDEX_POOL") == std::string::npos) {
						DEBUG_OUT << "\tMessage: Owner not found." << DEBUG_NEXT_LINE;
					} else {
						DEBUG_OUT << "\tMessage: Allocation limit reached." << DEBUG_NEXT_LINE;
					}
				#endif
				return false;
			}
			catch (const std::logic_error& e) {
				#ifdef DEBUG_RHE
					DEBUG_OUT << "ERROR::RHE::newResource" << DEBUG_NEXT_LINE;
					DEBUG_OUT << "\tMessage: Error occurred during new object move-constructing." << DEBUG_NEXT_LINE;
					DEBUG_OUT << "\tException content:" << e.what() << DEBUG_NEXT_LINE;
				#endif
				return false;
			}
		}

		template < class T >
		bool newResource(T&& _value, std::shared_ptr<Resource> _owner, std::shared_ptr<T>& _result) {
			return newResource<T>(std::move(_value), _owner.get(), _result);
		}

		template < class T >
		std::shared_ptr<T> newResource(T* _valueptr, std::shared_ptr<Resource> _owner) {
			return std::move(newResource<T>(_valueptr, _owner.get()));
		}

		template < class T >
		std::shared_ptr<T> setResource(T&& _value, ResourceID _Id, Resource* _owner) {

		}

		template < class T >
		std::shared_ptr<T> setResource(T* _valueptr, ResourceID _Id, Resource* _owner) {

		}

		template < class T >
		std::shared_ptr<T> setResource(T&& _value, ResourceID _Id, std::shared_ptr<Resource> _owner) {
			return std::move(setResource<T>(std::move(_value), _Id, _owner.get()));
		}

		template < class T >
		std::shared_ptr<T> setResource(T* _valueptr, ResourceID _Id, std::shared_ptr<Resource> _owner) {
			return std::move(setResource<T>(_valueptr, _Id, _owner.get()));
		}

		void deleteResource(ResourceID _Id, Resource* _owner) {

		}

		void deleteResource(ResourceID _Id, std::shared_ptr<Resource> _owner) {	deleteResource(_Id, _owner.get()); }
	};

}
#endif
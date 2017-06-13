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
#include <map>
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
	//Enumiration of all types of handled resources
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

	//Unsigned integer type used as resources identificator
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
		typedef std::map<ResourceID, Resource*> Storage;
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

		~ResourceHandler() {
			for (auto &v : storage)
				delete v.second;
		}

		int Cache(int _count) {

		}

		template < class T >
		T* const inline newResource(T&& _value, ResourceID _Id) { 
			storage[_Id] = new T(std::move(_value));
			return dynamic_cast<T*>(storage[_Id]);
		}

		template < class T >
		T* const inline newResource(T* _valueptr, ResourceID _Id) {
			storage[_Id] = new T(std::move(*_valueptr));
			return dynamic_cast<T*>(storage[_Id]);
		}

		template < class T >
		T* const inline setResource(T&& _value, ResourceID _Id) {
			delete (dynamic_cast<T*>(storage[_Id]));
			storage[_Id] = new T(std::move(_value));
			return dynamic_cast<T*>(storage[_Id]);
		}

		template < class T >
		T* const inline setResource(T* _valueptr, ResourceID _Id) {
			delete (dynamic_cast<T*>(storage[_Id]));
			storage[_Id] = new T(std::move(*_valueptr));
			return dynamic_cast<T*>(storage[_Id]);
		}

		void deleteResource(ResourceID _Id) {
			delete (Storage::mapped_type)storage[_Id];
			storage.erase(_Id);
		}
	};

	#define RHE_GLOBAL_MAX_RESOURCES ((ResourceID)0x40000)

	/**
	*	Main singletone object of resource handling for all engine.
	*	Class definition: ResourceHandlingEngine
	**/
	class ResourceHandlingEngine : private Resource {
		SimpleIndexPool<ResourceID> indexPool;
		typedef std::map<Resource*const, ResourceHandler*> HandlersStorage;
		HandlersStorage handlers;
	public:
		ResourceHandlingEngine() : indexPool(1, RHE_GLOBAL_MAX_RESOURCES) 
		{
			handlers[this] = new ResourceHandler();
		}

		template < class T >
		void newResource(T&& _value, Resource* _owner) {

		}

		template < class T >
		void newResource(T* _valueptr, Resource* _owner) {

		}

		template < class T >
		void setResource(T&& _value, ResourceID _Id, Resource* _owner) {

		}

		template < class T >
		void setResource(T* _valueptr, ResourceID _Id, Resource* _owner) {

		}

		void deleteResource() {

		}

	};
}
#endif
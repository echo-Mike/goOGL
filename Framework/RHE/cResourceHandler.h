#ifndef RESOURCEHANDLER_H
#define RESOURCEHANDLER_H "[0.0.5@cResourceHandler.h]"
/**
*	DESCRIPTION:
*		Module contains implementation of internal resource handler class.
*	AUTHOR:
*		Mikhail Demchenko
*		mailto:dev.echo.mike@gmail.com
*		https://github.com/echo-Mike
**/
//STD
#include <map>
#include <memory>
#ifdef DEBUG_RESOURCEHANDLER
	#include <iostream>
#endif
#ifdef UNUSED_V006
	#include <vector>
	#include <fstream>
	#include <sstream>
	#include <cstdio>
	#include <iterator>
#endif
//OUR
#include "RHE\vResourceGeneral.h"
#include "RHE\cResource.h"
#ifdef UNUSED_V006
	#include "RHE\cCacheFile.h"
#endif
//DEBUG
//#include "../Model Loading/goOGL13/src/debug.h"
//#define DEBUG_RESOURCEHANDLER
//#define RESOURCEHANDLER_MINOR_ERRORS
//#define RESOURCE_HANDLER_STRICT
#ifdef DEBUG_RESOURCEHANDLER
	#ifndef DEBUG_OUT
		#define DEBUG_OUT std::cout
	#endif
	#ifndef DEBUG_NEXT_LINE
		#define DEBUG_NEXT_LINE "\n"
	#endif
#endif

namespace resources {

	#ifdef UNUSED_V006
		#ifndef RH_CAHCE_UNIQUE_OPEN
			/**
			*	Cache files resource identifier open pattern.
			*	$ - will be replaced with resource id.
			**/
			#define RH_CAHCE_UNIQUE_OPEN "[(@@@$@@@]"
		#endif

		#ifndef RH_CAHCE_UNIQUE_CLOSE
			/**
			*	Cache files resource identifier close pattern.
			*	$ - will be replaced with resource id.
			**/
			#define RH_CAHCE_UNIQUE_CLOSE "[@@@$@@@)]"
		#endif
	#endif 

	#ifdef RESOURCE_HANDLER_STRICT
		/**
		*	Runtime identification of Resource Handler strict mode.
		**/
		CONST_OR_CONSTEXPR bool __RHStrictMode(true);
	#else
		/**
		*	Runtime identification of Resource Handler strict mode.
		**/
		CONST_OR_CONSTEXPR bool __RHStrictMode(false);
	#endif

	/**
	*	Forward declaration of ResourceHandlingEngine class.
	**/
	class ResourceHandlingEngine;

	/**
	*	Class that represents resource storage for one scene.
	*	This is a helper class with API open only to ResourceHandlingEngine.
	*	Class have two modes NORMAL and STRICT defined in compile-time.
	*	Class definition: ResourceHandler
	**/
	class ResourceHandler final {
		friend class ResourceHandlingEngine;
		//Define resource storage type
		using Storage = std::map<ResourceID, std::shared_ptr<Resource>>;
		//Resource storage
		Storage storage;
		//Owner RHE of current handler
		ResourceHandlingEngine* owner;

		#ifdef UNUSED_V006
			//Dinamyc array of cache files
			std::vector<std::shared_ptr<CacheFile>> cacheFiles;
		#endif // UNUSED_V006

		//Enumiration of all possible resource handler states
		enum class ResourceHandlerStatus {
			UNDEFINED,
			PUBLIC,
			PRIVATE
		};
		ResourceHandlerStatus status{ ResourceHandlerStatus::UNDEFINED };

		ResourceHandler(const ResourceHandlerStatus _status, ResourceHandlingEngine* _owner) : status(_status), owner(_owner)
		{
			#ifdef UNUSED_V006
				cacheFiles.push_back(std::move(std::shared_ptr<CacheFile>(new CacheFile())));
			#endif // UNUSED_V006
		}

		~ResourceHandler() NOEXCEPT { owner = nullptr; }

		/**
		*	\brief Counts amount of handled memory.
		*	Corrects possible uint overflow with '_cary' parameter.
		*	Linear complexity.
		*	\param[out]	_cary	Carry uint to correct uint overflow.
		*	\throw Ignore
		*	\return Summed up amount of memory used by handled objects.
		**/
		unsigned int memoryHandled(unsigned int& _cary) {
			_cary = 0;
			unsigned int _result = 0;
			unsigned int _buff = 0;
			for (auto& v : storage) {
				_buff = v.second->usedMemory();
				if (_result + _buff < _result)
					_cary++;
				_result += _buff;
			}
		}

		/**
		*	\brief Flags to be used in checkResource.
		**/
		enum ResourceCheckFlags : int {
			//Resource internal states:: (next <br> is intentional)

			//Check that resource is defined 
			DEFINED		= Resource::ResourceStatus::DEFINED,
			//Check that resource is loaded or reloaded
			LOADED		= Resource::ResourceStatus::LOADED,
			//Check that resource is unloaded 
			UNLOADED	= Resource::ResourceStatus::MAX << 1,
			//UNUSED : Check that resource was cached
			CACHED		= Resource::ResourceStatus::CACHED,
			//Check that resource is valid
			VALID		= Resource::ResourceStatus::INVALID,
			//Resource states in storage:: (next <br> is intentional)

			//Check that resource is presented in current storage
			PRESENTED	= Resource::ResourceStatus::MAX << 2,
			//Complex checks:: (next <br> is intentional)

			//Check that resource defined and loaded
			DEFLOAD		= DEFINED | LOADED,
			//Check that resource defined and unloaded
			DEFULOAD	= DEFINED | UNLOADED,
			//Check that resource is presented and valid
			PRESVAL		= PRESENTED | VALID,
			//PRESVAL and DEFLOAD checks
			PVDL		= PRESVAL | DEFLOAD,
			//PRESVAL and DEFULOAD checks
			PVDUL		= PRESVAL | DEFULOAD
		};

		/**
		*	\brief Check resource status to satisfy certain flag arrangement with special way.
		*	Check ResourceCheckFlags::PRESVAL every time.
		*	Other checks performd in ANY OF manner.
		*	Resource with INVALID status will be deleted after check.
		*	\param[in]	_Id		Identificator of resource.
		*	\param[in]	_flags	Flags to check.
		*	\throw nothrow
		*	\return Result of check
		**/
		bool checkResource(const ResourceID _Id, int _flags = ResourceCheckFlags::PRESVAL) NOEXCEPT{
			//If object is presented performs other checks
			auto _iterator = storage.find(_Id);
			if (_iterator != storage.end())	{
				//Unset PRESENTED flag in input flags
				_flags &= ~ResourceCheckFlags::PRESENTED;
				//Only PRESENTED check is needed
				if (!_flags)
					return true;
				//Obtain status of resource
				auto _status = _iterator->second->status;
				//Other checks performed only if resource is valid
				if (!(_status & ResourceCheckFlags::VALID)) {
					//Unset INVALID flag in input flags
					_flags &= ~ResourceCheckFlags::VALID;
					//Only PRESVAL check
					if (!_flags)
						return true;
					//Perform unusual checks
					switch (_flags)	{
						case ResourceCheckFlags::DEFLOAD:
							return _status & ResourceCheckFlags::DEFINED && _status & ResourceCheckFlags::LOADED;
							break;
						case ResourceCheckFlags::DEFULOAD:
							return _status & ResourceCheckFlags::DEFINED && !(_status & ResourceCheckFlags::LOADED);
							break;
						default:
							return _status & _flags;
							break;
					}					
				}
				//Delete resource if it is invalid
				//Hardcoded solution
				forceDelete(_Id);
			}
			return false;
		}

		template < class T >
		/**
		*	\brief Constructs new resource with id '_Id' of type "T" by calling default constructor.
		*	Actually calls operator new : new T().
		*	NORMAL : Erase object with id '_Id' if it presented in current time before creating new one.
		*	STRICT : Returns an shared pointer to nullptr if object with id '_Id' already exists. Returns without calling constructor.
		*	\param[in]	_Id		Identificator of new object.
		*	\param[in]	_defptr	Parameter to make template overload possible.
		*	\throw std::logic_error On exception in object default constructor or operator new.
		*	\return Shared pointer of type "T" to new object on success and to nullptr on error.
		**/
		inline std::shared_ptr<T> newResource(ResourceID _Id, T* _defptr = nullptr) {
			//Find object with id '_Id'
			if (storage.count(_Id)) {
				#ifdef RESOURCE_HANDLER_STRICT
					#ifdef DEBUG_RESOURCEHANDLER
						DEBUG_NEW_MESSAGE("ERROR::RESOURCE_HANDLER::newResource")
							DEBUG_WRITE3("\tMessage: Object with '_Id': ", _Id, " already exists.");
						DEBUG_END_MESSAGE
					#endif
					return std::shared_ptr<T>(nullptr);
				#endif
				storage.erase(_Id);
			}
			//Try to default construct new object
			try { std::shared_ptr<T> _newptr(new T()); }
			catch (const std::exception& e) { throw std::logic_error(e.what()); } //Warp up external exception to std::logic_error
			catch (...) { throw std::logic_error("ERROR::RESOURCE_HANDLER::newResource::Object creation error."); } //Provide any other throw with std::logic_error
			//Insert new object to storage
			storage[_Id] = _newptr;
			return std::move(_newptr);
		}

		template < class T >
		/**
		*	\brief Performs an attempt to create '_count' objects by calling newResource with default constructor.
		*	NORMAL : Increment result on every successful (nothrow) creation.
		*	STRICT : Increment result only on non nullptr return of newResource.
		*	\param[in]	_Id			Array of identificators.
		*	\param[out]	_result		Array of shared pointers to new objects.
		*	\param[in]	_count		Count of objects to be created.
		*	\param[out]	_success	[Optional] Per object status of successful creation.
		*	\throw Ignore : exception may occur if _Id or _result not long enougth.
		*	\return Count of successfully allocated objects.
		**/
		inline unsigned int newResource(ResourceID _Id[], std::shared_ptr<T> _result[], unsigned int _count, bool _success[] = nullptr) {
			if (!_count)
				return _count;
			if (!_result)
				return 0;
			//Counter of allocated objects
			unsigned int _allocated = 0; 
			for (unsigned int _index = 0; _index < _count; _index++) {
				try { _result[_index] = newResource<T>(_Id[_index]); }
				catch (...) { 
					if (_success)
						_success[_index] = false;
					continue;
				}
				#ifdef RESOURCE_HANDLER_STRICT
					if (_result[_index]) {
						if (_success)
							_success[_index] = true;
						_allocated++;
					} else {
						if (_success)
							_success[_index] = false;
					}
				#else
					if (_success)
						_success[_index] = true;
					_allocated++;
				#endif
			}
			return _allocated;
		}

		template < class T >
		/**
		*	\brief Constructs new resource of type "T" by move-constructing from '_value' with id '_Id'.
		*	NORMAL : Erase object with id '_Id' if it presented in current time before creating new one.
		*	STRICT : Returns an shared pointer to nullptr if object with id '_Id' already exists. Returns without calling move-constructor.
		*	\param[in]	_value	Move reference to object.
		*	\param[in]	_Id		Identificator of new object.
		*	\throw std::logic_error On exception in object move-constructor or operator new.
		*	\return Shared pointer of type "T" to new object on success and to nullptr on error.
		**/
		inline std::shared_ptr<T> newResource(T&& _value, ResourceID _Id) {
			//Find object with id '_Id'
			if (storage.count(_Id)) {
				#ifdef RESOURCE_HANDLER_STRICT
					#ifdef DEBUG_RESOURCEHANDLER
						DEBUG_NEW_MESSAGE("ERROR::RESOURCE_HANDLER::newResource")
							DEBUG_WRITE3("\tMessage: Object with '_Id': ", _Id, " already exists.");
						DEBUG_END_MESSAGE
					#endif
					return std::shared_ptr<T>(nullptr);
				#endif
				storage.erase(_Id);
			}
			//Try to move-construct new object
			try { std::shared_ptr<T> _newptr(new T(std::move(_value))); }
			catch (const std::exception& e) { throw std::logic_error(e.what()); } //Warp up external exception to std::logic_error
			catch (...) { throw std::logic_error("ERROR::RESOURCE_HANDLER::newResource::Object creation error."); } //Provide any other throw with std::logic_error
			//Insert new object to storage
			storage[_Id] = _newptr;
			return std::move(_newptr);
		}

		template < class T >
		/**
		*	\brief Takes ownership of resource with type "T" located by pointer '_valueptr' as resource with id '_Id'.
		*	NORMAL : Erase object with id '_Id' if it presented in current time.
		*			 Accepts nullptr objects.
		*	STRICT : Returns an shared pointer to nullptr if object with id '_Id' already exists.
		*			 Doesn't accepts nullptr objects.
		*	\param[in]	_valueptr	Pointer to resource.
		*	\param[in]	_Id			Identificator of new object.
		*	\throw nothrow
		*	\return Shared pointer of type "T" to new object on success and to nullptr on error.
		**/
		inline std::shared_ptr<T> newResource(T* _valueptr, ResourceID _Id) {
			#ifdef RESOURCE_HANDLER_STRICT
				if (!_valueptr) {
					#ifdef DEBUG_RESOURCEHANDLER
						DEBUG_NEW_MESSAGE("ERROR::RESOURCE_HANDLER::newResource")
							DEBUG_WRITE1("\tMessage: '_valueptr' is a nullptr." );
						DEBUG_END_MESSAGE
					#endif
					return std::shared_ptr<T>(nullptr);
				}
			#endif
			//Find object with id '_Id'
			if (storage.count(_Id)) {
				#ifdef RESOURCE_HANDLER_STRICT
					#ifdef DEBUG_RESOURCEHANDLER
						DEBUG_NEW_MESSAGE("ERROR::RESOURCE_HANDLER::newResource")
							DEBUG_WRITE3("\tMessage: Object with '_Id': ", _Id, " already exists.");
						DEBUG_END_MESSAGE
					#endif
					return std::shared_ptr<T>(nullptr);
				#endif
				storage.erase(_Id);
			}
			//Result of next two lines are the same, so less actions more performance.
			/*std::shared_ptr<Resource> _newptr((Resource*)_valueptr);*/
			std::shared_ptr<T> _newptr(_valueptr);
			storage[_Id] = _newptr;
			return std::move(_newptr);
		}

		template < class T >
		/**
		*	\brief Performs an attempt to create '_count' objects by calling newResource on copy of '_value'.
		*	NORMAL : Increment result on every successful (nothrow) creation.
		*	STRICT : Increment result only on non nullptr return of newResource.
		*	\param[in]	_value		Constant reference to be used in copy construction of new objects.
		*	\param[in]	_Id			Array of identificators.
		*	\param[out]	_result		Array of shared pointers to new objects.
		*	\param[in]	_count		Count of objects to be created.
		*	\param[out]	_success	[Optional] Per object status of successful creation.
		*	\throw nothrow
		*	\return Count of successfully allocated objects.
		**/
		inline unsigned int newResource(const T& _value, ResourceID _Id[], std::shared_ptr<T> _result[], unsigned int _count, bool _success[] = nullptr) {
			if (!_count)
				return _count;
			//Counter of allocated objects
			unsigned int _allocated = 0; 
			for (unsigned int _index = 0; _index < _count; _index++) {
				try { _result[_index] = newResource<T>(std::move(T(_value)), _Id[_index]); }
				catch (...) { 
					if (_success)
						_success[_index] = false;
					continue;
				}
				#ifdef RESOURCE_HANDLER_STRICT
					if (_result[_index]) {
						if (_success)
							_success[_index] = true;
						_allocated++;
					} else {
						if (_success)
							_success[_index] = false;
					}
				#else
					if (_success)
						_success[_index] = true;
					_allocated++;
				#endif
			}
			return _allocated;
		}

		template < class T >
		/**
		*	\brief Performs an attempt to create '_count' objects by calling newResource on copy of deferenced '_valueptr'.
		*	NORMAL : Increment result on every successful (nothrow) creation.
		*	STRICT : Increment result only on non nullptr return of newResource.
		*	\param[in]	_valueptr	Constant pointer to the resource to be copied.
		*	\param[in]	_Id			Array of identificators.
		*	\param[out]	_result		Array of shared pointers to new objects.
		*	\param[in]	_count		Count of objects to be created.
		*	\param[out]	_success	[Optional] Per object status of successful creation.
		*	\throw nothrow
		*	\return Count of successfully allocated objects.
		**/
		inline unsigned int newResource(const T* _valueptr, ResourceID _Id[], std::shared_ptr<T> _result[], unsigned int _count, bool _success[] = nullptr) {
			return newResource(*_valueptr, _Id, _result, _count, _success);
		}

		template < class T >
		/**
		*	\brief Performs an attempt to copy-construct new object with id '_Id' from object with id '_sourceId' and type "T".
		*	If object '_sourceId' doesn't exist return shared pointer to nullptr.
		*	NORMAL : Erase object with id '_Id' if it presented in current time before creating new one.
		*	STRICT : Returns an shared pointer to nullptr if object with id '_Id' already exists. Returns without calling copy-constructor.
		*	\param[in]	_sourceId	Identifier of the object from which the copy is made.
		*	\param[in]	_Id			New object identificator.
		*	\param[in]	_defptr		Parameter to make template overload possible.
		*	\throw std::logic_error On exception in object copy-constructor or operator new or dynamic_pointer_cast error.
		*	\return Shared pointer of type "T" to new object on success and to nullptr on error.
		**/
		inline std::shared_ptr<T> newCopy(ResourceID _sourceId, ResourceID _Id, T* const _defptr = nullptr) {
			if (storage.count(_sourceId)) {
				if (storage.count(_Id)) {
					#ifdef RESOURCE_HANDLER_STRICT
						#ifdef DEBUG_RESOURCEHANDLER
							DEBUG_NEW_MESSAGE("ERROR::RESOURCE_HANDLER::newCopy")
								DEBUG_WRITE3("\tMessage: Object with '_Id': ", _Id, " already exists.");
							DEBUG_END_MESSAGE
						#endif
						return std::shared_ptr<T>(nullptr);
					#endif
					storage.erase(_Id);
				}
				try { std::shared_ptr<T> _newptr(new T(*(std::dynamic_pointer_cast<T>(storage[_sourceId])))); }
				catch (const std::exception& e) { throw std::logic_error(e.what()); }
				catch (...) { throw std::logic_error("ERROR::RESOURCE_HANDLER::newCopy::Object creation error."); }
				storage[_Id] = _newptr;
				return std::move(_newptr);
			} else {
				#ifdef DEBUG_RESOURCEHANDLER
					DEBUG_NEW_MESSAGE("ERROR::RESOURCE_HANDLER::newCopy")
						DEBUG_WRITE2("\tMessage: Invalid '_sourceId': ", _sourceId);
					DEBUG_END_MESSAGE
				#endif
				return std::shared_ptr<T>(nullptr);
			}
		}

		template < class T >
		/**
		*	\brief Performs an attempt to copy-construct new object from object with id '_sourceId' in place of object with id '_destId'.
		*	If object '_sourceId' or '_destId' doesn't exist return shared pointer to nullptr.
		*	NORMAL/STRICT : Performs check on '_sourceId' and '_destId' objects, then creates new one.
		*	\param[in]	_sourceId	Identifier of the object from which the copy is made.
		*	\param[in]	_destId		Identifier of object rewritten with new copy.
		*	\param[in]	_defptr		Parameter to make template overload possible.
		*	\throw std::logic_error On exception in object copy-constructor or operator new or dynamic_pointer_cast error.
		*	\return Shared pointer of type "T" to new object on success and to nullptr on error.
		**/
		inline std::shared_ptr<T> copyResource(ResourceID _sourceId, ResourceID _destId, T* const _defptr = nullptr) {
			if (storage.count(_sourceId)) {
				if (!storage.erase(_destId)) {
					#ifdef DEBUG_RESOURCEHANDLER
						DEBUG_NEW_MESSAGE("ERROR::RESOURCE_HANDLER::copyResource")
							DEBUG_WRITE3("\tMessage: Object with '_destId': ", _destId, " doesn't exists.");
						DEBUG_END_MESSAGE
					#endif
					return std::shared_ptr<T>(nullptr);
				}
				try { std::shared_ptr<T> _newptr(new T(*(std::dynamic_pointer_cast<T>(storage[_sourceId])))); }
				catch (const std::exception& e) { throw std::logic_error(e.what()); }
				catch (...) { throw std::logic_error("ERROR::RESOURCE_HANDLER::copyResource::Object creation error."); }
				storage[_destId] = _newptr;
				return std::move(_newptr);
			} else {
				#ifdef DEBUG_RESOURCEHANDLER
					DEBUG_NEW_MESSAGE("ERROR::RESOURCE_HANDLER::copyResource")
						DEBUG_WRITE2("\tMessage: Invalid '_sourceId': ", _sourceId);
					DEBUG_END_MESSAGE
				#endif
				return std::shared_ptr<T>(nullptr);
			}
		}

		template < class T >
		/**
		*	\brief Moves resource '_sourceId' to place of resource '_destId'.
		*	Performs erase of '_destId' before moving and erase of '_sourceId' after.
		*	NORMAL : Erase object with id '_Id' if it presented in current time before creating new one.
		*	STRICT : Returns an shared pointer to nullptr if object with id '_Id' already exists. Returns without calling copy-constructor.
		*	\param[in]	_sourceId	Identifier of the object being moved.
		*	\param[in]	_destId		Identifier of overwritten object.
		*	\param[in]	_defptr		Parameter to make template overload possible.
		*	\throw nothrow
		*	\return Shared pointer of type "T" to moved resource on success and to nullptr on error.
		**/
		inline std::shared_ptr<T> moveResource(ResourceID _sourceId, ResourceID _destId, T* const _defptr = nullptr) {
			if (storage.count(_sourceId)) {
				if (!storage.erase(_destId)) {
					#ifdef RESOURCE_HANDLER_STRICT
						#ifdef DEBUG_RESOURCEHANDLER
							DEBUG_NEW_MESSAGE("ERROR::RESOURCE_HANDLER::moveResource")
								DEBUG_WRITE2("\tMessage: Invalid '_destId': ", _destId);
							DEBUG_END_MESSAGE
						#endif
						return std::shared_ptr<T>(nullptr);
					#endif
				}
				storage[_destId] = storage[_sourceId];
				storage.erase(_sourceId);
				return std::shared_ptr<T>(storage[_destId]);
			} else {
				#ifdef DEBUG_RESOURCEHANDLER
					DEBUG_NEW_MESSAGE("ERROR::RESOURCE_HANDLER::moveResource")
						DEBUG_WRITE2("\tMessage: Invalid '_sourceId': ", _sourceId);
					DEBUG_END_MESSAGE
				#endif
				return std::shared_ptr<T>(nullptr);
			}
		}

		template < class T >
		/**
		*	\brief Setups new resource by move-constructing from '_value' in place of object with id '_Id'.
		*	NORMAL : If object with id '_Id' not presented - calls newResource else resets saved pointer to own new object 
		*			 of type "T" created with move-construction from '_value'.
		*	STRICT : Doesn't call existence check on object with id '_Id'. If it doesn't exists throws exception.
		*	\param[in]	_value	Move reference to object.
		*	\param[in]	_Id		Identificator of new object.
		*	\throw std::logic_error On exception in object move-constructor or operator new.
		*	\throw std::out_of_range In STRICT mode if object with id '_Id' doesn't exist.
		*	\return Shared pointer of type "T" to new object resource on success and to nullptr on error.
		**/
		inline std::shared_ptr<T> setResource(T&& _value, ResourceID _Id) {
			#ifndef RESOURCE_HANDLER_STRICT
				if (!storage.count(_Id)) {
					try { return newResource<T>(std::move(_value), _Id); }
					catch (...) { throw; }
				}
			#endif
			//Result of next two lines are the same, so less actions more performance.
			/*storage.at(_Id).reset<Resource>((Resource*)(new T(std::move(_value))));*/
			try { storage.at(_Id).reset<T>(new T(std::move(_value))); }
			catch (const std::out_of_range& e) { throw; }
			catch (const std::exception& e) { throw std::logic_error(e.what()); }
			catch (...) { throw std::logic_error("ERROR::RESOURCE_HANDLER::setResource::Object creation error."); }
			//If exception isn't thrown by previous line then it safe to use operator[]
			return std::dynamic_pointer_cast<T>(storage[_Id]);
		}

		template < class T >
		/**
		*	\brief Takes ownership of resource with type "T" located by pointer '_valueptr'.
		*	Resource with id '_Id' is overwritten with newly obtained object.
		*	NORMAL : If object with id '_Id' not presented - calls newResource else resets saved pointer to own object pointed by '_valueptr'.
		*			 Accepts nullptr objects.
		*	STRICT : Doesn't call existence check on object with id '_Id'. If it doesn't exists throws exception.
		*			 Doesn't accepts nullptr objects.
		*	\param[in]	_valueptr	Pointer to resource.
		*	\param[in]	_Id			Identificator of new object.
		*	\throw std::out_of_range In STRICT mode if object with id '_Id' doesn't exist.
		*	\return Shared pointer of type "T" to new object resource on success and to nullptr on error.
		**/
		inline std::shared_ptr<T> setResource(T* _valueptr, ResourceID _Id) {
			#ifdef RESOURCE_HANDLER_STRICT
				if (!_valueptr) {
					#ifdef DEBUG_RESOURCEHANDLER
						DEBUG_NEW_MESSAGE("ERROR::RESOURCE_HANDLER::setResource")
							DEBUG_WRITE1("\tMessage: '_valueptr' is a nullptr.");
						DEBUG_END_MESSAGE
					#endif
					return std::shared_ptr<T>(nullptr);
				}
			#endif
			#ifndef RESOURCE_HANDLER_STRICT
				if (!storage.count(_Id)) {
					try { return newResource<T>(_valueptr, _Id); }
					catch (...) { throw; }
				}
			#endif
			//Result of next two lines are the same, so less actions more performance.
			/*storage.at(_Id).reset<Resource>((Resource*)_valueptr);*/
			try { storage.at(_Id).reset<T>(_valueptr); }
			catch (const std::out_of_range& e) { throw; }
			//If exception isn't thrown by previous line then it safe to use operator[]
			return std::dynamic_pointer_cast<T>(storage[_Id]);
		}

		template < class T >
		/**
		*	\brief Performs an attempt to setup '_count' objects by calling setResource on copy of '_value'.
		*	NORMAL : Increment result on every successful (nothrow) creation.
		*	STRICT : Increment result only on non nullptr return of newResource.
		*	\param[in]	_value		Constant reference to be used in copy construction of new objects.
		*	\param[in]	_Id			Array of identificators of objects that will be replaced.
		*	\param[out]	_result		Array of shared pointers to new objects.
		*	\param[in]	_count		Count of objects to be created.
		*	\param[out]	_success	[Optional] Per object status of successful creation.
		*	\throw nothrow
		*	\return Count of successfully allocated objects.
		**/
		inline unsigned int setResource(const T& _value, ResourceID _Id[], std::shared_ptr<T> _result[], unsigned int _count, bool _success[] = nullptr) {
			if (!_count)
				return 0;
			unsigned int _allocated = 0;
			for (unsigned int _index = 0; _index < _count; _index++) {
				try { _result[_index] = setResource<T>(std::move(T(_value)), _Id[_index]); }
				catch (...) { 
					if (_success)
						_success[_index] = false;
					continue;
				}
				#ifdef RESOURCE_HANDLER_STRICT
					if (_result[_index]) {
						if (_success)
							_success[_index] = true;
						_allocated++;
					} else {
						if (_success)
							_success[_index] = false;
					}
				#else
					if (_success)
						_success[_index] = true;
					_allocated++;
				#endif
			}
			return _allocated;
		}

		template < class T >
		/**
		*	\brief Performs an attempt to setup '_count' objects by calling setResource on copy of deferenced '_valueptr'.
		*	NORMAL : Increment result on every successful (nothrow) creation.
		*	STRICT : Increment result only on non nullptr return of newResource.
		*	\param[in]	_valueptr	Constant pointer to the resource to be copied.
		*	\param[in]	_Id			Array of identificators.
		*	\param[out]	_result		Array of shared pointers to new objects.
		*	\param[in]	_count		Count of objects to be created.
		*	\param[out]	_success	[Optional] Per object status of successful creation.
		*	\throw nothrow
		*	\return Count of successfully allocated objects.
		**/
		inline unsigned int setResource(const T* _valueptr, ResourceID _Id[], std::shared_ptr<T> _result[], unsigned int _count, bool _success[] = nullptr) {
			return setResource(*_valueptr, _Id, _result, _count, _success);
		}

		template < class T >
		/**
		*	\brief Performs an attempt to get shared pointer to resource with id '_Id'.
		*	NORMAL/STRICT : Performs dynamic cast on copy of pointer stored under id '_Id'.
		*	\param[in]	_Id		Identificator of stored object.
		*	\param[in]	_defptr	Parameter to make template overload possible.
		*	\throw nothrow
		*	\return Shared pointer to object with id '_Id' or to nullptr on error.
		**/
		inline std::shared_ptr<T> getResource(ResourceID _Id, T* const _defptr = nullptr) {
			try { return std::dynamic_pointer_cast<T>(storage.at(_Id)); }
			catch (const std::out_of_range& e) {
				#ifdef DEBUG_RESOURCEHANDLER
					DEBUG_NEW_MESSAGE("ERROR::RESOURCE_HANDLER::getResource")
						DEBUG_WRITE3("\tMessage: Identifier '_Id': ", _Id, " doesn't exist.");
					DEBUG_END_MESSAGE
				#endif
			}
			catch (...) {
				#ifdef DEBUG_RESOURCEHANDLER
					DEBUG_NEW_MESSAGE("ERROR::RESOURCE_HANDLER::getResource")
						DEBUG_WRITE1("\tMessage: Unknown error.");
					DEBUG_END_MESSAGE
				#endif
			}
			return std::shared_ptr<T>(nullptr);
		}

		/**
		*	\brief Safely delete stored resource.
		*	NORMAL : Erases stored under id '_Id' shared pointer from storage.
		*	STRICT : Erases if and only if object with id '_Id' is presented and handled only by current instance of shared pointer.
		*	\param[in]	_Id	Identificator of resource to be deleted.
		*	\throw nothrow
		*	\return True on successfull deletion, false on else.
		**/
		inline bool deleteResource(const ResourceID _Id) { 
			#ifdef RESOURCE_HANDLER_STRICT
				if (storage.count(_Id)) {
					if (storage[_Id].use_count() > 1) {
						#ifdef DEBUG_RESOURCEHANDLER
							DEBUG_NEW_MESSAGE("ERROR::RESOURCE_HANDLER::deleteResource")
								DEBUG_WRITE1("\tMessage: Only handler must own a resource to release it.");
							DEBUG_END_MESSAGE
						#endif
						return false;
					}
					return false;
				}
			#endif	
			storage.erase(_Id);
			return true;
		}

		/**
		*	\brief Force removal of resource with id '_Id'.
		*	NORMAL/STRICT : Ignores all strict rules for deleteResource.
		*	This function is for internal use only.
		*	\param[in]	_Id	Identificator of resource to be deleted.
		*	\throw nothrow
		*	\return Nothing
		**/
		void forceDelete(const ResourceID _Id) NOEXCEPT { 
			owner->secureRemove(_Id, this);
			storage.erase(_Id); 
		}

		/**
		*	\brief Performs an attempt to call Load function of resource with id '_Id'.
		*	NORMAL : Performs checks that resource is presented and valid. Deletes resource if it is not valid.
		*	STRICT : Performs checks of NORMAL and DEFINED and UNLOADED check.
		*	\param[in]	_Id	Identificator of resource to be processed.
		*	\throw nothrow
		*	\return Return of Load call is resource passes check, false on exception or not pass.
		**/
		inline bool loadResource(const ResourceID _Id) NOEXCEPT {
		#ifdef RESOURCE_HANDLER_STRICT
			if (checkResource(_Id, ResourceCheckFlags::DEFULOAD)) {
		#else
			if (checkResource(_Id)) {
		#endif
				try { return storage[_Id]->Load(); }
				catch (const std::exception& e) {
					#if defined(DEBUG_RESOURCEHANDLER) && defined(RESOURCEHANDLER_MINOR_ERRORS)
						DEBUG_NEW_MESSAGE("ERROR::RESOURCE_HANDLER::loadResource")
							DEBUG_WRITE1("\tMessage: Error occurred during call to Load function. Exception captured.");
							DEBUG_WRITE2("\tResource id: ", _Id);
							DEBUG_WRITE2("\tException content:", e.what());
						DEBUG_END_MESSAGE
					#endif
					return false;
				}
				catch (...) {
					#if defined(DEBUG_RESOURCEHANDLER) && defined(RESOURCEHANDLER_MINOR_ERRORS)
						DEBUG_NEW_MESSAGE("ERROR::RESOURCE_HANDLER::loadResource")
							DEBUG_WRITE1("\tMessage: Error occurred during call to Load function. Something was thrown.");
							DEBUG_WRITE2("\tResource id: ", _Id);
						DEBUG_END_MESSAGE
					#endif
					return false;
				}
			}
			return false;
		}

		/**
		*	\brief Performs an attempt to call Load function of '_count' resources with id in array '_Id'.
		*	NORMAL/STRICT : Derives behaviour from loadResource.
		*	\param[in]	_Id		Array of identificators of resource to be processed.
		*	\param[in]	_count	Count of resources to be processed.
		*	\param[out]	_result	[Optional] Array for every single process result.
		*	\throw nothrow
		*	\return Return true if and only if all resources are successfully processed.
		**/
		bool loadResource(const ResourceID _Id[], const unsigned int _count, bool _result[] = nullptr) NOEXCEPT {
			if (!_count)
				return false;
			bool result = true;
			if (_result) {
				for (unsigned int _index = 0; _index < _count; _index++) {
					_result[_index] = loadResource(_Id[_index]);
					result &= _result[_index];
				}
			} else {
				for (unsigned int _index = 0; _index < _count; _index++)
					result &= loadResource(_Id[_index]);
			}
			return result;
		}

		/**
		*	\brief Performs an attempt to call Load function of all valid resources.
		*	NORMAL/STRICT : Doesn't delete resource if it is invalid.
		*	\param[out]	_count	Size of returned array.
		*	\throw nothrow
		*	\return Return std::unique_ptr to array of process statuses.
		**/
		std::unique_ptr<bool> loadAll(unsigned int& _count) NOEXCEPT {
			//Return size via _count
			_count = storage.size();
			//Allocate new array of bool
			std::unique_ptr<bool> _result(new bool[_count]);
			//Simple pointer for write only operation
			bool* _ptr = _result.get();
			//Some counter to adsress array
			unsigned int _counter = 0;
			for (auto &v : storage) {
				//There is undefined behaviour if call forceDelete during for (auto &v : storage)
				//So just don't process invalid resources.
				if (v.second->status & Resource::ResourceStatus::INVALID) {
					_ptr[_counter] = false;
					_counter++;
					continue;
				}
				try { _ptr[_counter] = v.second->Load(); }
				catch (const std::exception& e) {
					#if defined(DEBUG_RESOURCEHANDLER) && defined(RESOURCEHANDLER_MINOR_ERRORS)
						DEBUG_NEW_MESSAGE("ERROR::RESOURCE_HANDLER::loadAll")
							DEBUG_WRITE1("\tMessage: Error occurred during call to Load function. Exception captured.");
							DEBUG_WRITE2("\tResource id: ", v.first);
							DEBUG_WRITE2("\tException content:", e.what());
						DEBUG_END_MESSAGE
					#endif
					_ptr[_counter] = false;
				}
				catch (...) {
					#if defined(DEBUG_RESOURCEHANDLER) && defined(RESOURCEHANDLER_MINOR_ERRORS)
						DEBUG_NEW_MESSAGE("ERROR::RESOURCE_HANDLER::loadAll")
							DEBUG_WRITE1("\tMessage: Error occurred during call to Load function. Something was thrown.");
							DEBUG_WRITE2("\tResource id: ", v.first);
						DEBUG_END_MESSAGE
					#endif
					_ptr[_counter] = false;
				}
				_counter++;
			}
			//Clear used pointer
			_ptr = nullptr;
			return std::move(_result);
		}

		/**
		*	\brief Performs an attempt to call Load function of all valid resources.
		*	NORMAL/STRICT : Doesn't delete resource if it is invalid.
		*	\throw nothrow
		*	\return Return true if and only if all valid resources are successfully processed.
		**/
		bool loadAll() NOEXCEPT {
			bool _result = true;
			for (auto &v : storage) {
				//There is undefined behaviour if call forceDelete during for (auto &v : storage)
				//So just don't process invalid resources.
				if (v.second->status & Resource::ResourceStatus::INVALID)
					continue;
				try { _result &= v.second->Load(); }
				catch (const std::exception& e) {
					#if defined(DEBUG_RESOURCEHANDLER) && defined(RESOURCEHANDLER_MINOR_ERRORS)
						DEBUG_NEW_MESSAGE("ERROR::RESOURCE_HANDLER::loadAll")
							DEBUG_WRITE1("\tMessage: Error occurred during call to Load function. Exception captured.");
							DEBUG_WRITE2("\tResource id: ", v.first);
							DEBUG_WRITE2("\tException content:", e.what());
						DEBUG_END_MESSAGE
					#endif
					_result = false;
				}
				catch (...) {
					#if defined(DEBUG_RESOURCEHANDLER) && defined(RESOURCEHANDLER_MINOR_ERRORS)
						DEBUG_NEW_MESSAGE("ERROR::RESOURCE_HANDLER::loadAll")
							DEBUG_WRITE1("\tMessage: Error occurred during call to Load function. Something was thrown.");
							DEBUG_WRITE2("\tResource id: ", v.first);
						DEBUG_END_MESSAGE
					#endif
					_result = false;
				}
			}
			return _result;
		}

		/**
		*	\brief Performs an attempt to call Unload function of resource with id '_Id'.
		*	NORMAL : Performs checks that resource is presented and valid. Deletes resource if it is not valid.
		*	STRICT : Performs checks of NORMAL and DEFINED and LOADED check.
		*	\param[in]	_Id	Identificator of resource to be processed.
		*	\throw nothrow
		*	\return Return of Load call is resource passes check, false on exception or not pass.
		**/
		inline bool unloadResource(const ResourceID _Id) NOEXCEPT {
		#ifdef RESOURCE_HANDLER_STRICT
			if (checkResource(_Id, ResourceCheckFlags::DEFLOAD)) {
		#else
			if (checkResource(_Id)) {
		#endif
				try { return storage[_Id]->Unload(); }
				catch (const std::exception& e) {
					#if defined(DEBUG_RESOURCEHANDLER) && defined(RESOURCEHANDLER_MINOR_ERRORS)
						DEBUG_NEW_MESSAGE("ERROR::RESOURCE_HANDLER::unloadResource")
							DEBUG_WRITE1("\tMessage: Error occurred during call to Unload function. Exception captured.");
							DEBUG_WRITE2("\tResource id: ", _Id);
							DEBUG_WRITE2("\tException content:", e.what());
						DEBUG_END_MESSAGE
					#endif
					return false;
				}
				catch (...) {
					#if defined(DEBUG_RESOURCEHANDLER) && defined(RESOURCEHANDLER_MINOR_ERRORS)
						DEBUG_NEW_MESSAGE("ERROR::RESOURCE_HANDLER::unloadResource")
							DEBUG_WRITE1("\tMessage: Error occurred during call to Unload function. Something was thrown.");
							DEBUG_WRITE2("\tResource id: ", _Id);
						DEBUG_END_MESSAGE
					#endif
					return false;
				}
			}
			return false;
		}

		/**
		*	\brief Performs an attempt to call Unload function of '_count' resources with id in array '_Id'.
		*	NORMAL/STRICT : Derives behaviour from unloadResource.
		*	\param[in]	_Id		Array of identificators of resource to be processed.
		*	\param[in]	_count	Count of resources to be processed.
		*	\param[out]	_result	[Optional] Array for every single process result.
		*	\throw nothrow
		*	\return Return true if and only if all resources are successfully processed.
		**/
		bool unloadResource(const ResourceID _Id[], const unsigned int _count, bool _result[] = nullptr) NOEXCEPT {
			if (!_count)
				return false;
			bool result = true;
			if (_result) {
				for (unsigned int _index = 0; _index < _count; _index++) {
					_result[_index] = unloadResource(_Id[_index]);
					result &= _result[_index];
				}
			} else {
				for (unsigned int _index = 0; _index < _count; _index++)
					result &= unloadResource(_Id[_index]);
			}
			return result;
		}

		/**
		*	\brief Performs an attempt to call Unload function of all valid resources.
		*	NORMAL/STRICT : Doesn't delete resource if it is invalid.
		*	\param[out]	_count	Size of returned array.
		*	\throw nothrow
		*	\return Return std::unique_ptr to array of process statuses.
		**/
		std::unique_ptr<bool> unloadAll(unsigned int& _count) NOEXCEPT {
			//Return size via _count
			_count = storage.size();
			//Allocate new array of bool
			std::unique_ptr<bool> _result(new bool[_count]);
			//Simple pointer for write only operation
			bool* _ptr = _result.get();
			//Some counter to adsress array
			unsigned int _counter = 0;
			for (auto &v : storage) {
				//There is undefined behaviour if call forceDelete during for (auto &v : storage)
				//So just don't process invalid resources.
				if (v.second->status & Resource::ResourceStatus::INVALID) {
					_ptr[_counter] = false;
					_counter++;
					continue;
				}
				try { _ptr[_counter] = v.second->Unload(); }
				catch (const std::exception& e) {
					#if defined(DEBUG_RESOURCEHANDLER) && defined(RESOURCEHANDLER_MINOR_ERRORS)
						DEBUG_NEW_MESSAGE("ERROR::RESOURCE_HANDLER::unloadAll")
							DEBUG_WRITE1("\tMessage: Error occurred during call to Unload function. Exception captured.");
							DEBUG_WRITE2("\tResource id: ", v.first);
							DEBUG_WRITE2("\tException content:", e.what());
						DEBUG_END_MESSAGE
					#endif
					_ptr[_counter] = false;
				}
				catch (...) {
					#if defined(DEBUG_RESOURCEHANDLER) && defined(RESOURCEHANDLER_MINOR_ERRORS)
						DEBUG_NEW_MESSAGE("ERROR::RESOURCE_HANDLER::unloadAll")
							DEBUG_WRITE1("\tMessage: Error occurred during call to Unload function. Something was thrown.");
							DEBUG_WRITE2("\tResource id: ", v.first);
						DEBUG_END_MESSAGE
					#endif
					_ptr[_counter] = false;
				}
				_counter++;
			}
			//Clear used pointer
			_ptr = nullptr;
			return std::move(_result);
		}

		/**
		*	\brief Performs an attempt to call Unload function of all valid resources.
		*	NORMAL/STRICT : Doesn't delete resource if it is invalid.
		*	\throw nothrow
		*	\return Return true if and only if all valid resources are successfully processed.
		**/
		bool unloadAll() NOEXCEPT {
			bool _result = true;
			for (auto &v : storage) {
				//There is undefined behaviour if call forceDelete during for (auto &v : storage)
				//So just don't process invalid resources.
				if (v.second->status & Resource::ResourceStatus::INVALID)
					continue;
				try { _result &= v.second->Unload(); }
				catch (const std::exception& e) {
					#if defined(DEBUG_RESOURCEHANDLER) && defined(RESOURCEHANDLER_MINOR_ERRORS)
						DEBUG_NEW_MESSAGE("ERROR::RESOURCE_HANDLER::unloadAll")
							DEBUG_WRITE1("\tMessage: Error occurred during call to Unload function. Exception captured.");
							DEBUG_WRITE2("\tResource id: ", v.first);
							DEBUG_WRITE2("\tException content:", e.what());
						DEBUG_END_MESSAGE
					#endif
					_result = false;
				}
				catch (...) {
					#if defined(DEBUG_RESOURCEHANDLER) && defined(RESOURCEHANDLER_MINOR_ERRORS)
						DEBUG_NEW_MESSAGE("ERROR::RESOURCE_HANDLER::unloadAll")
							DEBUG_WRITE1("\tMessage: Error occurred during call to Unload function. Something was thrown.");
							DEBUG_WRITE2("\tResource id: ", v.first);
						DEBUG_END_MESSAGE
					#endif
					_result = false;
				}
			}
			return _result;
		}

		/**
		*	\brief Performs an attempt to call Reload function of resource with id '_Id'.
		*	NORMAL/STRICT : Performs checks that resource is presented and valid. Deletes resource if it is not valid.
		*	\param[in]	_Id	Identificator of resource to be processed.
		*	\throw nothrow
		*	\return Return of Load call is resource passes check, false on exception or not pass.
		**/
		inline bool reloadResource(const ResourceID _Id) NOEXCEPT {
			if (checkResource(_Id)) {
				try { return storage[_Id]->Reload(); }
				catch (const std::exception& e) {
					#if defined(DEBUG_RESOURCEHANDLER) && defined(RESOURCEHANDLER_MINOR_ERRORS)
						DEBUG_NEW_MESSAGE("ERROR::RESOURCE_HANDLER::reloadResource")
							DEBUG_WRITE1("\tMessage: Error occurred during call to Reload function. Exception captured.");
							DEBUG_WRITE2("\tResource id: ", _Id);
							DEBUG_WRITE2("\tException content:", e.what());
						DEBUG_END_MESSAGE
					#endif
					return false;
				}
				catch (...) {
					#if defined(DEBUG_RESOURCEHANDLER) && defined(RESOURCEHANDLER_MINOR_ERRORS)
						DEBUG_NEW_MESSAGE("ERROR::RESOURCE_HANDLER::reloadResource")
							DEBUG_WRITE1("\tMessage: Error occurred during call to Reload function. Something was thrown.");
							DEBUG_WRITE2("\tResource id: ", _Id);
						DEBUG_END_MESSAGE
					#endif
					return false;
				}
			}
			return false;
		}

		/**
		*	\brief Performs an attempt to call Reload function of '_count' resources with id in array '_Id'.
		*	NORMAL/STRICT : Derives behaviour from reloadResource.
		*	\param[in]	_Id		Array of identificators of resource to be processed.
		*	\param[in]	_count	Count of resources to be processed.
		*	\param[out]	_result	[Optional] Array for every single process result.
		*	\throw nothrow
		*	\return Return true if and only if all resources are successfully processed.
		**/
		bool reloadResource(const ResourceID _Id[], const unsigned int _count, bool _result[] = nullptr) NOEXCEPT {
			if (!_count)
				return false;
			bool result = true;
			if (_result) {
				for (unsigned int _index = 0; _index < _count; _index++) {
					_result[_index] = loadResource(_Id[_index]);
					result &= _result[_index];
				}
			} else {
				for (unsigned int _index = 0; _index < _count; _index++)
					result &= loadResource(_Id[_index]);
			}
			return result;
		}

		/**
		*	\brief Performs an attempt to call Reload function of all valid resources.
		*	NORMAL/STRICT : Doesn't delete resource if it is invalid.
		*	\param[out]	_count	Size of returned array.
		*	\throw nothrow
		*	\return Return std::unique_ptr to array of process statuses.
		**/
		std::unique_ptr<bool> reloadAll(unsigned int& _count) NOEXCEPT {
			//Return size via _count
			_count = storage.size();
			//Allocate new array of bool
			std::unique_ptr<bool> _result(new bool[_count]);
			//Simple pointer for write only operation
			bool* _ptr = _result.get();
			//Some counter to adsress array
			unsigned int _counter = 0;
			for (auto &v : storage) {
				//There is undefined behaviour if call forceDelete during for (auto &v : storage)
				//So just don't process invalid resources.
				if (v.second->status & Resource::ResourceStatus::INVALID) {
					_ptr[_counter] = false;
					_counter++;
					continue;
				}
				try { _ptr[_counter] = v.second->Reload(); }
				catch (const std::exception& e) {
					#if defined(DEBUG_RESOURCEHANDLER) && defined(RESOURCEHANDLER_MINOR_ERRORS)
						DEBUG_NEW_MESSAGE("ERROR::RESOURCE_HANDLER::reloadAll")
							DEBUG_WRITE1("\tMessage: Error occurred during call to Reload function. Exception captured.");
							DEBUG_WRITE2("\tResource id: ", v.first);
							DEBUG_WRITE2("\tException content:", e.what());
						DEBUG_END_MESSAGE
					#endif
					_ptr[_counter] = false;
				}
				catch (...) {
					#if defined(DEBUG_RESOURCEHANDLER) && defined(RESOURCEHANDLER_MINOR_ERRORS)
						DEBUG_NEW_MESSAGE("ERROR::RESOURCE_HANDLER::reloadAll")
							DEBUG_WRITE1("\tMessage: Error occurred during call to Reload function. Something was thrown.");
							DEBUG_WRITE2("\tResource id: ", v.first);
						DEBUG_END_MESSAGE
					#endif
					_ptr[_counter] = false;
				}
				_counter++;
			}
			//Clear used pointer
			_ptr = nullptr;
			return std::move(_result);
		}

		/**
		*	\brief Performs an attempt to call Reload function of all valid resources.
		*	NORMAL/STRICT : Doesn't delete resource if it is invalid.
		*	\throw nothrow
		*	\return Return true if and only if all valid resources are successfully processed.
		**/
		bool reloadAll() NOEXCEPT {
			bool _result = true;
			for (auto &v : storage) {
				//There is undefined behaviour if call forceDelete during for (auto &v : storage)
				//So just don't process invalid resources.
				if (v.second->status & Resource::ResourceStatus::INVALID)
					continue;
				try { _result &= v.second->Reload(); }
				catch (const std::exception& e) {
					#if defined(DEBUG_RESOURCEHANDLER) && defined(RESOURCEHANDLER_MINOR_ERRORS)
						DEBUG_NEW_MESSAGE("ERROR::RESOURCE_HANDLER::reloadAll")
							DEBUG_WRITE1("\tMessage: Error occurred during call to Reload function. Exception captured.");
							DEBUG_WRITE2("\tResource id: ", v.first);
							DEBUG_WRITE2("\tException content:", e.what());
						DEBUG_END_MESSAGE
					#endif
					_result = false;
				}
				catch (...) {
					#if defined(DEBUG_RESOURCEHANDLER) && defined(RESOURCEHANDLER_MINOR_ERRORS)
						DEBUG_NEW_MESSAGE("ERROR::RESOURCE_HANDLER::reloadAll")
							DEBUG_WRITE1("\tMessage: Error occurred during call to Reload function. Something was thrown.");
							DEBUG_WRITE2("\tResource id: ", v.first);
						DEBUG_END_MESSAGE
					#endif
					_result = false;
				}
			}
			return _result;
		}

		unsigned int collectGarbage(int _bandwidth = -1) {

		}

		#ifdef UNUSED_V006
			bool recoverCacheFile(std::shared_ptr<CacheFile>& _invalidCacheFile) {
				//Create new empty cache file
				cacheFiles.push_back(std::move(std::shared_ptr<CacheFile>(new CacheFile())));
				//Move resources of invalid file to new one
				if (!(_invalidCacheFile->Recover(*cacheFiles.back())))
					return false;
				//Search and delete shared_ptr to invalid cache file
				auto _begin = cacheFiles.begin();
				auto _end = cacheFiles.end();
				while (_begin != _end) {
					if (_begin->get() == _invalidCacheFile.get()) {
						cacheFiles.erase(_begin);
						break;
					}
					_begin++;
				}
				return true;
			}

			bool cacheResource(ResourceID _Id, std::shared_ptr<CacheFile> _hint = nullptr) {
				//Check resource owner: #1
				if (storage.count(_Id)) {
					//Ptr to cache resource
					auto _resource = storage[_Id];
					//Check that resource not already cached: #2
					if (_resource->status & Resource::ResourceStatus::INVALID) {
						#ifdef DEBUG_RESOURCEHANDLER
							#ifdef EVENTS_RESOURCEHANDLER
								DEBUG_OUT << "EVENT::RESOURCE_HANDLER::cacheResource" << DEBUG_NEXT_LINE;
								DEBUG_OUT << "\tMessage: Attempt to cache invalid resource." << DEBUG_NEXT_LINE;
								#ifdef RESOURCE_HANDLER_STRICT
									DEBUG_OUT << "\tStrict mode: Resource will be removed." << DEBUG_NEXT_LINE;
								#endif
								DEBUG_OUT << "\tResource id: " << _Id << DEBUG_NEXT_LINE;
							#endif
						#endif
						#ifdef RESOURCE_HANDLER_STRICT
							deleteResource(_Id);
						#endif 
						return false;
					}//#2
					//Check that resource can be cached: #3
					if (!_resource->canBeCached) {
						#ifdef DEBUG_RESOURCEHANDLER
							DEBUG_OUT << "ERROR::RESOURCE_HANDLER::cacheResource" << DEBUG_NEXT_LINE;
							DEBUG_OUT << "\tMessage: Object with '_Id': " << _Id << " is defined as uncached." << DEBUG_NEXT_LINE;
						#endif
						return false;
					}//#3
					//Check that resource not already cached: #4
					if (_resource->status & Resource::ResourceStatus::CACHED) {
						#ifdef DEBUG_RESOURCEHANDLER
							#ifdef EVENTS_RESOURCEHANDLER
								DEBUG_OUT << "EVENT::RESOURCE_HANDLER::cacheResource" << DEBUG_NEXT_LINE;
								DEBUG_OUT << "\tMessage: Attempt to cache cached resource." << DEBUG_NEXT_LINE;
							#endif
						#endif
						return false;
					}//#4
					//Acquire resource size information
					auto _resSize = _resource->usedMemory();
					//Setup cache file ptr
					std::shared_ptr<CacheFile> _currCacheFile(_hint);
					if (!_currCacheFile) { //#5
						//Serach for suitable cache file
						for (auto &v : cacheFiles) {
							if (v->getFreeSize() > _resSize || !v->holdedResourcesCount()) {
								_currCacheFile = v;
								break;
							}
						}
						//If suitable cache file not found create new one.
						if (!_currCacheFile) {//#6
							#ifdef DEBUG_RESOURCEHANDLER
								#ifdef EVENTS_RESOURCEHANDLER
									DEBUG_OUT << "EVENT::RESOURCE_HANDLER::cacheResource" << DEBUG_NEXT_LINE;
									DEBUG_OUT << "\tMessage: No suitable cache file found. New cache file created." << DEBUG_NEXT_LINE;
								#endif
							#endif
							cacheFiles.push_back(std::move(std::shared_ptr<CacheFile>(new CacheFile())));
							_currCacheFile = cacheFiles.back();
						}//#6
					}//#5

					//Open cache file stream to write mode : #7
					if (_currCacheFile->Open(CacheFile::CacheFileState::WRITE)) {
						//Acquire underlying filestream
						auto _cacheStream = _currCacheFile->getStream();
						//Check that filestream is valid : #8
						if (_cacheStream) {
							//Register new resource to cache file storage : #9
							if (_currCacheFile->registerResource(_Id, _resSize)) {
								//Create buffer stream
								std::istringstream _outputBuffer;
								//Transfer buffer stream to be filled with cache data : #10
								if (_resource->Cache(_outputBuffer)) {
									//Check for IO errors : #11
									if (_outputBuffer) {
										//Create unique identifier
										std::string _oidentifier(RH_CAHCE_UNIQUE_OPEN);
										std::string _cidentifier(RH_CAHCE_UNIQUE_CLOSE);
										_oidentifier.replace(_oidentifier.find("$"), 1, std::to_string(_Id));
										_cidentifier.replace(_cidentifier.find("$"), 1, std::to_string(_Id));
										*_cacheStream << _oidentifier;
										*_cacheStream << _outputBuffer.str();
										*_cacheStream << _cidentifier;
										*_cacheStream << std::endl;
										if (_currCacheFile->Close()) { //#12
											_resource->status |= Resource::ResourceStatus::CACHED;
											return true;
										}//#12
										//Error during write operation _buffer -> fstream
										#ifdef DEBUG_RESOURCEHANDLER
											DEBUG_OUT << "ERROR::RESOURCE_HANDLER::cacheResource" << DEBUG_NEXT_LINE;
											DEBUG_OUT << "\tMessage: Stream error occurred during unbuffering." << DEBUG_NEXT_LINE;
										#endif
										return false;
									}//#11
									//Stream error occurred during cache process in resource
									#ifdef DEBUG_RESOURCEHANDLER
										DEBUG_OUT << "ERROR::RESOURCE_HANDLER::cacheResource" << DEBUG_NEXT_LINE;
										DEBUG_OUT << "\tMessage: Stream error occurred during resource caching." << DEBUG_NEXT_LINE;
									#endif
									return false;
								}//#10
								//Cache process doesn't succeeded in _resource
								#ifdef DEBUG_RESOURCEHANDLER
									DEBUG_OUT << "ERROR::RESOURCE_HANDLER::cacheResource" << DEBUG_NEXT_LINE;
									DEBUG_OUT << "\tMessage: Error occurred during resource caching." << DEBUG_NEXT_LINE;
								#endif
								return false;
							}//#9
							//Cache file can't register resorce. This fork must never be used by reason : #4
							#ifdef DEBUG_RESOURCEHANDLER
								DEBUG_OUT << "ERROR::RESOURCE_HANDLER::cacheResource" << DEBUG_NEXT_LINE;
								DEBUG_OUT << "\tMessage: _currCacheFile can't register a resource." << DEBUG_NEXT_LINE;
							#endif
							return false;
						}//#8
						//Cache file underlying stream error
						recoverCacheFile(_currCacheFile);
						return cacheResource(_Id, cacheFiles.back());
					}//#7
					//Cache file open internal error
					recoverCacheFile(_currCacheFile);
					return cacheResource(_Id, cacheFiles.back());
				}//#1
				//Resource not found in current RH
				#ifdef DEBUG_RESOURCEHANDLER
					DEBUG_OUT << "ERROR::RESOURCE_HANDLER::cacheResource" << DEBUG_NEXT_LINE;
					DEBUG_OUT << "\tMessage: Object with '_Id': " << _Id << " not found." << DEBUG_NEXT_LINE;
				#endif
				return false;
			}

			int Cache(int _count = -1) {

			}

			bool restoreResource(ResourceID _Id, std::shared_ptr<CacheFile> _hint = nullptr) {
				if (storage.count(_Id)) {
					auto _resource = storage[_Id];
					std::shared_ptr<CacheFile> _currCacheFile(_hint);
					if (!_currCacheFile || !_currCacheFile->isRegistred(_Id)) {
						for (auto &v : cacheFiles) {
							if (_currCacheFile->isRegistred(_Id))
								_currCacheFile = v;
						}
						if (!_currCacheFile) {
							return false;
						}
					}
				
					if (_currCacheFile->Open(CacheFile::CacheFileState::READ)) {
						//Acquire underlying filestream
						auto _cacheStream = _currCacheFile->getStream();
						if (_cacheStream) {
							//Create unique open identifier
							std::string _oidentifier(RH_CAHCE_UNIQUE_OPEN);
							_oidentifier.replace(_oidentifier.find("$"), 1, std::to_string(_Id));
							std::istream_iterator<std::string> _siterator(*_cacheStream);
							std::istream_iterator<std::string> _end;
							while (_siterator != _end) {
								if (_siterator->find(_oidentifier) != std::string::npos)
									break;
								_siterator++;
							}
							if (_siterator == _end) {
								//Open identifier not found
								return false;
							}
							//Create unique close identifier
							std::string _cidentifier(RH_CAHCE_UNIQUE_CLOSE);
							_cidentifier.replace(_cidentifier.find("$"), 1, std::to_string(_Id));
							//Whole resource presented in one string
							if (_siterator->find(_cidentifier)) {

								return true;
							}
							//Resource end in other string
							std::istream_iterator<std::string> _eiterator(_siterator);
							_eiterator++;
							while (_eiterator != _end) {
								if (_eiterator->find(_cidentifier) != std::string::npos)
									break;
								_eiterator++;
							}
							if (_eiterator == _end) {
								//Close identifier not found

								return false;
							}
							//Create buffer string stream
							std::stringstream _inputBuffer;
							//Выделение из текущей строки всего, что после метки: может вызвать проблемы
							_inputBuffer << _siterator->substr(_siterator->find(_oidentifier)).erase(0, _oidentifier.length());
							std::istream_iterator<std::string> _miterator(_siterator);
							while (_miterator != _eiterator) {
								_inputBuffer << *_miterator;
								_miterator++;
							}
							_inputBuffer << _eiterator->substr(0, _eiterator->find(_cidentifier));

							if (_resource->Restore(_inputBuffer)) {

							}

							return false;
						}

						return false;
					}

					return false;
				}

				return false;
			}

			int Restore(int _count = -1) {

			}
		#endif // UNUSED_V006
	};
}
#endif
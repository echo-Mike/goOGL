#ifndef RESOURCEHANDLER_H
#define RESOURCEHANDLER_H "[0.0.5@cResourceHandler.h]"
/*
*	DESCRIPTION:
*		Module contains implementation of internal resource handler class and cache file
*		helper structure.
*	AUTHOR:
*		Mikhail Demchenko
*		mailto:dev.echo.mike@gmail.com
*		https://github.com/echo-Mike
*/
//STD
#include <map>
#include <set>
#include <vector>
#include <memory>
#include <string>
#include <iostream>
#include <fstream>
#include <cstdio>
//ASSIMP

//OUR
#include "RHE\vResourceGeneral.h"
#include "RHE\cResource.h"
#include "RHE\cCacheFile.h"
//DEBUG
#ifdef DEBUG_RESOURCEHANDLER
	#ifndef DEBUG_OUT
		#define DEBUG_OUT std::cout
	#endif
	#ifndef DEBUG_NEXT_LINE
		#define DEBUG_NEXT_LINE std::endl
	#endif
#endif

namespace resources {

	#ifdef RESOURCE_HANDLER_STRICT
		/**
		*	Runtime identification of Resource Handler strict mode.
		**/
		const bool __RHStrictMode = true;
	#else
		/**
		*	Runtime identification of Resource Handler strict mode.
		**/
		const bool __RHStrictMode = false;
	#endif

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
		std::vector<std::unique_ptr<CacheFile>> cacheFiles;
		//Enumiration of all possible resource handler states
		enum ResourceHandlerStatus : int {
			UNDEFINED,
			PUBLIC,
			PRIVATE
		};
		ResourceHandlerStatus status;
		
		ResourceHandler() : status(ResourceHandlerStatus::UNDEFINED) 
		{
			cacheFiles.push_back(std::move(std::unique_ptr<CacheFile>(new CacheFile())));
		}

		~ResourceHandler() {}

		unsigned int memoryHandled(unsigned int& _cary) {
			_cary = 0;
			unsigned int _result = 0;
			unsigned int _buff = 0;
			for (auto &v : storage) {
				_buff = v.second->usedMemory();
				if (_result + _buff < _result)
					_cary++;
				_result += _buff;
			}
		}

		bool Cache(ResourceID _Id) {

		}

		int Cache(int _count = -1) {

		}

		bool Restore(ResourceID _Id) {

		}

		int Restore(int _count = -1) {

		}

		template < class T >
		inline std::shared_ptr<T> newResource(T&& _value, ResourceID _Id) {
			try { std::shared_ptr<T> _newptr(new T(std::move(_value))); }
			catch (const std::exception& e) { throw std::logic_error(e.what()); }
			catch (...) { throw std::logic_error("ERROR::RESOURCE_HANDLER::newResource::Object creation error."); }
			if (storage.count(_Id)) {
				#ifdef RESOURCE_HANDLER_STRICT
					#ifdef DEBUG_RESOURCEHANDLER
						DEBUG_OUT << "ERROR::RESOURCE_HANDLER::newResource" << DEBUG_NEXT_LINE;
						DEBUG_OUT << "\tMessage: Object with '_Id': " << _Id << " already exists." << DEBUG_NEXT_LINE;
					#endif
					return std::shared_ptr<T>(nullptr);
				#endif
				storage.erase(_Id);
			}
			storage[_Id] = _newptr;
			return std::move(_newptr);
		}

		template < class T >
		inline std::shared_ptr<T> newResource(T* _valueptr, ResourceID _Id) {
			#ifdef RESOURCE_HANDLER_STRICT
				if (!_valueptr) {
					#ifdef DEBUG_RESOURCEHANDLER
						DEBUG_OUT << "ERROR::RESOURCE_HANDLER::newResource" << DEBUG_NEXT_LINE;
						DEBUG_OUT << "\tMessage: '_valueptr' is a nullptr." << DEBUG_NEXT_LINE;
					#endif
					return std::shared_ptr<T>(nullptr);
				}
			#endif
			//Result of next two lines are the same, so less actions more performance.
			/*std::shared_ptr<Resource> _newptr((Resource*)_valueptr);*/
			std::shared_ptr<T> _newptr(_valueptr);
			if (storage.count(_Id)) {
				#ifdef RESOURCE_HANDLER_STRICT
					#ifdef DEBUG_RESOURCEHANDLER
						DEBUG_OUT << "ERROR::RESOURCE_HANDLER::newResource" << DEBUG_NEXT_LINE;
						DEBUG_OUT << "\tMessage: Object with '_Id': " << _Id << " already exists." << DEBUG_NEXT_LINE;
					#endif
					return std::shared_ptr<T>(nullptr);
				#endif
				storage.erase(_Id);
			}
			storage[_Id] = _newptr;
			return std::move(_newptr);
		}

		template < class T >
		inline unsigned int newResource(const T& _value, ResourceID _Id[], std::shared_ptr<T> _result[], unsigned int _count, bool _success[] = nullptr) {
			if (!_count)
				return 0;
			unsigned int _allocated = 0;
			for (unsigned int _index = 0; _index < _count; _index++) {
				try { _result[_index] = newResource(std::move(T(_value)), _Id[_index]); }
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
		inline unsigned int newResource(const T* _valueptr, ResourceID _Id[], std::shared_ptr<T> _result[], unsigned int _count, bool _success[] = nullptr) {
			return newResource(*_valueptr, _Id, _result, _count, _success);
		}

		template < class T >
		inline std::shared_ptr<T> newCopy(ResourceID _sourceId, ResourceID _Id) {
			if (storage.count(_sourceId)) {
				try { std::shared_ptr<T> _newptr(new T(*(storage[_sourceId]))); }
				catch (const std::exception& e) { throw std::logic_error(e.what()); }
				catch (...) { throw std::logic_error("ERROR::RESOURCE_HANDLER::newCopy::Object creation error."); }
				if (storage.count(_Id)) {
					#ifdef RESOURCE_HANDLER_STRICT
						#ifdef DEBUG_RESOURCEHANDLER
							DEBUG_OUT << "ERROR::RESOURCE_HANDLER::newResource" << DEBUG_NEXT_LINE;
							DEBUG_OUT << "\tMessage: Object with '_Id': " << _Id << " already exists." << DEBUG_NEXT_LINE;
						#endif
						return std::shared_ptr<T>(nullptr);
					#endif
					storage.erase(_Id);
				}
				storage[_Id] = _newptr;
				return std::move(_newptr);
			} else {
				#ifdef DEBUG_RESOURCEHANDLER
					DEBUG_OUT << "ERROR::RESOURCE_HANDLER::newCopy" << DEBUG_NEXT_LINE;
					DEBUG_OUT << "\tMessage: Invalid '_sourceId': " << _sourceId << DEBUG_NEXT_LINE;
				#endif
				return std::shared_ptr<T>(nullptr);
			}
		}

		template < class T >
		inline std::shared_ptr<T> copyResource(ResourceID _sourceId, ResourceID _destId) {
			if (storage.count(_sourceId)) {
				try { std::shared_ptr<T> _newptr(new T(*(storage[_sourceId]))); }
				catch (const std::exception& e) { throw std::logic_error(e.what()); }
				catch (...) { throw std::logic_error("ERROR::RESOURCE_HANDLER::newCopy::Object creation error."); }
				if (storage.erase(_destId)) {
					storage[_destId] = _newptr;
					return std::move(_newptr);
				} else {
					#ifdef DEBUG_RESOURCEHANDLER
						DEBUG_OUT << "ERROR::RESOURCE_HANDLER::copyResource" << DEBUG_NEXT_LINE;
						DEBUG_OUT << "\tMessage: Object with '_Id': " << _Id << " doesn't exists." << DEBUG_NEXT_LINE;
					#endif
					return std::shared_ptr<T>(nullptr);
				}
			} else {
				#ifdef DEBUG_RESOURCEHANDLER
					DEBUG_OUT << "ERROR::RESOURCE_HANDLER::copyResource" << DEBUG_NEXT_LINE;
					DEBUG_OUT << "\tMessage: Invalid '_sourceId': " << _sourceId << DEBUG_NEXT_LINE;
				#endif
				return std::shared_ptr<T>(nullptr);
			}
		}

		template < class T >
		inline std::shared_ptr<T> moveResource(ResourceID _sourceId, ResourceID _destId) {
			if (storage.count(_sourceId)) {
				if (!storage.erase(_destId)) {
					#ifdef RESOURCE_HANDLER_STRICT
						#ifdef DEBUG_RESOURCEHANDLER
							DEBUG_OUT << "ERROR::RESOURCE_HANDLER::moveResource" << DEBUG_NEXT_LINE;
							DEBUG_OUT << "\tMessage: Invalid '_destId': " << _Id << DEBUG_NEXT_LINE;
						#endif
						return std::shared_ptr<T>(nullptr);
					#endif
				}
				storage[_destId] = std::move(storage[_sourceId]);
				storage.erase(_sourceId);
				return std::shared_ptr<T>(storage[_destId]);
			} else {
				#ifdef DEBUG_RESOURCEHANDLER
					DEBUG_OUT << "ERROR::RESOURCE_HANDLER::moveResource" << DEBUG_NEXT_LINE;
					DEBUG_OUT << "\tMessage: Invalid '_sourceId': " << _sourceId << DEBUG_NEXT_LINE;
				#endif
				return std::shared_ptr<T>(nullptr);
			}
		}

		template < class T >
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
		inline std::shared_ptr<T> setResource(T* _valueptr, ResourceID _Id) {
			#ifdef RESOURCE_HANDLER_STRICT
				if (!_valueptr) {
					#ifdef DEBUG_RESOURCEHANDLER
						DEBUG_OUT << "ERROR::RESOURCE_HANDLER::setResource" << DEBUG_NEXT_LINE;
						DEBUG_OUT << "\tMessage: '_valueptr' is a nullptr." << DEBUG_NEXT_LINE;
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
		inline unsigned int setResource(const T& _value, ResourceID _Id[], std::shared_ptr<T> _result[], unsigned int _count, bool _success[] = nullptr) {
			if (!_count)
				return 0;
			unsigned int _allocated = 0;
			for (unsigned int _index = 0; _index < _count; _index++) {
				try { _result[_index] = setResource(std::move(T(_value)), _Id[_index]); }
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
		inline unsigned int setResource(const T* _valueptr, ResourceID _Id[], std::shared_ptr<T> _result[], unsigned int _count, bool _success[] = nullptr) {
			return setResource(*_valueptr, _Id, _result, _count, _success);
		}

		template < class T >
		inline std::shared_ptr<T> getResource(ResourceID _Id) {
			try { return std::shared_ptr<T>(std::dynamic_pointer_cast<T>(storage.at(_Id))); }
			catch (const std::out_of_range& e) {
				#ifdef DEBUG_RESOURCEHANDLER
					DEBUG_OUT << "ERROR::RESOURCE_HANDLER::getResource" << DEBUG_NEXT_LINE;
					DEBUG_OUT << "\tMessage: Identifier '_Id': " << _Id << " doesn't exist." << DEBUG_NEXT_LINE;
				#endif
			}
			catch (...) {
				#ifdef DEBUG_RESOURCEHANDLER
					DEBUG_OUT << "ERROR::RESOURCE_HANDLER::getResource" << DEBUG_NEXT_LINE;
					DEBUG_OUT << "\tMessage: Unknown error." << DEBUG_NEXT_LINE;
				#endif
			}
			return std::shared_ptr<T>(nullptr);
		}

		inline bool deleteResource(ResourceID _Id) { 
			#ifdef RESOURCE_HANDLER_STRICT
				if (storage.count(_Id)) {
					if (storage[_Id].use_count() > 1) {
						#ifdef DEBUG_RESOURCEHANDLER
							DEBUG_OUT << "ERROR::RESOURCE_HANDLER::deleteResource" << DEBUG_NEXT_LINE;
							DEBUG_OUT << "\tMessage: Only handler must own a resource to release it." << DEBUG_NEXT_LINE;
						#endif
						return false;
					}
				}
			#endif	
			storage.erase(_Id);
			return true;
		}
	};
}
#endif
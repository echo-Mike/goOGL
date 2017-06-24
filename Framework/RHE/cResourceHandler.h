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
#include <vector>
#include <memory>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdio>
#include <iterator>
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

		#ifdef UNUSED_V006
			//Dinamyc array of cache files
			std::vector<std::shared_ptr<CacheFile>> cacheFiles;
		#endif // UNUSED_V006

		//Enumiration of all possible resource handler states
		enum ResourceHandlerStatus : int {
			UNDEFINED,
			PUBLIC,
			PRIVATE
		};
		ResourceHandlerStatus status;
		
		ResourceHandler() : status(ResourceHandlerStatus::UNDEFINED) 
		{
			#ifdef UNUSED_V006
				cacheFiles.push_back(std::move(std::shared_ptr<CacheFile>(new CacheFile())));
			#endif // UNUSED_V006
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
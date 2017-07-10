#ifndef RESOURCEHANDLER_H
#define RESOURCEHANDLER_H "[0.0.5@cResourceHandler.h]"
/**
*	DESCRIPTION:
*		Module contains implementation of internal resource handler class.
*	AUTHOR:
*		Mikhail Demchenko
*		mailto:dev.echo.mike@gmail.com
*		https://github.com/echo-Mike
*	PAIRED WITH: cResourceHandler.cpp
**/
//STD
#include <memory>
#include <algorithm>
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
#include "general\vPolymorphicContainerGeneral.hpp"
#ifdef RESOURCE_HANDLER_STRICT
	#include "general\cStrictPolymorphicMap.hpp"
#else
	#include "general\cPolymorphicMap.hpp"
#endif // RESOURCE_HANDLER_STRICT
#ifdef UNUSED_V006
	#include "RHE\cCacheFile.h"
#endif
//DEBUG
#define DEBUG_RESOURCEHANDLER
#define RESOURCEHANDLER_MINOR_ERRORS
//#define RESOURCE_HANDLER_STRICT
#if defined(DEBUG_RESOURCEHANDLER) && !defined(OTHER_DEBUG)
	#include "general\mDebug.h"		
#elif defined(DEBUG_RESOURCEHANDLER) && defined(OTHER_DEBUG)
	#include OTHER_DEBUG
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
	class ResourceHandlingEngine 
	//TODO::DELETE AFTER TEST!!!
	{
		int lol;
	};

	/**
	*	Class that represents resource storage for one scene.
	*	This is a helper class with API open only to ResourceHandlingEngine.
	*	Class have two modes NORMAL and STRICT defined in compile-time.
	*	Class definition: ResourceHandler
	**/
	class ResourceHandler final :
	#ifdef RESOURCE_HANDLER_STRICT
		protected StrictPolymorphicMap<ResourceID, Resource, &Resource::getAllocStrategy>
	#else
		protected PolymorphicMap<ResourceID, Resource, &Resource::getAllocStrategy>
	#endif
	{
		friend class ResourceHandlingEngine;
		//TODO::DELETE AFTER TEST!!!
	public:
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

		//TODO::DELETE AFTER TEST!!!
		ResourceHandler() : status(ResourceHandlerStatus::UNDEFINED), owner(nullptr) {}

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
		unsigned int memoryHandled(unsigned int& _cary);

	public:
		/**
		*	\brief Flags to be used in checkResource.
		**/
		enum ResourceCheckFlags : int {
			//Resource internal states:: (next <br> is intentional)

			//Check that resource is defined 
			DEFINED		= Resource::ResourceStatus::DEFINED,
			//Check that resource is loaded or reloaded
			LOADED		= Resource::ResourceStatus::LOADED,
			//UNUSED : Check that resource was cached
			CACHED		= Resource::ResourceStatus::CACHED,
			//Check that resource use BIG alloc trategy
			ALLOCBIG	= Resource::ResourceStatus::ALLOCBIG,
			//Check that resource is valid
			INVALID		= Resource::ResourceStatus::INVALID,
			//Resource states in storage:: (next <br> is intentional)

			//Check that resource is presented in current storage
			PRESENTED	= Resource::ResourceStatus::MAX << 1,
			//Complex checks:: (next <br> is intentional)

			//Check that resource defined and loaded
			DEFLOAD		= DEFINED | LOADED,
			//Check that resource defined and presented
			PRESDEF		= DEFINED | PRESENTED,

			//End of bits indicator
			MAX			= PRESENTED
		};
		//TODO::UNCOMMENT AFTER TEST!!!
	//private:

		/**
		*	\brief Check resource status to satisfy certain flag arrangement.
		*	Checks that ALL '_upFlags' are UP and ALL '_downFlags' are DOWN.
		*	Resource with INVALID flag in UP state will be deleted after check.
		*	Will always return false if any flag checked in UP and DOWN state simultaneously.
		*	\param[in]	_Id			Identificator of resource.
		*	\param[in]	_upFlags	Flags to check it's state is UP.
		*	\param[in]	_downFlags	Flags to check it's state is DOWN.
		*	\throw nothrow
		*	\return Result of check
		**/
		bool checkResourceAll(const ResourceID _Id, int _upFlags = ResourceCheckFlags::PRESDEF, int _downFlags = ResourceCheckFlags::INVALID) NOEXCEPT;

		/**
		*	\brief Check resource status to satisfy certain flag arrangement.
		*	Checks that ANY of '_upFlags' are UP or ANY of '_downFlags' are DOWN.
		*	Resource with INVALID flag in UP state will be deleted after check.
		*	\param[in]	_Id			Identificator of resource.
		*	\param[in]	_upFlags	Flags to check it's state is UP.
		*	\param[in]	_downFlags	Flags to check it's state is DOWN.
		*	\throw nothrow
		*	\return Result of check
		**/
		bool checkResourceAny(const ResourceID _Id, int _upFlags = ResourceCheckFlags::PRESDEF, int _downFlags = ResourceCheckFlags::INVALID) NOEXCEPT;

		/**
		*	\brief Force removal of resource with id '_Id'.
		*	NORMAL/STRICT : Ignores all strict rules for deleteResource.
		*	This function is for internal use only.
		*	\param[in]	_Id	Identificator of resource to be deleted.
		*	\throw nothrow
		*	\return Nothing
		**/
		void forceDelete(const ResourceID _Id) NOEXCEPT;

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
			if (checkResourceAll(_Id, ResourceCheckFlags::PRESDEF,  ResourceCheckFlags::LOADED | ResourceCheckFlags::INVALID)) {
		#else
			if (checkResourceAll(_Id)) {
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
		bool loadResource(const ResourceID _Id[], const unsigned int _count, bool _result[] = nullptr) NOEXCEPT;

		/**
		*	\brief Performs an attempt to call Load function of all valid resources.
		*	NORMAL/STRICT : Doesn't delete resource if it is invalid.
		*	\param[out]	_count	Size of returned array.
		*	\throw nothrow
		*	\return Return std::unique_ptr to array of process statuses.
		**/
		std::unique_ptr<bool[]> loadAll(unsigned int& _count) NOEXCEPT;

		/**
		*	\brief Performs an attempt to call Load function of all valid resources.
		*	NORMAL/STRICT : Doesn't delete resource if it is invalid.
		*	\throw nothrow
		*	\return Return true if and only if all valid resources are successfully processed.
		**/
		bool loadAll() NOEXCEPT;

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
			if (checkResourceAll(_Id, ResourceCheckFlags::DEFLOAD)) {
		#else
			if (checkResourceAll(_Id)) {
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
		bool unloadResource(const ResourceID _Id[], const unsigned int _count, bool _result[] = nullptr) NOEXCEPT;

		/**
		*	\brief Performs an attempt to call Unload function of all valid resources.
		*	NORMAL/STRICT : Doesn't delete resource if it is invalid.
		*	\param[out]	_count	Size of returned array.
		*	\throw nothrow
		*	\return Return std::unique_ptr to array of process statuses.
		**/
		std::unique_ptr<bool[]> unloadAll(unsigned int& _count) NOEXCEPT;

		/**
		*	\brief Performs an attempt to call Unload function of all valid resources.
		*	NORMAL/STRICT : Doesn't delete resource if it is invalid.
		*	\throw nothrow
		*	\return Return true if and only if all valid resources are successfully processed.
		**/
		bool unloadAll() NOEXCEPT;

		/**
		*	\brief Performs an attempt to call Reload function of resource with id '_Id'.
		*	NORMAL/STRICT : Performs checks that resource is presented and valid. Deletes resource if it is not valid.
		*	\param[in]	_Id	Identificator of resource to be processed.
		*	\throw nothrow
		*	\return Return of Load call is resource passes check, false on exception or not pass.
		**/
		inline bool reloadResource(const ResourceID _Id) NOEXCEPT {
			if (checkResourceAll(_Id)) {
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
		bool reloadResource(const ResourceID _Id[], const unsigned int _count, bool _result[] = nullptr) NOEXCEPT;

		/**
		*	\brief Performs an attempt to call Reload function of all valid resources.
		*	NORMAL/STRICT : Doesn't delete resource if it is invalid.
		*	\param[out]	_count	Size of returned array.
		*	\throw nothrow
		*	\return Return std::unique_ptr to array of process statuses.
		**/
		std::unique_ptr<bool[]> reloadAll(unsigned int& _count) NOEXCEPT;

		/**
		*	\brief Performs an attempt to call Reload function of all valid resources.
		*	NORMAL/STRICT : Doesn't delete resource if it is invalid.
		*	\throw nothrow
		*	\return Return true if and only if all valid resources are successfully processed.
		**/
		bool reloadAll() NOEXCEPT;
		
		/**
		*	\brief Preforms garbage collection round over handled objects with specified '_bandwidth'.
		*	Negative '_bandwidth' is same as "delete as many as you can".
		*	\param[in]	_relMemo	Count of relesed memory in bytes(not overflow protected). 
		*	\param[in]	_bandwidth	Max count of objects to be deleted during round.
		*	\throw unknown
		*	\return Real count of deleted objects.
		**/
		unsigned int collectGarbage(unsigned int& _relMemo, int _bandwidth = -1);

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
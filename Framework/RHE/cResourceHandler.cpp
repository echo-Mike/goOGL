#include "cResourceHandler.h"

namespace resources {

	/**
	*	\brief Counts amount of handled memory.
	*	Corrects possible uint overflow with '_cary' parameter.
	*	Linear complexity.
	*	\param[out]	_cary	Carry uint to correct uint overflow.
	*	\throw Ignore
	*	\return Summed up amount of memory used by handled objects.
	**/
	unsigned int ResourceHandler::memoryHandled(unsigned int& _cary) {
		_cary = 0;
		unsigned int _result = 0;
		unsigned int _buff = 0;
		for (auto& v : storage) {
			_buff = v.second->usedMemory();
			if (_result + _buff < _result)
				_cary++;
			_result += _buff;
		}
		return _result;
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
	bool ResourceHandler::checkResource(const ResourceID _Id, int _flags) NOEXCEPT {
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
						//warning C4800: "!= 0" part needed.
						//See for detail: https://msdn.microsoft.com/ru-ru/library/b6801kcy.aspx
						return (_status & _flags) != 0;
						break;
				}
			}
			//Delete resource if it is invalid
			//Hardcoded solution
			forceDelete(_Id);
		}
		return false;
	};

	bool checkAll(const ResourceID _Id, int _upFlags, int _downFlags) NOEXCEPT {
		return true;
	};

	bool checkAny(const ResourceID _Id, int _upFlags, int _downFlags) NOEXCEPT {
		return true;
	};

	/**
	*	\brief Force removal of resource with id '_Id'.
	*	NORMAL/STRICT : Ignores all strict rules for deleteResource.
	*	This function is for internal use only.
	*	\param[in]	_Id	Identificator of resource to be deleted.
	*	\throw nothrow
	*	\return Nothing
	**/
	void ResourceHandler::forceDelete(const ResourceID _Id) NOEXCEPT {
		//TODO::DELETE AFTER TEST!!!
		//owner->secureRemove(_Id, this);
		storage.erase(_Id);
	};


	/**
	*	\brief Performs an attempt to call Load function of '_count' resources with id in array '_Id'.
	*	NORMAL/STRICT : Derives behaviour from loadResource.
	*	\param[in]	_Id		Array of identificators of resource to be processed.
	*	\param[in]	_count	Count of resources to be processed.
	*	\param[out]	_result	[Optional] Array for every single process result.
	*	\throw nothrow
	*	\return Return true if and only if all resources are successfully processed.
	**/
	bool ResourceHandler::loadResource(const ResourceID _Id[], const unsigned int _count, bool _result[]) NOEXCEPT {
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
	std::unique_ptr<bool[]> ResourceHandler::loadAll(unsigned int& _count) NOEXCEPT {
		//Return size via _count
		_count = storage.size();
		//Allocate new array of bool
		std::unique_ptr<bool[]> _result(new bool[_count]);
		//Some counter to adsress array
		unsigned int _counter = 0;
		for (auto &v : storage) {
			//There is undefined behaviour if call forceDelete during for (auto &v : storage)
			//So just don't process invalid resources.
			if (v.second->status & Resource::ResourceStatus::INVALID) {
				_result[_counter] = false;
				_counter++;
				continue;
			}
			try { _result[_counter] = v.second->Load(); }
			catch (const std::exception& e) {
				#if defined(DEBUG_RESOURCEHANDLER) && defined(RESOURCEHANDLER_MINOR_ERRORS)
					DEBUG_NEW_MESSAGE("ERROR::RESOURCE_HANDLER::loadAll")
						DEBUG_WRITE1("\tMessage: Error occurred during call to Load function. Exception captured.");
						DEBUG_WRITE2("\tResource id: ", v.first);
						DEBUG_WRITE2("\tException content:", e.what());
					DEBUG_END_MESSAGE
				#endif
				_result[_counter] = false;
			}
			catch (...) {
				#if defined(DEBUG_RESOURCEHANDLER) && defined(RESOURCEHANDLER_MINOR_ERRORS)
					DEBUG_NEW_MESSAGE("ERROR::RESOURCE_HANDLER::loadAll")
						DEBUG_WRITE1("\tMessage: Error occurred during call to Load function. Something was thrown.");
						DEBUG_WRITE2("\tResource id: ", v.first);
					DEBUG_END_MESSAGE
				#endif
				_result[_counter] = false;
			}
			_counter++;
		}
		return std::move(_result);
	}

	/**
	*	\brief Performs an attempt to call Load function of all valid resources.
	*	NORMAL/STRICT : Doesn't delete resource if it is invalid.
	*	\throw nothrow
	*	\return Return true if and only if all valid resources are successfully processed.
	**/
	bool ResourceHandler::loadAll() NOEXCEPT {
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
	*	\brief Performs an attempt to call Unload function of '_count' resources with id in array '_Id'.
	*	NORMAL/STRICT : Derives behaviour from unloadResource.
	*	\param[in]	_Id		Array of identificators of resource to be processed.
	*	\param[in]	_count	Count of resources to be processed.
	*	\param[out]	_result	[Optional] Array for every single process result.
	*	\throw nothrow
	*	\return Return true if and only if all resources are successfully processed.
	**/
	bool ResourceHandler::unloadResource(const ResourceID _Id[], const unsigned int _count, bool _result[]) NOEXCEPT {
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
	std::unique_ptr<bool[]> ResourceHandler::unloadAll(unsigned int& _count) NOEXCEPT {
		//Return size via _count
		_count = storage.size();
		//Allocate new array of bool
		std::unique_ptr<bool[]> _result(new bool[_count]);
		//Some counter to adsress array
		unsigned int _counter = 0;
		for (auto &v : storage) {
			//There is undefined behaviour if call forceDelete during for (auto &v : storage)
			//So just don't process invalid resources.
			if (v.second->status & Resource::ResourceStatus::INVALID) {
				_result[_counter] = false;
				_counter++;
				continue;
			}
			try { _result[_counter] = v.second->Unload(); }
			catch (const std::exception& e) {
				#if defined(DEBUG_RESOURCEHANDLER) && defined(RESOURCEHANDLER_MINOR_ERRORS)
					DEBUG_NEW_MESSAGE("ERROR::RESOURCE_HANDLER::unloadAll")
						DEBUG_WRITE1("\tMessage: Error occurred during call to Unload function. Exception captured.");
						DEBUG_WRITE2("\tResource id: ", v.first);
						DEBUG_WRITE2("\tException content:", e.what());
					DEBUG_END_MESSAGE
				#endif
				_result[_counter] = false;
			}
			catch (...) {
				#if defined(DEBUG_RESOURCEHANDLER) && defined(RESOURCEHANDLER_MINOR_ERRORS)
					DEBUG_NEW_MESSAGE("ERROR::RESOURCE_HANDLER::unloadAll")
						DEBUG_WRITE1("\tMessage: Error occurred during call to Unload function. Something was thrown.");
						DEBUG_WRITE2("\tResource id: ", v.first);
					DEBUG_END_MESSAGE
				#endif
				_result[_counter] = false;
			}
			_counter++;
		}
		return std::move(_result);
	}

	/**
	*	\brief Performs an attempt to call Unload function of all valid resources.
	*	NORMAL/STRICT : Doesn't delete resource if it is invalid.
	*	\throw nothrow
	*	\return Return true if and only if all valid resources are successfully processed.
	**/
	bool ResourceHandler::unloadAll() NOEXCEPT {
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
	*	\brief Performs an attempt to call Reload function of '_count' resources with id in array '_Id'.
	*	NORMAL/STRICT : Derives behaviour from reloadResource.
	*	\param[in]	_Id		Array of identificators of resource to be processed.
	*	\param[in]	_count	Count of resources to be processed.
	*	\param[out]	_result	[Optional] Array for every single process result.
	*	\throw nothrow
	*	\return Return true if and only if all resources are successfully processed.
	**/
	bool ResourceHandler::reloadResource(const ResourceID _Id[], const unsigned int _count, bool _result[]) NOEXCEPT {
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
	std::unique_ptr<bool[]> ResourceHandler::reloadAll(unsigned int& _count) NOEXCEPT {
		//Return size via _count
		_count = storage.size();
		//Allocate new array of bool
		std::unique_ptr<bool[]> _result(new bool[_count]);
		//Some counter to adsress array
		unsigned int _counter = 0;
		for (auto &v : storage) {
			//There is undefined behaviour if call forceDelete during for (auto &v : storage)
			//So just don't process invalid resources.
			if (v.second->status & Resource::ResourceStatus::INVALID) {
				_result[_counter] = false;
				_counter++;
				continue;
			}
			try { _result[_counter] = v.second->Reload(); }
			catch (const std::exception& e) {
				#if defined(DEBUG_RESOURCEHANDLER) && defined(RESOURCEHANDLER_MINOR_ERRORS)
					DEBUG_NEW_MESSAGE("ERROR::RESOURCE_HANDLER::reloadAll")
						DEBUG_WRITE1("\tMessage: Error occurred during call to Reload function. Exception captured.");
						DEBUG_WRITE2("\tResource id: ", v.first);
						DEBUG_WRITE2("\tException content:", e.what());
					DEBUG_END_MESSAGE
				#endif
				_result[_counter] = false;
			}
			catch (...) {
				#if defined(DEBUG_RESOURCEHANDLER) && defined(RESOURCEHANDLER_MINOR_ERRORS)
					DEBUG_NEW_MESSAGE("ERROR::RESOURCE_HANDLER::reloadAll")
						DEBUG_WRITE1("\tMessage: Error occurred during call to Reload function. Something was thrown.");
						DEBUG_WRITE2("\tResource id: ", v.first);
					DEBUG_END_MESSAGE
				#endif
				_result[_counter] = false;
			}
			_counter++;
		}
		return std::move(_result);
	}

	/**
	*	\brief Performs an attempt to call Reload function of all valid resources.
	*	NORMAL/STRICT : Doesn't delete resource if it is invalid.
	*	\throw nothrow
	*	\return Return true if and only if all valid resources are successfully processed.
	**/
	bool ResourceHandler::reloadAll() NOEXCEPT {
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
}
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
	bool ResourceHandler::checkResourceAll(const ResourceID _Id, int _upFlags, int _downFlags) NOEXCEPT {
		auto _iterator = storage.find(_Id);
		if (_iterator == storage.end())
			if (_downFlags & ResourceCheckFlags::PRESENTED && !(_upFlags & ResourceCheckFlags::PRESENTED)) {
				return true;
			} else {
				return false;
			}
		_upFlags &= ~ResourceCheckFlags::PRESENTED;
		_downFlags &= ~ResourceCheckFlags::PRESENTED;
		if (!(_upFlags || _downFlags))
			return true;
		//Obtain status of resource
		auto _status = _iterator->second->status;
		/**
		*	Check goal: All _upFlags are UP and ALL _downFlags are DOWN.
		*	x1 - status;	x2 - flag
		*	Flag up:		Flag down:		Together:
		*	|x1|x2|f1|		|x1|x2|f2|		|f1|f2|f3|
		*	| 0| 0| 1|		| 0| 0| 1|		| 0| 0| 0|
		*	| 0| 1| 0|		| 0| 1| 1|		| 0| 1| 0|
		*	| 1| 0| 1|		| 1| 0| 1|		| 1| 0| 0|
		*	| 1| 1| 1|		| 1| 1| 0|		| 1| 1| 1|
		*	!(~x1 & x2)		!(x1 & x2)		f1 & f2		=>
		*	!(~x1 & x2) & !(x1 & x2) == !((~x1 & x2) | (x1 & x2))
		*	_checkStatus = (~x1 & x2) | (x1 & x2);
		**/
		auto _checkStatus = (~_status & _upFlags) | (_status & _downFlags);
		bool _result(true);
		for (unsigned int _shifter = 0; ResourceCheckFlags::MAX >> _shifter > 0; _shifter++)
			_result &= !(_checkStatus >> _shifter & 0x1);
		if (_status & ResourceCheckFlags::INVALID)
			forceDelete(_Id);
		return _result;
	};

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
	bool ResourceHandler::checkResourceAny(const ResourceID _Id, int _upFlags, int _downFlags) NOEXCEPT {
		auto _iterator = storage.find(_Id);
		if (_iterator == storage.end())
			if (_downFlags & ResourceCheckFlags::PRESENTED && !(_upFlags & ResourceCheckFlags::PRESENTED)) {
				return true;
			} else {
				return false;
			}
		_upFlags &= ~ResourceCheckFlags::PRESENTED;
		_downFlags &= ~ResourceCheckFlags::PRESENTED;
		if (!(_upFlags || _downFlags))
			return true;
		//Obtain status of resource
		auto _status = _iterator->second->status;
		bool _result = (_status & _upFlags) || (~_status & _downFlags);
		if (_status & ResourceCheckFlags::INVALID)
			forceDelete(_Id);
		return _result;
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

	/**
	*	\brief Preforms garbage collection round over handled objects with specified '_bandwidth'.
	*	Negative '_bandwidth' is same as "delete as many as you can".
	*	STRICT:	Resources with more then one allocated shared_ptr to it not erased.
	*	\param[in]	_relMemo	Count of relesed memory in bytes(not overflow protected).
	*	\param[in]	_bandwidth	Max count of objects to be deleted during round.
	*	\throw unknown
	*	\return Real count of deleted objects.
	**/
	unsigned int ResourceHandler::collectGarbage(unsigned int& _relMemo, int _bandwidth) {
		_relMemo = 0;
		if (!_bandwidth)
			return 0;
		if (_bandwidth < 0)
			_bandwidth = storage.size();
		auto _predicate = [](Storage::iterator& _iterator) -> bool { return _iterator->second || _iterator->second->getStatus() & ResourceCheckFlags::INVALID;};
		auto _iterator  = storage.begin();
		auto _siterator = storage.begin();
		auto _eiterator = storage.end();
		for (int _count = 0; _count < _bandwidth; _count++) {
			_siterator = std::find_if(_siterator, _eiterator, _predicate);
			if (_iterator == _eiterator)
				return _count;
			//Postincrement operation not defined by standard for 'Iterator' object but preincrement is.
			_iterator = _siterator;
			++_siterator;
			#ifdef RESOURCE_HANDLER_STRICT
			if (_iterator->second.use_count() > 1)
				continue;
			#endif // RESOURCE_HANDLER_STRICT
			_relMemo += _iterator->second->usedMemory();
			storage.erase(_iterator);
		}
		return _bandwidth;
	}
}
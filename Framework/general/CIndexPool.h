#ifndef INDEXPOOL_H
#define INDEXPOOL_H "[multi@CIndexPool.h]"
/*
*	DESCRIPTION:
*		Module contains implementation of index handler class.
*	AUTHOR:
*		Mikhail Demchenko
*		mailto:dev.echo.mike@gmail.com
*		https://github.com/echo-Mike
*/
//STD
#include <vector>
#include <algorithm>
#include <type_traits>
//OUR
#include "general\vs2013tweaks.h"
#include "general\mConcepts.hpp"
//DEBUG
#if defined(DEBUG_INDEXPOOL) && !defined(OTHER_DEBUG)
	#include "general\mDebug.h"		
#elif defined(DEBUG_INDEXPOOL) && defined(OTHER_DEBUG)
	#include OTHER_DEBUG
#endif

/* The automatic index manager.
*  Class template definition: IndexPool
*/
template < typename TIndex = int >
class IndexPool {
	typedef std::vector<TIndex> Container;
	//Sorted list of allocated indexes
	Container allocated;
	//Sorted list of ignored indexes
	Container ignored;
	//Sorted list of indexes ready to use
	Container pool;
	TIndex increment;
	TIndex poolTop;
public:
	typedef TIndex IndexType;
	const TIndex minIndex;
	const TIndex maxIndex;
	const unsigned int size;

	IndexPool(	TIndex _min, TIndex _max, unsigned int _size) : 
				minIndex(_min), maxIndex(_max), size(_size) 
	
	{
		if (_min > _max) 
			throw std::invalid_argument("ERROR::INDEX_POOL::MININDEX_IS_GRATER_THAN_MAXINDEX");
		if (minIndex < (TIndex)0)
			throw std::invalid_argument("ERROR::INDEX_POOL::MININDEX_TO_LOW");
		poolTop = minIndex;
		increment = (maxIndex - minIndex) / size; 
	}

	TIndex newIndex() {
		auto _ignend = ignored.end();
		auto _ignbeg = ignored.begin();

		//Try to allocate from index pool
		if (pool.size() > 0) {
			TIndex _lastUsed = pool.back();
			bool _success = true;
			while (_ignend != std::find(_ignbeg, _ignend, _lastUsed)) {
				pool.pop_back();
				if (pool.size() > 0) {
					_lastUsed = pool.back();
				} else {
					_success = false;
					break;
				}
			}
			if (_success) {
				allocated.push_back(_lastUsed);
				pool.pop_back();
				return _lastUsed;
			}
		} 

		//Try to allocate new index
		TIndex _buff = poolTop;
		if (allocated.size() != 0)
			_buff += increment;
		while (_ignend != std::find(_ignbeg, _ignend, _buff) && _buff < maxIndex)
			_buff += increment;
		if (_buff >= maxIndex) {
			throw std::out_of_range("ERROR::INDEX_POOL::CAN'T_ALLOCATE_NEW_INDEX");
			return (TIndex)-1;
		} else {
			poolTop = _buff;
			allocated.push_back(_buff);
			return _buff;
		}
	}

	unsigned int newIndex(TIndex _array[], unsigned int _count) {
		for (int _index = 0; _index < (int)_count; _index++) {
			try {
				_array[_index] = newIndex();
			}
			catch (std::out_of_range e) {
				#ifdef DEBUG_INDEXPOOL
					#ifdef WARNINGS_INDEXPOOL
						DEBUG_OUT << "WARNING::INDEX_POOL::NO_MORE_INDEXES_CAN_BE_ALLOCATED" << DEBUG_NEXT_LINE;
						DEBUG_OUT << "\tMessage: Final allocated count: " << _index << DEBUG_NEXT_LINE;
					#endif
				#endif
				return _index;
			}
		}
		return _count;
	}

	void deleteIndex(TIndex _index) {
		auto _toDelete = std::find(allocated.begin(), allocated.end(), _index);
		if (_toDelete == allocated.end()) {
			#ifdef DEBUG_INDEXPOOL
				#ifdef WARNINGS_INDEXPOOL
					DEBUG_OUT << "WARNING::INDEX_POOL::CAN'T_FIND_INDEX_TO_DELETE" << DEBUG_NEXT_LINE;
					DEBUG_OUT << "\tMessage: Index: " << _index << " isn't allocated." << DEBUG_NEXT_LINE;
				#endif
			#endif
		} else {
			auto _isIgnored = std::find(ignored.begin(), ignored.end(), *_toDelete);
			if (_isIgnored != ignored.end())
				pool.push_back(*_toDelete);
			allocated.erase(_toDelete);
		}
	}

	inline bool isValid(TIndex _index) {
		return  _index >= minindex && std::fmod(_index - minIndex, increment) == (TIndex)0 && _index < maxIndex;
	}

	inline void ignore(TIndex _index) {
		if (isValid(_index))
			ignored.push_back(_index);
	}

	void ignore(TIndex _start, TIndex _end) {
		TIndex _low = std::round((_start - minIndex) / (double)increment) * increment + minIndex;
		TIndex _high = std::round((_end - minIndex) / (double)increment) * increment + minIndex;
		for (TIndex _index = _low; _index <= _high; _index += increment)
			ignored.push_back(_index);
	}

	inline bool isIgnored(TIndex _index) {
		return std::find(ignored.begin(), ignored.end(), _index) != ignored.end() || !isValid(_index);
	}

	void allow(TIndex _index) {
		auto _toAllow = std::find(ignored.begin(), ignored.end(), _index);
		if (_toAllow != ignored.end()) {
			pool.push_back(*_toAllow);
			ignored.erase(_toAllow);
		}
	}

	void allow(TIndex _start, TIndex _end) {
		TIndex _low = std::round((_start - minIndex) / (double)increment) * increment + minIndex;
		TIndex _high = std::round((_end - minIndex) / (double)increment) * increment + minIndex;
		Container::iterator _toAllow;
		for (TIndex _index = _low; _index <= _high; _index += increment) {
			_toAllow = std::find(ignored.begin(), ignored.end(), _index);
			if (_toAllow != ignored.end()) {
				pool.push_back(*_toAllow);
				ignored.erase(_toAllow);
			}
		}
	}

	inline bool isAllowed(TIndex _index) { return !isIgnored(_index); }
};
#endif
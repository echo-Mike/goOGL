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
#include <cmath>
#include <limits.h>
#include <vector>
#include <algorithm>
#include <type_traits>
#include <cstring>
//DEBUG
#ifdef DEBUG_INDEXPOOL
	#ifndef DEBUG_OUT
		#define DEBUG_OUT std::cout
	#endif
	#ifndef DEBUG_NEXT_LINE
		#define DEBUG_NEXT_LINE std::endl
	#endif
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
					DEBUG_OUT << "WARNING::INDEX_POOL::NO_MORE_INDEXES_CAN_BE_ALLOCATED" << DEBUG_NEXT_LINE;
					DEBUG_OUT << "\tMessage: Final allocated count: " << _index << DEBUG_NEXT_LINE;
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
				DEBUG_OUT << "WARNING::INDEX_POOL::CAN'T_FIND_INDEX_TO_DELETE" << DEBUG_NEXT_LINE;
				DEBUG_OUT << "\tMessage: Index: " << _index << " isn't allocated." << DEBUG_NEXT_LINE;
			#endif
		} else {
			auto _isIgnored = std::find(ignored.begin(), ignored.end(), *_toDelete);
			if (_isIgnored != ignored.end())
				pool.push_back(*_toDelete);
			allocated.erase(_toDelete);
		}
	}

	bool isValid(TIndex _index) {
		return  _index >= minindex && std::fmod(_index - minIndex, increment) == (TIndex)0 && _index < maxIndex;
	}

	void ignore(TIndex _index) {
		if (isValid(_index))
			ignored.push_back(_index);
	}

	void ignore(TIndex _start, TIndex _end) {
		TIndex _low = std::round((_start - minIndex) / (double)increment) * increment + minIndex;
		TIndex _high = std::round((_end - minIndex) / (double)increment) * increment + minIndex;
		for (TIndex _index = _low; _index <= _high; _index += increment)
			ignored.push_back(_index);
	}

	bool isIgnored(TIndex _index) {
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

	bool isAllowed(TIndex _index) { return !isIgnored(_index); }
};

template < class TIndex = int >
class SimpleIndexPool {
	static_assert(	std::is_integral<TIndex>::value,
					"ASSERTION_ERROR::SIMPLE_INDEX_POOL::Provided type 'TIndex' must be integral.");
	typedef unsigned int Bucket;
	Bucket* pool;
	unsigned int size;
	unsigned int bucketBitSize;
	unsigned int length;
	unsigned int tail;
	TIndex minIndex;
	TIndex maxIndex;
public:
	typedef TIndex IndexType;

	SimpleIndexPool(TIndex _min, TIndex _max) : 
					minIndex(_min), maxIndex(_max)
	{
		if (_min > _max) {
			maxIndex = _min;
			minIndex = _max;
		}

		size = maxIndex - minIndex + 1;
		bucketBitSize = sizeof(Bucket) * CHAR_BIT;

		length = size / bucketBitSize;
		tail = size % bucketBitSize;
		if (tail)
			length++;

		pool = new Bucket[length];
		if (!pool)
			std::exception("SIMPLE_INDEX_POOL::System can't allocate memory.");

		std::memset(pool, 0, length * sizeof(Bucket));
	}

	SimpleIndexPool(const SimpleIndexPool& other) : minIndex(other.minIndex),
													maxIndex(other.maxIndex),
													size(other.size),
													tail(other.tail),
													length(other.length)
	{
		pool = new Bucket[length];
		std::memcpy(pool, other.pool, length * sizeof(Bucket));
	}

	SimpleIndexPool(SimpleIndexPool&& other) :  minIndex(other.minIndex),
												maxIndex(std::move(other.maxIndex)),
												size(std::move(other.size)),
												pool(other.pool),
												tail(std::move(other.tail)),
												length(std::move(other.length))
	{
		other.pool = nullptr;
	}

	~SimpleIndexPool() { delete pool; }

	unsigned int getSize() { return size; }

	TIndex getMinIndex() { return minIndex; }

	TIndex getMaxIndex() { return maxIndex; }

	TIndex newIndex(unsigned int startFrom = 0) {
		if (startFrom >= size)
			startFrom = size - 1;
		register Bucket* ptr(pool + startFrom / bucketBitSize);

		for (;;) {
			if (ptr == pool + length)
				break;

			if (tail && ptr == pool + length - 1) {
				for (register unsigned int _index = 0; _index < tail; _index++) {
					if (!(*ptr & (((Bucket)1) << _index))) {
						*ptr |= ((Bucket)1) << _index;
						return (TIndex)((ptr - pool) * bucketBitSize + _index + minIndex);
					}
				}
			} else {
				for (register unsigned int _index = 0; _index < bucketBitSize; _index++) {
					if (!(*ptr & (((Bucket)1) << _index))) {
						*ptr |= ((Bucket)1) << _index;
						return (TIndex)((ptr - pool) * bucketBitSize + _index + minIndex);
					}
				}
			}

			ptr++;
		}

		throw std::out_of_range("ERROR::SIMPLE_INDEX_POOL::CAN'T_ALLOCATE_NEW_INDEX");
		return (TIndex)0;
	}

	unsigned int newIndex(TIndex _array[], unsigned int _count) {
		try {
			_array[0] = newIndex(0);
		}
		catch (std::out_of_range e) {
			#ifdef DEBUG_INDEXPOOL
				#ifdef WARNINGS_INDEXPOOL
					DEBUG_OUT << "WARNING::SIMPLE_INDEX_POOL::NO_MORE_INDEXES_CAN_BE_ALLOCATED" << DEBUG_NEXT_LINE;
					DEBUG_OUT << "\tMessage: Final allocated count: 0" << DEBUG_NEXT_LINE;
				#endif
			#endif
			return 0;
		}
		for (unsigned int _index = 1; _index < _count; _index++) {
			try {
				_array[_index] = newIndex(_array[_index-1] - minIndex + 1);
			}
			catch (std::out_of_range e) {
				#ifdef DEBUG_INDEXPOOL
					#ifdef WARNINGS_INDEXPOOL
						DEBUG_OUT << "WARNING::SIMPLE_INDEX_POOL::NO_MORE_INDEXES_CAN_BE_ALLOCATED" << DEBUG_NEXT_LINE;
						DEBUG_OUT << "\tMessage: Final allocated count: " << _index << DEBUG_NEXT_LINE;
					#endif
				#endif
				return _index;
			}
		}
		return _count;
	}

	void deleteIndex(TIndex _index) {
		if (_index < minIndex || _index > maxIndex)
			return;

		Bucket* ptr = pool + (_index - minIndex) / bucketBitSize;

		unsigned int _tail = (_index - minIndex) % bucketBitSize;
		
		if (*ptr & (((Bucket)1) << _tail))
			*ptr &= (!((Bucket)0)) ^ (((Bucket)1) << _tail) ;
		return;
	}

	void deleteIndex(TIndex _start, TIndex _end) {
		if (_start > _end) 
			_start ^= _end ^= _start ^= _end;
		if (_start < minIndex)
			_start = minIndex;
		if (_end > maxIndex)
			_end = maxIndex;
		for (TIndex _index = _start; _index <= _end; _index++)
			deleteIndex(_index);
	}
};
#endif
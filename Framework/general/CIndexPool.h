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
#include <string.h>
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


template < class TIndex = int >
/**
*	This class performs time efficient index handling with simple algorithms.
*	Only integral index types are accepted with constant increment step (operaor++).
*	Heavily depends on CHAR_BIT macro from <limits.h>
*	Class template definition: SimpleIndexPool
**/
class SimpleIndexPool {
	static_assert(	std::is_integral<TIndex>::value,
					"ASSERTION_ERROR::SIMPLE_INDEX_POOL::Provided type 'TIndex' must be integral.");
	typedef unsigned int Bucket;
	//Storage of allocation data
	Bucket* pool;
	//Fast allocation access pointer
	Bucket* currentPosition;
	//Max possible count of allocated indexes 
	unsigned int size;
	//Computed bucket size in bits
	unsigned int bucketBitSize;
	//Length of allocated bucket array
	unsigned int length;
	//Count of used bits in last bucket
	unsigned int tail;
	//Maximal and minimal possible indexes
	TIndex minIndex;
	TIndex maxIndex;

	/**
	*	Performs index search based on last linear index search defined by 'currentPosition' pointer.
	*	Accepts ternary variant with SIMPLEINDEXPOOL_TERNARY macro.
	**/
	TIndex newIndexFromCurrentPosition() {
		register Bucket* ptr(currentPosition);
		register Bucket* _end(pool + length);
		while (ptr != _end) {
			#ifdef SIMPLEINDEXPOOL_TERNARY
				if (~(*ptr)) {
					for (	register unsigned int _index = 0; _index < ( tail && ptr == _end - 1 ? tail : bucketBitSize); _index++) {
						if (!(*ptr & (((Bucket)1) << _index))) {
							*ptr |= ((Bucket)1) << _index;
							currentPosition = ptr;
							return (TIndex)((ptr - pool) * bucketBitSize + _index + minIndex);
						}
					}
				}
			#else
				if (tail && ptr == _end - 1) {
					//Same algorithm as below but for smaller tail bucket
					for (register unsigned int _index = 0; _index < tail; _index++) {
						if (!(*ptr & (((Bucket)1) << _index))) {
							*ptr |= ((Bucket)1) << _index;
							currentPosition = ptr;
							return (TIndex)((ptr - pool) * bucketBitSize + _index + minIndex);
						}
					}
				} else if (~(*ptr)) { //Enable bit-check only if 0 bits presented in current bucket
					for (register unsigned int _index = 0; _index < bucketBitSize; _index++) {
						//Forming mask and checking bit
						if (!(*ptr & (((Bucket)1) << _index))) {
							//Setting founded bit to 1
							*ptr |= ((Bucket)1) << _index;
							//Save current position of linear search
							currentPosition = ptr;
							//Compute corresponding index
							return (TIndex)((ptr - pool) * bucketBitSize + _index + minIndex);
						}
					}
				}
			#endif
			ptr++;
		}
		throw std::out_of_range("ERROR::SIMPLE_INDEX_POOL::newIndexFromCurrentPosition::New index not found.");
		return (TIndex)0;
	}

	/**
	*	Preforms index search from start of index interval.
	*	Accepts ternary variant with SIMPLEINDEXPOOL_TERNARY macro.
	**/
	TIndex newIndexFromStart() {
		//Reset position of linear search to zero
		currentPosition = pool;
		//Preform a linear search
		try { return newIndexFromCurrentPosition(); }
		catch (...) { 
			throw std::out_of_range("ERROR::SIMPLE_INDEX_POOL::newIndexFromStart::New index not found.");
			return (TIndex)0;
		}
	}

	/**
	*	Preforms index search from provided starting position 'startFrom'.
	*	Explanation: This function contain a lot of reused code segments because all solutions with 
	*	ternary operator ?:; generates slower machine code and will perform a lot of
	*	unnecessary checks.
	*	Accepts ternary variant with SIMPLEINDEXPOOL_TERNARY macro.
	**/
	TIndex newIndexFromProvidedPosition(unsigned int startFrom) {
		//Get the bucket of index in position 'startFrom'
		register Bucket* ptr(pool + startFrom / bucketBitSize);
		#ifdef SIMPLEINDEXPOOL_TERNARY
			Bucket* _start = pool + startFrom / bucketBitSize;
			Bucket* _back  = pool + length - 1;
			Bucket* _end   = _back + 1;
			while (ptr != _end) {
				if (~(*ptr)) {
					for (	register unsigned int _index = (ptr == _start ? startFrom % bucketBitSize : 0);
							_index < ( tail && ptr == _back ? tail : bucketBitSize); 
							_index++) 
					{
						if (!(*ptr & (((Bucket)1) << _index))) {
							*ptr |= ((Bucket)1) << _index;
							return (TIndex)((ptr - pool) * bucketBitSize + _index + minIndex);
						}
					}
				}

				ptr++;
			}
		#else
			//First pass: set _index to "startFrom % bucketBitSize" to not spend processor cycles
			//Check for tail position
			if (tail && ptr == pool + length - 1) {
				for (register unsigned int _index = startFrom % bucketBitSize; _index < tail; _index++) {
					if (!(*ptr & (((Bucket)1) << _index))) {
						*ptr |= ((Bucket)1) << _index;
						return (TIndex)((ptr - pool) * bucketBitSize + _index + minIndex);
					}
				}
			//If current bucket is not full then pass thru it
			} else if (~(*ptr)) {
				for (register unsigned int _index = startFrom % bucketBitSize; _index < bucketBitSize; _index++) {
					if (!(*ptr & (((Bucket)1) << _index))) {
						*ptr |= ((Bucket)1) << _index;
						return (TIndex)((ptr - pool) * bucketBitSize + _index + minIndex);
					}
				}
			}

			ptr++;

			//Next passes: there is no need to set _index to "startFrom % bucketBitSize"
			while (ptr != pool + length) {

				if (tail && ptr == pool + length - 1) {
					for (register unsigned int _index = 0; _index < tail; _index++) {
						if (!(*ptr & (((Bucket)1) << _index))) {
							*ptr |= ((Bucket)1) << _index;
							return (TIndex)((ptr - pool) * bucketBitSize + _index + minIndex);
						}
					}
				} else if (~(*ptr)) {
					for (register unsigned int _index = 0; _index < bucketBitSize; _index++) {
						if (!(*ptr & (((Bucket)1) << _index))) {
							*ptr |= ((Bucket)1) << _index;
							return (TIndex)((ptr - pool) * bucketBitSize + _index + minIndex);
						}
					}
				}

				ptr++;
			}
		#endif
		throw std::out_of_range("ERROR::SIMPLE_INDEX_POOL::newIndexFromProvidedPosition::New index not found.");
		return (TIndex)0;
	}
protected:
	/**
	*	Internal function for performing trustful index data change.
	**/
	void allocateSpecific(TIndex _array[], unsigned int _length) {
		//Bucket position
		register Bucket* ptr = nullptr;
		//Bit position
		register unsigned int _tail = 0;
		for (unsigned int _index = 0; _index < _length; _index++) {
			ptr = pool + (_array[_index] - minIndex) / bucketBitSize;
			_tail = (_array[_index] - minIndex) % bucketBitSize;
			//Set bit to one
			*ptr |= ((Bucket)1) << _tail;
		}
	}
public:
	//Type of handeled index
	typedef TIndex IndexType;

	/**
	*	Helper structure for allocating all remaining indexes
	**/
	struct MaxAllocationHelper {
		//Array of pointers to allocated indexes
		TIndex** partitions;
		//Length of partitions array
		unsigned int allocatedCount;
		//Count of partitions used elements (TIndex*)
		unsigned int realCount;
		//Length of every non-tail array in partitions[]
		unsigned int length;
		//Lenth of tail array in partitions[realCount-1]
		unsigned int tailLength;
		//Total count of allocated indexes
		unsigned int totalAllocatedCount;
		
		MaxAllocationHelper() {};

		MaxAllocationHelper(MaxAllocationHelper&& other) : 
							partitions(std::move(other.partitions)),
							allocatedCount(std::move(other.allocatedCount)),
							realCount(std::move(other.realCount)),
							length(std::move(other.length)),
							tailLength(std::move(other.tailLength)),
							totalAllocatedCount(std::move(other.totalAllocatedCount)) 
		{
			other.partitions = nullptr;
		}
		//Proper memory cleaning
		~MaxAllocationHelper() {
			if (partitions)
				for (unsigned int _index = 0; _index < realCount; _index++)
					delete partitions[_index];
			delete partitions;
		}
	};

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

		currentPosition = pool;
	}

	SimpleIndexPool(const SimpleIndexPool& other) : minIndex(other.minIndex),
													maxIndex(other.maxIndex),
													size(other.size),
													tail(other.tail),
													length(other.length)
	{
		pool = new Bucket[length];
		std::memcpy(pool, other.pool, length * sizeof(Bucket));
		currentPosition = pool + (other.currentPosition - other.pool);
	}

	SimpleIndexPool(SimpleIndexPool&& other) :  minIndex(other.minIndex),
												maxIndex(std::move(other.maxIndex)),
												size(std::move(other.size)),
												pool(other.pool),
												currentPosition(std::move(other.currentPosition)),
												tail(std::move(other.tail)),
												length(std::move(other.length))
	{
		other.pool = nullptr;
	}

	~SimpleIndexPool() { delete pool; }

	/**
	*	Provides read access to 'size' value.
	**/
	unsigned int getSize() { return size; }

	/**
	*	Provides read access to 'minIndex' value.
	**/
	TIndex getMinIndex() { return minIndex; }

	/**
	*	Provides read access to 'maxIndex' value.
	**/
	TIndex getMaxIndex() { return maxIndex; }

	/**
	*	Computes mamory used by this pool.
	**/
	size_t usedMemory() { return length * sizeof(Bucket) + sizeof(SimpleIndexPool); }

	/**
	*	Performs a search of new index.
	*	Value of 'startFrom' defines behaviour of search:
	*	-- startFrom < 0 -- : Performs linear search based on last linear search request.
	*	-- startFrom == 0-- : Performs linear search from start of index interval.
	*	-- startFrom > 0 -- : Performs search from provided index place startFrom :
	*	---- Index places are marked from 0 to obj.size-1.
	*	---- Use 'obj.startLocation(last_given_index)' to obtain a new location for linear index search.
	*	Exceptions: std::out_of_range if no more indexes can be allocated.
	**/
	TIndex newIndex(int startFrom) {
		if (startFrom >= (int)size)
			startFrom = size - 1;
		if (!startFrom) {
			try { return newIndexFromStart(); }
			catch (...) { throw; }
		}
		try { return (startFrom < 0 ? newIndexFromCurrentPosition() : newIndexFromProvidedPosition(startFrom)); }
		catch (...) { 
			try { return newIndexFromStart(); }
			catch (...) { 
				throw std::out_of_range("ERROR::SIMPLE_INDEX_POOL::newIndex::Can't allocate more indexes.");
				return (TIndex)0;
			}
		}
	}

	/**
	*	Tries to perform allocation of '_count' indexes one-by-one.
	*	Allocated indexes returned via '_array' parameter.
	*	Returns count of successfully allocated indexes.
	*	Accepts ternary variant with SIMPLEINDEXPOOL_TERNARY macro.
	**/
	unsigned int newIndex(TIndex _array[], unsigned int _count) {
		if (!_count)
			return 0;
		#ifdef SIMPLEINDEXPOOL_TERNARY
			for (unsigned int _index = 0; _index < _count; _index++) {
				try {
					_array[_index] = newIndex(_index ? _array[_index - 1] - minIndex + 1 : -1);
				}
				catch (std::out_of_range e) {
					#ifdef DEBUG_INDEXPOOL
						#ifdef WARNINGS_INDEXPOOL
							DEBUG_OUT << "WARNING::SIMPLE_INDEX_POOL::newIndex::Can't allocate more indexes." << DEBUG_NEXT_LINE;
							DEBUG_OUT << "\tMessage: Final allocated count: " << _index << DEBUG_NEXT_LINE;
						#endif
					#endif
					return _index;
				}
			}
		#else
			try {
				_array[0] = newIndex(-1);
			}
			catch (std::out_of_range e) {
				#ifdef DEBUG_INDEXPOOL
					#ifdef WARNINGS_INDEXPOOL
						DEBUG_OUT << "WARNING::SIMPLE_INDEX_POOL::newIndex::Can't allocate more indexes." << DEBUG_NEXT_LINE;
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
							DEBUG_OUT << "WARNING::SIMPLE_INDEX_POOL::newIndex::Can't allocate more indexes." << DEBUG_NEXT_LINE;
							DEBUG_OUT << "\tMessage: Final allocated count: " << _index << DEBUG_NEXT_LINE;
						#endif
					#endif
					return _index;
				}
			}
		#endif
		return _count;
	}

	/**
	*	Performs allocation of all remaining non-allocated indexes.
	*	Returns a helper structure.
	**/
	MaxAllocationHelper allocateMax(unsigned int indexesPerPartiotion = 1000) {
		MaxAllocationHelper result;
		result.length = indexesPerPartiotion;

		unsigned int _freeCount = 0;
		Bucket* _ptr(pool);
		while (_ptr != pool + length - 1) {
			if (~(*_ptr))
				_freeCount++;
			_ptr++;
		}
		if (*_ptr ^ (~((~((Bucket)0)) << tail)))
			_freeCount++;

		result.allocatedCount = (_freeCount * bucketBitSize) / indexesPerPartiotion + 1;
		result.partitions = new TIndex*[result.allocatedCount];

		unsigned int _partitionIndex = 0;
		unsigned int _totalAllocatedCount = 0;
		unsigned int _stepAllocated = 0;

		for (;;) {
			result.partitions[_partitionIndex] = new TIndex[result.length];
			_stepAllocated = newIndex(result.partitions[_partitionIndex], result.length);
			_totalAllocatedCount += _stepAllocated;
			_partitionIndex++;
			if (_stepAllocated != result.length)
				break;
		}

		result.realCount = _partitionIndex;
		result.totalAllocatedCount = _totalAllocatedCount;
		result.tailLength = _stepAllocated;

		return std::move(result);
	}

	/**
	*	Performs allocation of all remaining non-allocated indexes.
	*	Returns a pointer to array of indexes.
	*	Returns count of allocated indexes via '_allocatedCount' variable.
	**/
	TIndex* allocateMaxByArray(unsigned int &_allocatedCount, unsigned int indexesPerPartiotion = 1000) {
		MaxAllocationHelper _buff = allocateMax(indexesPerPartiotion);
		_allocatedCount = _buff.totalAllocatedCount;
		TIndex* _resultPtr = new TIndex[_allocatedCount];
		TIndex* _currentPtr(_resultPtr);
		for (unsigned int _index = 0; _index < _buff.realCount; _index++) {
			_currentPtr = _resultPtr + _index * _buff.length;
			std::memcpy(_currentPtr, _buff.partitions[_index], (_index < _buff.realCount - 1 ? _buff.length : _buff.tailLength) * sizeof(TIndex));
		}
		return _resultPtr;
	}

	/**
	*	Provides the location to start searching for new index based on given index '_index'.
	*	Use it with newIndex() to speed up index search process on linear allocation:
	*		new_index = obj.newIndex(obj.startLocation(last_given_index));
	**/
	inline unsigned int startLocation(TIndex _index) { return _index - minIndex + 1; }

	/**
	*	Deallocate index '_index'.
	**/
	void deleteIndex(TIndex _index) {
		if (_index < minIndex || _index > maxIndex)
			return;
		//Bucket position
		register Bucket* ptr = pool + (_index - minIndex) / bucketBitSize;
		//Biit position
		register unsigned int _tail = (_index - minIndex) % bucketBitSize;
		//Set bit to zero
		*ptr &= ~(((Bucket)1) << _tail);
	}

	/**
	*	Deallocate indexex from '_start' index to '_end' index.
	**/
	void deleteIndex(TIndex _start, TIndex _end) {
		if (_start > _end) 
			_start ^= _end ^= _start ^= _end;
		if (_start < minIndex)
			_start = minIndex;
		if (_end > maxIndex)
			_end = maxIndex;
		//bucketBitSize * 2 was chosen to not handle a case with (_end == 1 + _start)
		if ((unsigned int)(_end - _start) < bucketBitSize * 2) {
			for (TIndex _index = _start; _index <= _end; _index++)
				deleteIndex(_index);
		} else {
			//Get ptr to bucket next to one with _start index
			//BTW: VS2013U doesn't allow you to use variable called "_sptr", so "_bptr" was used instead in which 'b' for begin.
			Bucket* _bptr = pool + (_start - minIndex) / bucketBitSize + 1;
			//Get ptr to bucket before one with _end index
			Bucket* _eptr = pool + (_end - minIndex) / bucketBitSize - 1;
			//Clear all buckets
			std::memset(_bptr, 0, ((_eptr - _bptr) + 1) * sizeof(Bucket));
			//Return to buckets with _start and _end indexes
			_bptr--;
			_eptr++;
			//Set and apply mask for forward clearing
			*_bptr &= ~((~((Bucket)0)) << ((_start - minIndex) % bucketBitSize));
			//Set and apply mask for backward clearing
			*_eptr &= (~((Bucket)0)) << ((_end - minIndex) % bucketBitSize + 1);
		}
	}
};

/**
*	This class implements handling of pre-allocated pool of indexes over SimpleIndexPool functionality.
*	Only integral index types are accepted with constant increment step (operaor++).
*	Heavily depends on CHAR_BIT macro from <limits.h>
*	Class template definition: SmartSimpleIndexPool
**/
template < class TIndex = int >
class SmartSimpleIndexPool : public SimpleIndexPool<TIndex> {
	static_assert(	std::is_integral<TIndex>::value,
					"ASSERTION_ERROR::SMART_SIMPLE_INDEX_POOL::Provided type 'TIndex' must be integral.");
	//Pool of preallocated indexes
	TIndex* preallocatedPool;
	//Pointer to pool top
	TIndex* topPtr;
	//Estimated size of periodic requests
	unsigned int meanRequestSize;
	//Hide public interface of SimpleIndexPool
	SimpleIndexPool::newIndex;
	SimpleIndexPool::startLocation;
	SimpleIndexPool::deleteIndex;
	SimpleIndexPool::allocateMax;
	SimpleIndexPool::allocateMaxByArray;
public:

	SmartSimpleIndexPool(	TIndex _min, TIndex _max, unsigned int _meanRequestSize) : 
							SimpleIndexPool(_max, _min), meanRequestSize(_meanRequestSize)
	{
		preallocatedPool = new TIndex[meanRequestSize * 2];
		SimpleIndexPool::newIndex(preallocatedPool, meanRequestSize * 2);
		topPtr = preallocatedPool + meanRequestSize * 2 - 1;
	}

	SmartSimpleIndexPool(const SmartSimpleIndexPool& other) :	SimpleIndexPool(other),
																meanRequestSize(other.meanRequestSize)
	{
		preallocatedPool = new TIndex[meanRequestSize * 2];
		std::memcpy(preallocatedPool, other.preallocatedPool, meanRequestSize * 2 * sizeof(TIndex));
		topPtr = preallocatedPool + (other.topPtr - other.preallocatedPool);
	}

	SmartSimpleIndexPool(SmartSimpleIndexPool&& other) : SimpleIndexPool(std::move(other)),
														 preallocatedPool(std::move(other.preallocatedPool)),
														 topPtr(std::move(other.topPtr))
	{
		other.preallocatedPool = nullptr;
	}

	~SmartSimpleIndexPool() { delete preallocatedPool; }

	/**
	*	Computes mamory used by this pool.
	**/
	size_t usedMemory() { return SimpleIndexPool::usedMemory() - sizeof(SimpleIndexPool) + sizeof(SmartSimpleIndexPool); }

	/**
	*	Performs an allocation of new index.
	*	Exceptions: std::out_of_range if no more indexes can be allocated.
	**/
	TIndex newIndex() {
		if (topPtr < preallocatedPool + meanRequestSize) {
			/*unsigned int _allocated = 2 * meanRequestSize - (topPtr - preallocatedPool + 1);
			_allocated = SimpleIndexPool::newIndex(++topPtr, _allocated);
			topPtr += _allocated;*/
			//oneliner:
			topPtr += SimpleIndexPool::newIndex(++topPtr, 2 * meanRequestSize - (topPtr - preallocatedPool)) - 1;
		}
		if (topPtr != preallocatedPool - 1) {
			topPtr--;
			return *(topPtr + 1);
		}
		throw std::out_of_range("ERROR::SMART_SIMPLE_INDEX_POOL::newIndex::Can't allocate more indexes.");
		return (TIndex)0;
	}

	/**
	*	Tries to perform allocation of '_count' indexes.
	*	Allocated indexes returned via '_array' parameter.
	*	Returns count of successfully allocated indexes.
	**/
	unsigned int newIndex(TIndex _array[], unsigned int _count) {
		if (!_count)
			return 0;
		if (_count < 2) {
			try { _array[0] = newIndex(); }
			catch (...) { return 0; }
			return 1;
		}
		unsigned int _inStock = topPtr - preallocatedPool + 1;
		if (_count <= _inStock) {
			topPtr = preallocatedPool + (_inStock - _count) - 1;
			std::memcpy(_array, topPtr + 1, _count * sizeof(TIndex));
			return _count;
		} else {
			topPtr += SimpleIndexPool::newIndex(++topPtr, _count - _inStock) - 1;
			_inStock = topPtr - preallocatedPool + 1;
			if (_count <= _inStock) {
				topPtr = preallocatedPool + (_inStock - _count) - 1;
				std::memcpy(_array, topPtr + 1, _count * sizeof(TIndex));
				return _count;
			} else if(_inStock) {
				std::memcpy(_array, preallocatedPool, _inStock * sizeof(TIndex));
				topPtr = preallocatedPool - 1;
				#ifdef DEBUG_INDEXPOOL
					#ifdef WARNINGS_INDEXPOOL
						DEBUG_OUT << "WARNING::SMART_SIMPLE_INDEX_POOL::newIndex::Can't allocate more indexes." << DEBUG_NEXT_LINE;
						DEBUG_OUT << "\tMessage: Final allocated count: " << _inStock << DEBUG_NEXT_LINE;
					#endif
				#endif
				return _inStock;
			}
		}
		#ifdef DEBUG_INDEXPOOL
			#ifdef WARNINGS_INDEXPOOL
				DEBUG_OUT << "WARNING::SMART_SIMPLE_INDEX_POOL::newIndex::Can't allocate more indexes." << DEBUG_NEXT_LINE;
				DEBUG_OUT << "\tMessage: Final allocated count: 0" << DEBUG_NEXT_LINE;
			#endif
		#endif
		return 0;
	}

	/**
	*	Deallocate index '_index'.
	**/
	void deleteIndex(TIndex _index) {
		for (int _index = 0; _index <= topPtr - preallocatedPool; _index++)
			if (*(preallocatedPool + _index) == _index)
				return;
		SimpleIndexPool::deleteIndex(_index);
	}

	/**
	*	Deallocate indexex from '_start' index to '_end' index.
	**/
	void deleteIndex(TIndex _begin, TIndex _end) {
		SimpleIndexPool::deleteIndex(_begin, _end);
		SimpleIndexPool::allocateSpecific(preallocatedPool, (unsigned int)(topPtr - preallocatedPool + 1));
	}
};
#endif
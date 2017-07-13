#ifndef SIMPLEINDEXPOOL_H
#define SIMPLEINDEXPOOL_H "[multi@cSimpleIndexPool.hpp]"
/*
*	DESCRIPTION:
*		Module contains implementation of efficient index pool with simple logic.
*		Logic: heap
*	AUTHOR:
*		Mikhail Demchenko
*		mailto:dev.echo.mike@gmail.com
*		https://github.com/echo-Mike
*/
//STD
#include <cmath>
#include <limits.h>
#include <cstring>
#include <string.h>
#include <stdexcept>
//OUR
#include "general\vs2013tweaks.h"
#include "general\mConcepts.hpp"
//DEBUG
#if defined(DEBUG_SIMPLEINDEXPOOL) && !defined(OTHER_DEBUG)
	#include "general\mDebug.h"		
#elif defined(DEBUG_SIMPLEINDEXPOOL) && defined(OTHER_DEBUG)
	#include OTHER_DEBUG
#endif

template < class TIndex = int >
/**
*	\brief Performs time efficient index handling with simple algorithms.
*	Only integral index types are accepted with constant increment step (operaor++).
*	Heavily depends on CHAR_BIT macro from <limits.h>
*	!!!ATTENTION!!! This class directly operates on memory so all construct, copy and move operations may throw exceptions.
*	Class template definition: SimpleIndexPool
**/
class SimpleIndexPool {
	CONCEPT_INTEGRAL(TIndex, "ASSERTION_ERROR::SIMPLE_INDEX_POOL::Provided type \"TIndex\" must be integral.")
	//Bitset storage of state of index allocation
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
	*	\brief Performs index search based on last linear index search defined by 'currentPosition' pointer.
	*	Accepts ternary variant with SIMPLEINDEXPOOL_TERNARY macro.
	*	\throw nothrow
	*	\return Newly allocated index or 'notFoundIndex'.
	**/
	TIndex newIndexFromCurrentPosition() NOEXCEPT {
		TIndex _result;
		register Bucket* ptr(currentPosition);
		register Bucket* _end(pool + length);
		while (ptr != _end) {
			#ifdef SIMPLEINDEXPOOL_TERNARY
				if (~(*ptr)) {
					for (	register unsigned int _index = 0; _index < ( tail && ptr == _end - 1 ? tail : bucketBitSize); _index++) {
						if (!(*ptr & (((Bucket)1) << _index))) {
							*ptr |= ((Bucket)1) << _index;
							currentPosition = ptr;
							_result = (TIndex)(((ptr - pool) * bucketBitSize + _index) + minIndex);
							ptr = nullptr;
							_end = nullptr;
							return _result;
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
							_result = (TIndex)(((ptr - pool) * bucketBitSize + _index) + minIndex);
							ptr = nullptr;
							_end = nullptr;
							return _result;
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
							_result = (TIndex)(((ptr - pool) * bucketBitSize + _index) + minIndex);
							ptr = nullptr;
							_end = nullptr;
							return _result;
						}
					}
				}
			#endif
			ptr++;
		}
		ptr = nullptr;
		_end = nullptr;
		//throw std::out_of_range("ERROR::SIMPLE_INDEX_POOL::newIndexFromCurrentPosition::New index not found.");
		return notFoundIndex;
	}

	/**
	*	\brief Preforms index search from start of index interval.
	*	Accepts ternary variant with SIMPLEINDEXPOOL_TERNARY macro.
	*	\throw nothrow
	*	\return Newly allocated index or 'notFoundIndex'.
	**/
	TIndex newIndexFromStart() NOEXCEPT {
		//Reset position of linear search to zero
		currentPosition = pool;
		//Preform a linear search
		/*try { return newIndexFromCurrentPosition(); }
		catch (...) { 
			throw std::out_of_range("ERROR::SIMPLE_INDEX_POOL::newIndexFromStart::New index not found.");
			return (TIndex)0;
		}*/
		return newIndexFromCurrentPosition();
	}

	/**
	*	\brief Preforms index search from provided starting position 'startFrom'.
	*	This function contain a lot of reused code segments because all solutions with 
	*	ternary operator ?:; generates slower machine code and will perform a lot of unnecessary checks.
	*	Accepts ternary variant with SIMPLEINDEXPOOL_TERNARY macro.
	*	\param[in]	startFrom	Index to start search.
	*	\throw nothrow
	*	\return Newly allocated index or 'notFoundIndex'.
	**/
	TIndex newIndexFromProvidedPosition(unsigned int startFrom) NOEXCEPT {
		TIndex _result;
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
							_result = (TIndex)(((ptr - pool) * bucketBitSize + _index) + minIndex);
							ptr = nullptr;
							return _result;
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
						_result = (TIndex)(((ptr - pool) * bucketBitSize + _index) + minIndex);
						ptr = nullptr;
						return _result;
					}
				}
			//If current bucket is not full then pass thru it
			} else if (~(*ptr)) {
				for (register unsigned int _index = startFrom % bucketBitSize; _index < bucketBitSize; _index++) {
					if (!(*ptr & (((Bucket)1) << _index))) {
						*ptr |= ((Bucket)1) << _index;
						_result = (TIndex)(((ptr - pool) * bucketBitSize + _index) + minIndex);
						ptr = nullptr;
						return _result;
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
							_result = (TIndex)(((ptr - pool) * bucketBitSize + _index) + minIndex);
							ptr = nullptr;
							return _result;
						}
					}
				} else if (~(*ptr)) {
					for (register unsigned int _index = 0; _index < bucketBitSize; _index++) {
						if (!(*ptr & (((Bucket)1) << _index))) {
							*ptr |= ((Bucket)1) << _index;
							_result = (TIndex)(((ptr - pool) * bucketBitSize + _index) + minIndex);
							ptr = nullptr;
							return _result;
						}
					}
				}

				ptr++;
			}
		#endif
		ptr = nullptr;
		//throw std::out_of_range("ERROR::SIMPLE_INDEX_POOL::newIndexFromProvidedPosition::New index not found.");
		//return (TIndex)0;
		return notFoundIndex;
	}
protected:
	//Special index that indicates that no indexes was found.
	TIndex notFoundIndex;

	/**
	*	\brief Internal function for performing trustful index data change.
	*	\param[in] _array	Array of indexes to be trastfuly allocated.
	*	\param[in] _length	Length of array.
	*	\throw nothrow
	*	\return noreturn
	**/
	void allocateSpecific(TIndex _array[], unsigned int _length) NOEXCEPT {
		if (!pool)
			return;
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
		ptr = nullptr;
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
		
		MaxAllocationHelper() : partitions(nullptr), realCount(0), length(0), totalAllocatedCount(0), tailLength(0), allocatedCount(0) {};

		MaxAllocationHelper(const MaxAllocationHelper&) = delete;

		MaxAllocationHelper& operator=(const MaxAllocationHelper&) = delete;

		MaxAllocationHelper(MaxAllocationHelper&& other) : 
							partitions(other.partitions),	allocatedCount(other.allocatedCount),
							realCount(sother.realCount),	length(other.length),
							tailLength(other.tailLength),	totalAllocatedCount(other.totalAllocatedCount)
		{
			other.partitions = nullptr;
		}

		MaxAllocationHelper& operator=(MaxAllocationHelper&& other)
		{
			if (partitions)
				for (unsigned int _index = 0; _index < realCount; _index++)
					delete[] partitions[_index];
			delete[] partitions;
			partitions = other.partitions;
			allocatedCount = other.allocatedCount;
			realCount = other.realCount;
			length = other.length;
			tailLength = other.tailLength;
			totalAllocatedCount = other.totalAllocatedCount;
			other.partitions = nullptr;
		}

		//Proper memory cleaning
		~MaxAllocationHelper() NOEXCEPT {
			if (partitions)
				for (unsigned int _index = 0; _index < realCount; _index++)
					delete[] partitions[_index];
			delete[] partitions;
		}
	};

	SimpleIndexPool() = delete;

	SimpleIndexPool(TIndex _min, TIndex _max) : minIndex(_min), maxIndex(_max), pool(nullptr)
	{
		if (_min > _max) {
			maxIndex = _min;
			minIndex = _max;
		}

		if (maxIndex == minIndex)
			throw std::invalid_argument("ERROR::SIMPLE_INDEX_POOL::Constructor::Invalid pool size.");

		notFoundIndex = maxIndex + 1;
		if (notFoundIndex == minIndex)
			notFoundIndex == maxIndex;

		size = maxIndex - minIndex + 1;
		bucketBitSize = sizeof(Bucket) * CHAR_BIT;

		length = size / bucketBitSize;
		tail = size % bucketBitSize;
		if (tail)
			length++;

		//ERROR::SIMPLE_INDEX_POOL::Constructor::System can't allocate memory for pool.
		//std::bad_alloc may be thrown here
		pool = new Bucket[length];

		std::memset(pool, 0, length * sizeof(Bucket));

		currentPosition = pool;
	}

	~SimpleIndexPool() NOEXCEPT {
		currentPosition = nullptr;
		delete[] pool; 
	}

	SimpleIndexPool(const SimpleIndexPool& other) : minIndex(other.minIndex),
													maxIndex(other.maxIndex),
													notFoundIndex(other.notFoundIndex),
													size(other.size),
													tail(other.tail),
													length(other.length)
	{
		//ERROR::SIMPLE_INDEX_POOL::CopyConstructor::System can't allocate memory for pool.
		//std::bad_alloc may be thrown here
		pool = new Bucket[length];
		std::memcpy(pool, other.pool, length * sizeof(Bucket));
		currentPosition = pool + (other.currentPosition - other.pool);
	}

	SimpleIndexPool& operator= (const SimpleIndexPool& other) {
		if (&other == this)
			return *this;
		delete[] pool;
		minIndex = other.minIndex;
		maxIndex = other.maxIndex;
		notFoundIndex = other.notFoundIndex;
		size = other.size;
		tail = other.tail;
		length = other.length;
		//ERROR::SIMPLE_INDEX_POOL::CopyAssignment::System can't allocate memory for pool.
		//std::bad_alloc may be thrown here
		pool = new Bucket[length];
		std::memcpy(pool, other.pool, length * sizeof(Bucket));
		currentPosition = pool + (other.currentPosition - other.pool);
	}

	SimpleIndexPool(SimpleIndexPool&& other) NOEXCEPT_IF(CONCEPT_NOEXCEPT_MOVE_CONSTRUCTIBLE_V(TIndex)) :
					minIndex(std::move(other.minIndex)),
					maxIndex(std::move(other.maxIndex)),
					notFoundIndex(std::move(other.notFoundIndex)),
					size(other.size),
					currentPosition(other.currentPosition),
					tail(other.tail),
					length(other.length)
	{
		//ERROR::SIMPLE_INDEX_POOL::MoveConstructor::Empty pool of rvalue.
		pool = other.pool;
		other.pool = nullptr;
		other.currentPosition = nullptr;
	}

	SimpleIndexPool& operator= (SimpleIndexPool&& other) NOEXCEPT_IF(CONCEPT_NOEXCEPT_MOVE_CONSTRUCTIBLE_V(TIndex)) {
		delete[] pool;
		minIndex = std::move(other.minIndex);
		maxIndex = std::move(other.maxIndex);
		notFoundIndex = std::move(other.notFoundIndex);
		size = other.size;
		tail = other.tail;
		length = other.length;
		//ERROR::SIMPLE_INDEX_POOL::MoveAssignment::Empty pool of rvalue.
		pool = other.pool;
		currentPosition = other.currentPosition;
		if (!currentPosition)
			currentPosition = pool;
		other.pool = nullptr;
		other.currentPosition = nullptr;
	}

	/**
	*	Compares given '_index' with notFound index.
	**/
	inline bool isNotFound(const TIndex& _index) { return _index == notFoundIndex; }

	/**
	*	Provides read access to 'size' value.
	**/
	unsigned int getSize() NOEXCEPT { return size; }

	/**
	*	Provides read access to 'minIndex' value.
	**/
	TIndex getMinIndex() NOEXCEPT { return minIndex; }

	/**
	*	Provides read access to 'maxIndex' value.
	**/
	TIndex getMaxIndex() NOEXCEPT { return maxIndex; }

	/**
	*	Computes memory used by this pool.
	**/
	size_t usedMemory() NOEXCEPT { return length * sizeof(Bucket) + sizeof(SimpleIndexPool); }

	/**
	*	\brief Performs a search of new index.
	*	Value of 'startFrom' defines behaviour of search:
	*	-- startFrom < 0 -- : Performs linear search based on last linear search request.
	*	-- startFrom == 0-- : Performs linear search from start of index interval.
	*	-- startFrom > 0 -- : Performs search from provided index place startFrom :
	*	---- Index places are marked from 0 to obj.size-1.
	*	---- Use 'obj.startLocation(last_given_index)' to obtain a new location for linear index search.
	*	\param[in]	startFrom	Behaviour qualificator.
	*	\throw nothrow
	*	\return Newly allocated index or 'notFoundIndex'.
	**/
	TIndex newIndex(int startFrom) NOEXCEPT {
		if (!pool)
			return notFoundIndex;
		if (startFrom >= (int)size)
			startFrom = size - 1;
		/*
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
		}*/
		if (!startFrom)
			return newIndexFromStart();
		return startFrom < 0 ? newIndexFromCurrentPosition() : newIndexFromProvidedPosition(startFrom);
	}

	/**
	*	\brief Tries to perform allocation of '_count' indexes one-by-one.
	*	Accepts ternary variant with SIMPLEINDEXPOOL_TERNARY macro.
	*	\param[out]	_array	Array filled with new indexes.
	*	\param[in]	_count	Count of indexes to be allocated.
	*	\throw nothrow
	*	\return Count of successfully allocated indexes.
	**/
	unsigned int newIndex(TIndex _array[], unsigned int _count) NOEXCEPT {
		if (!(_count && pool))
			return 0;
		#ifdef SIMPLEINDEXPOOL_TERNARY
			for (unsigned int _index = 0; _index < _count; _index++) {
				_array[_index] = newIndex(_index ? _array[_index - 1] - minIndex + 1 : -1);
				//No debug logger: it may produce exceptions.
				if (_array[_index] == notFoundIndex) 
					return _index;
			}
		#else
			//No debug logger: it may produce exceptions.
			_array[0] = newIndex(-1);
			if (_array[0] == notFoundIndex) 
				return 0;
			for (unsigned int _index = 1; _index < _count; _index++) {
				_array[_index] = newIndex(_array[_index - 1] - minIndex + 1);
				if (_array[_index] == notFoundIndex) 
					return _index;
			}
		#endif
		return _count;
	}

	/**
	*	\brief Performs allocation of all remaining non-allocated indexes.
	*	\param[in]	indexesPerPartiotion	Length of internal index buckets.
	*	\throw std::bad_alloc If system don't have enougth memory for this allocation.
	*	\return Helper structure.
	**/
	MaxAllocationHelper allocateMax(unsigned int indexesPerPartiotion = 1000) {
		MaxAllocationHelper result;
		if (!pool || indexesPerPartiotion == 0)
			return result;
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
		//ERROR::SIMPLE_INDEX_POOL::allocateMax::System don't have enougth memory to allocate halper array.
		//std::bad_alloc may be thrown here
		result.partitions = new TIndex*[result.allocatedCount];

		result.realCount = 0;
		result.totalAllocatedCount = 0;
		for (;;) {
			//ERROR::SIMPLE_INDEX_POOL::allocateMax::System don't have enougth memory to allocate halper array.
			//std::bad_alloc may be thrown here
			result.partitions[result.realCount] = new TIndex[result.length];
			//Now there is no need to manualy free memory.
			//In case of exception 'result' as a local object will be automatically deleted.
			result.tailLength = newIndex(result.partitions[result.realCount], result.length);
			result.totalAllocatedCount += result.tailLength;
			(result.realCount)++;
			if (result.tailLength != result.length)
				break;
		}

		_ptr = nullptr;
		return result;
	}

	/**
	*	\brief Performs allocation of all remaining non-allocated indexes.
	*	Returns count of allocated indexes via '_allocatedCount' variable.
	*	\param[out]	_allocatedCount			Count of allocated indexes.
	*	\param[in]	indexesPerPartiotion	Length of internal index buckets.
	*	\throw std::bad_alloc If system don't have enougth memory for this allocation.
	*	\return Pointer to array of indexes, may be nullptr if pool is not allocated.
	**/
	TIndex* allocateMaxByArray(unsigned int& _allocatedCount, unsigned int indexesPerPartiotion = 1000) {
		if (!pool) {
			_allocatedCount = 0;
			return nullptr;
		}
		MaxAllocationHelper _buff;
		//std::bad_alloc may be thrown here
		_buff = allocateMax(indexesPerPartiotion);
		_allocatedCount = _buff.totalAllocatedCount;
		//ERROR::SIMPLE_INDEX_POOL::allocateMaxByArray::System don't have enougth memory to unpack helper structure.
		//std::bad_alloc may be thrown here
		TIndex* _resultPtr = new TIndex[_allocatedCount];
		TIndex* _currentPtr(_resultPtr);
		for (unsigned int _index = 0; _index < _buff.realCount - 1; _index++) {
			//Copy normal length buffers
			_currentPtr = _resultPtr + _index * _buff.length;
			std::memcpy(_currentPtr, _buff.partitions[_index],  _buff.length * sizeof(TIndex));
		}
		//Copy tail buffer
		_currentPtr = _resultPtr + (_buff.realCount - 1) * _buff.length;
		std::memcpy(_currentPtr, _buff.partitions[_index], _buff.tailLength * sizeof(TIndex));
		_currentPtr = nullptr;
		return _resultPtr;
	}

	/**
	*	\brief Provides the location to start searching for new index based on given index '_index'.
	*	Use it with newIndex() to speed up index search process on linear allocation:
	*		new_index = obj.newIndex(obj.startLocation(last_given_index));
	*	\param[in]	_index	Index to be computed in relative position.
	*	\throw nothrow
	*	\return Relative index position.
	**/
	inline unsigned int startLocation(TIndex _index) NOEXCEPT {
		if (_index > maxIndex)
			_index = maxIndex;
		if (_index < minIndex)
			_index = minIndex;
		return _index - minIndex + 1; 
	}

	/**
	*	\brief Deallocate index '_index'.
	*	\param[in]	_index	Index to be deallocated.
	*	\throw nothrow
	*	\return noreturn
	**/
	void deleteIndex(TIndex _index) NOEXCEPT {
		if (_index < minIndex || _index > maxIndex || !pool)
			return;
		//Bucket position
		register Bucket* ptr = pool + (_index - minIndex) / bucketBitSize;
		//Biit position
		register unsigned int _tail = (_index - minIndex) % bucketBitSize;
		//Set bit to zero
		*ptr &= ~(((Bucket)1) << _tail);
		ptr = nullptr;
	}

	/**
	*	\brief Deallocate indexex from '_start' index to '_end' index.
	*	\param[in]	_start	Start of indexes to be deallocated.
	*	\param[in]	_end	End of indexes to be deallocated.
	*	\throw nothrow
	*	\return noreturn
	**/
	void deleteIndex(TIndex _start, TIndex _end) NOEXCEPT {
		if (!pool) 
			return;
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
			_bptr = nullptr;
			_eptr = nullptr;
		}
	}

	/**
	*	\brief Check index allocation status of '_index'.
	*	\param[in]	_index	Index to be checked.
	*	\throw nothrow
	*	\return Index allocation status.
	**/
	bool isUsed(TIndex _index) NOEXCEPT {
		if (_index < minIndex || _index > maxIndex || !pool)
			return false;
		bool _result;
		//Bucket position
		register Bucket* ptr = pool + (_index - minIndex) / bucketBitSize;
		//Biit position
		register unsigned int _tail = (_index - minIndex) % bucketBitSize;
		//Set bit to zero
		_result = *ptr & (((Bucket)1) << _tail);
		ptr = nullptr;
		return _result;
	}
};
#endif
#ifndef SMARTSIMPLEINDEXPOOL_H
#define SMARTSIMPLEINDEXPOOL_H "[multi@cSmartSimpleIndexPool.hpp]"
/*
*	DESCRIPTION:
*		Module contains implementation of efficient index pool with simple logic and 
*		buld-in preallocated pool for indexes.
*		Logic: double heap
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
#include "cSimpleIndexPool.hpp"
//DEBUG
#if defined(DEBUG_SMARTSIMPLEINDEXPOOL) && !defined(OTHER_DEBUG)
	#include "general\mDebug.h"		
#elif defined(DEBUG_SMARTSIMPLEINDEXPOOL) && defined(OTHER_DEBUG)
	#include OTHER_DEBUG
#endif

template < class TIndex = int >
/**
*	\brief This class implements handling of pre-allocated pool of indexes over SimpleIndexPool functionality.
*	Only integral index types are accepted with constant increment step (operaor++).
*	Heavily depends on CHAR_BIT macro from <limits.h>
*	!!!ATTENTION!!! This class directly operates on memory so all construct, copy and move operations may throw exceptions.
*	Class template definition: SmartSimpleIndexPool
**/
class SmartSimpleIndexPool : public SimpleIndexPool<TIndex> {
	CONCEPT_INTEGRAL(TIndex, "ASSERTION_ERROR::SMART_SIMPLE_INDEX_POOL::Provided type \"TIndex\" must be integral.")
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

	SmartSimpleIndexPool() = delete;

	SmartSimpleIndexPool(	TIndex _min, TIndex _max, unsigned int _meanRequestSize) : 
							SimpleIndexPool(_max, _min), meanRequestSize(_meanRequestSize)
	{
		preallocatedPool = new TIndex[meanRequestSize * 2];
		if (!preallocatedPool)
			std::exception("ERROR::SMART_SIMPLE_INDEX_POOL::Constructor::System can't allocate memory for preallocated pool.");
		auto _allocated = SimpleIndexPool::newIndex(preallocatedPool, meanRequestSize * 2);
		topPtr = preallocatedPool + _allocated - 1;
	}

	~SmartSimpleIndexPool() NOEXCEPT { 
		topPtr = nullptr;
		delete[] preallocatedPool; 
	}

	SmartSimpleIndexPool(const SmartSimpleIndexPool& other) :	SimpleIndexPool(other),
																meanRequestSize(other.meanRequestSize)
	{
		preallocatedPool = new TIndex[meanRequestSize * 2];
		if (!preallocatedPool)
			std::exception("ERROR::SMART_SIMPLE_INDEX_POOL::CopyConstructor::System can't allocate memory for preallocated pool.");
		std::memcpy(preallocatedPool, other.preallocatedPool, meanRequestSize * 2 * sizeof(TIndex));
		topPtr = preallocatedPool + (other.topPtr - other.preallocatedPool);
	}

	SmartSimpleIndexPool& operator= (const SmartSimpleIndexPool& other) {
		if (&other == this)
			return *this;
		SimpleIndexPool::operator=(other);
		delete[] preallocatedPool;
		meanRequestSize = other.meanRequestSize;
		preallocatedPool = new TIndex[meanRequestSize * 2];
		if (!preallocatedPool)
			std::exception("ERROR::SMART_SIMPLE_INDEX_POOL::CopyAssignment::System can't allocate memory for preallocated pool.");
		std::memcpy(preallocatedPool, other.preallocatedPool, meanRequestSize * 2 * sizeof(TIndex));
		topPtr = preallocatedPool + (other.topPtr - other.preallocatedPool);
	}

	SmartSimpleIndexPool(SmartSimpleIndexPool&& other) :SimpleIndexPool(std::move(other)),
														topPtr(std::move(other.topPtr))
	{
		preallocatedPool = other.preallocatedPool;
		if (!preallocatedPool) {
			//This is a controversial line, we don't realy now how to free resources of base index pool.
			//other.~SmartSimpleIndexPool();
			std::exception("ERROR::SMART_SIMPLE_INDEX_POOL::MoveConstructor::Empty preallocatedPool of rvalue.");
		}
		other.preallocatedPool = nullptr;
		other.topPtr = nullptr;
	}

	SmartSimpleIndexPool& operator= (SmartSimpleIndexPool&& other) {
		delete[] preallocatedPool;
		meanRequestSize = other.meanRequestSize;
		preallocatedPool = other.preallocatedPool;
		if (!preallocatedPool) {
			other.~SmartSimpleIndexPool();
			std::exception("ERROR::SMART_SIMPLE_INDEX_POOL::MoveAssignment::Empty preallocatedPool of rvalue.");
		}
		//This is not safe:
		topPtr = other.topPtr;
		try { SimpleIndexPool::operator=(std::move(other)); }
		catch (...) { 
			delete[] preallocatedPool;
			preallocatedPool = nullptr;
			topPtr = nullptr;
			throw; 
		}
		other.preallocatedPool = nullptr;
		other.topPtr = nullptr;
	}

	/**
	*	Computes memory used by this pool.
	**/
	size_t usedMemory() { return SimpleIndexPool::usedMemory() - sizeof(SimpleIndexPool) + sizeof(SmartSimpleIndexPool); }

	/**
	*	\brief Performs an allocation of new index.
	*	\return Newly allocated index.
	*	\throw std::out_of_range If no more indexes can be allocated.
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
	*	\brief Tries to perform allocation of '_count' indexes.
	*	Allocated indexes returned via '_array' parameter.
	*	\param[out]	_array	Array filled with new indexes.
	*	\param[in]	_count	Count of indexes to be allocated.
	*	\throw nothrow
	*	\return Count of successfully allocated indexes.
	**/
	unsigned int newIndex(TIndex _array[], unsigned int _count) NOEXCEPT {
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
				#if defined(DEBUG_SMARTSIMPLEINDEXPOOL) && defined(WARNINGS_SMARTSIMPLEINDEXPOOL)
					DEBUG_NEW_MESSAGE("WARNING::SMART_SIMPLE_INDEX_POOL::newIndex")
						DEBUG_WRITE1("\tMessage: Can't allocate more indexes.");
						DEBUG_WRITE2("\tFinal allocated count: ", _index);
					DEBUG_END_MESSAGE
				#endif
				return _inStock;
			}
		}
		#if defined(DEBUG_SMARTSIMPLEINDEXPOOL) && defined(WARNINGS_SMARTSIMPLEINDEXPOOL)
			DEBUG_NEW_MESSAGE("WARNING::SMART_SIMPLE_INDEX_POOL::newIndex")
				DEBUG_WRITE1("\tMessage: Can't allocate more indexes.");
				DEBUG_WRITE1("\tFinal allocated count: 0");
			DEBUG_END_MESSAGE
		#endif
		return 0;
	}

	/**
	*	\brief Deallocate index '_index'.
	*	\param[in]	_index	Index to be deallocated.
	*	\throw nothrow
	*	\return noreturn
	**/
	void deleteIndex(TIndex _index) NOEXCEPT {
		for (int _index = 0; _index <= topPtr - preallocatedPool; _index++)
			if (*(preallocatedPool + _index) == _index)
				return;
		SimpleIndexPool::deleteIndex(_index);
	}

	/**
	*	\brief Deallocate indexex from '_begin' index to '_end' index.
	*	\param[in]	_begin	Begin of indexes to be deallocated.
	*	\param[in]	_end	End of indexes to be deallocated.
	*	\throw nothrow
	*	\return noreturn
	**/
	void deleteIndex(TIndex _begin, TIndex _end) NOEXCEPT {
		SimpleIndexPool::deleteIndex(_begin, _end);
		SimpleIndexPool::allocateSpecific(preallocatedPool, (unsigned int)(topPtr - preallocatedPool + 1));
	}

	/**
	*	\brief Check index allocation status of '_index'.
	*	\param[in]	_index	Index to be checked.
	*	\throw nothrow
	*	\return Index allocation status.
	**/
	bool isUsed(TIndex _index) NOEXCEPT {
		if (_index < minIndex || _index > maxIndex)
			return false;
		if (SimpleIndexPool::isUsed(_index)) {
			for (int _index0 = 0; _index0 <= topPtr - preallocatedPool; _index0++)
				if (*(preallocatedPool + _index0) == _index0)
					return false;
			return true;
		} else {
			return false;
		}
	}
};
#endif
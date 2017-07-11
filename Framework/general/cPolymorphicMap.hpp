#ifndef POLYMORPHICMAP_H
#define POLYMORPHICMAP_H "[multy@cPolymorphicMap.hpp]"
/*
*	DESCRIPTION:
*		Module contains implementation of polymorphic map class.
*	AUTHOR:
*		Mikhail Demchenko
*		mailto:dev.echo.mike@gmail.com
*		https://github.com/echo-Mike
*/
//STD
#include <map>
//OUR
#include "vPolymorphicContainerGeneral.hpp"
//DEBUG
#if defined(DEBUG_POLYMORPHICMAP) && !defined(OTHER_DEBUG)
	#include "general\mDebug.h"
#elif  defined(DEBUG_POLYMORPHICMAP) && defined(OTHER_DEBUG)
	#include OTHER_DEBUG
#endif

template <	class _Index, class _Base, 
			allocateStrategy (_Base::* _getAllocStrategy)() = &_Base::getAllocStrategy>
/**
*	Class that represents polymorthic container of map type.
*	Class template definition: PolymorphicMap
**/
class PolymorphicMap {
public:
	//Type used as index.
	typedef _Index Index;
	//Type used as base type of handled objects.
	typedef _Base Base;
	//Type of handled pointers to objects.
	typedef std::shared_ptr<_Base> Member;
	//Type of internal storage.
	typedef std::map<_Index, Member> Storage;
protected:
	/**
	*	Storage of pointers to handled objects.
	*	Accessible from derived classes.
	**/
	Storage storage;
public:

	PolymorphicMap() = default;

	~PolymorphicMap() = default;

	PolymorphicMap(const PolymorphicMap&) = delete;

	PolymorphicMap& operator=(const PolymorphicMap&) = delete;

#ifdef MOVE_GENERATION
	PolymorphicMap(PolymorphicMap&&) = default;

	PolymorphicMap& operator=(PolymorphicMap&&) = default;
#else
	PolymorphicMap(PolymorphicMap&& other) NOEXCEPT : storage(std::move(other.storage)) {}

	PolymorphicMap& operator= (PolymorphicMap&& other) NOEXCEPT {
		storage = std::move(other.storage);
		return *this;
	}
#endif // MOVE_GENERATION

	//Public interface start
	template < class T >
	/**
	*	\brief Constructs new object with index '_Id' of type "T" by calling default constructor.
	*	Erase object with index '_Id' if it presented in current time before creating new one.
	*	\param[in]	_Id			Identificator of new object.
	*	\param[in]	_strategy	Defines strategy of new object allocation.
	*	\param[in]	_defptr		Parameter to make template overload possible.
	*	\throw std::logic_error On exception in object default constructor or operator new.
	*	\throw std::bad_alloc On not enougth memory in make_shared operation.
	*	\return Shared pointer of type "T" to new object on success and to nullptr on error.
	**/
	virtual inline auto newObject(	const Index _Id, const allocateStrategy _strategy = allocateStrategy::DEFAULT,
									T* const _defptr = nullptr) -> decltype(std::shared_ptr<CONCEPT_CLEAR_TYPE_T(T)>())
	{
		CONCEPT_CLEAR_TYPE(T, _ObjType)
		CONCEPT_DERIVED(_ObjType, Base, "ASSERTION_ERROR::POLYMORPHIC_MAP::newObject::Provided type \"T\" must be derived from \"Base\".")
		CONCEPT_DEFCONSTR(_ObjType, "ASSERTION_ERROR::POLYMORPHIC_MAP::newObject::Provided type \"T\" must be default constuctible.")
		std::shared_ptr<_ObjType> _newptr(nullptr);
		auto _iterator = storage.find(_Id);
		if (_iterator != storage.end())
			storage.erase(_iterator);
		//Try to default construct new object
		try {
			if (_strategy == allocateStrategy::BIG) {
				/**
				*	Entry: https://stackoverflow.com/questions/620137/do-the-parentheses-after-the-type-name-make-a-difference-with-new
				*	Comment by: https://stackoverflow.com/users/12711/michael-burr
				*	"new _ObjType()" fits better than "new _ObjType"
				**/
				_newptr.reset(new _ObjType());
			} else {
				_newptr = std::move(std::make_shared<_ObjType>());
			}
		}
		//std::make_shared exception : not enougth memory
		catch (const std::bad_alloc&) { throw; }
		//Warp up external exception to std::logic_error
		catch (const std::exception& e) { throw std::logic_error(e.what()); }
		//Provide any other throw with std::logic_error
		catch (...) { throw std::logic_error("ERROR::POLYMORPHIC_MAP::newObject::Object creation error."); }
		//Insert new object to storage
		storage[_Id] = _newptr;
		return _newptr;
	}

	template < class T >
	/**
	*	\brief Performs an attempt to create '_count' objects by calling newObject with default constructor.
	*	Increment result on every successful (nothrow) creation.
	*	\param[in]	_Id			Array of identificators.
	*	\param[out]	_result		Array of shared pointers to new objects.
	*	\param[in]	_count		Count of objects to be created.
	*	\param[in]	_strategy	Defines strategy of new object allocation.
	*	\param[out]	_success	[Optional] Per object status of successful creation.
	*	\throw Ignore : exception may occur if _Id or _result not long enougth.
	*	\return Count of successfully allocated objects.
	**/
	virtual inline unsigned int newObject(	const Index _Id[], std::shared_ptr<T> _result[], const unsigned int _count,
											const allocateStrategy _strategy = allocateStrategy::DEFAULT, bool _success[] = nullptr) 
	{
		CONCEPT_NOT_CVRP(T, "ASSERTION_ERROR::POLYMORPHIC_MAP::newObject::Provided type \"T\" must not be constant/volatile pointer or reference.")
		if (!_count || !_result)
			return 0;
		//Counter of allocated objects
		unsigned int _allocated = 0; 
		for (unsigned int _index = 0; _index < _count; _index++) {
			try { _result[_index] = std::move(newObject<T>(_Id[_index], _strategy)); }
			catch (...) { 
				if (_success)
					_success[_index] = false;
				continue;
			}
			if (_success)
				_success[_index] = true;
			_allocated++;
		}
		return _allocated;
	}

	template < class T >
	/**
	*	\brief Constructs new object of type "T" by move-constructing from '_value' with index '_Id'.
	*	Erase object with index '_Id' if it presented in current time before creating new one.
	*	\param[in]	_value		Move reference to object.
	*	\param[in]	_Id			Identificator of new object.
	*	\param[in]	_strategy	Defines strategy of new object allocation.
	*	\throw std::logic_error On exception in object move-constructor or operator new.
	*	\throw std::bad_alloc On not enougth memory in make_shared operation.
	*	\return Shared pointer of type "T" to new object on success and to nullptr on error.
	**/
	virtual inline auto newObject(	T&& _value, const Index _Id, const allocateStrategy _strategy = allocateStrategy::DEFAULT)
									-> decltype(std::shared_ptr<CONCEPT_CLEAR_TYPE_T(T)>())
	{
		CONCEPT_CLEAR_TYPE(T, _ObjType)
		CONCEPT_UNREF(T, _value, "ASSERTION_ERROR::POLYMORPHIC_MAP::newObject::Provided '_value' is not rvalue or lvalue reference.")
		CONCEPT_DERIVED(_ObjType, Base, "ASSERTION_ERROR::POLYMORPHIC_MAP::newObject::Provided type \"T\" must be derived from \"Base\".")
		CONCEPT_CONSTRUCTIBLE_F(_ObjType, T, _value, "ASSERTION_ERROR::POLYMORPHIC_MAP::newObject::Provided type \"T\" must be constructible from '_value'.")
		std::shared_ptr<_ObjType> _newptr(nullptr);
		//Find object with id '_Id'
		auto _iterator = storage.find(_Id);
		if (_iterator != storage.end()) 
			storage.erase(_iterator);
		//Try to move-construct new object
		try {
			if (_strategy == allocateStrategy::BIG) {
				_newptr.reset(new _ObjType(std::forward<T>(_value)));
			} else {
				_newptr = std::move(std::make_shared<_ObjType>(std::forward<T>(_value)));
			}
		}
		//std::make_shared exception : not enougth memory
		catch (const std::bad_alloc&) { throw; }
		//Warp up external exception to std::logic_error
		catch (const std::exception& e) { throw std::logic_error(e.what()); }
		//Provide any other throw with std::logic_error
		catch (...) { throw std::logic_error("ERROR::POLYMORPHIC_MAP::newObject::Object creation error."); }
		//Insert new object to storage
		storage[_Id] = _newptr;
		return _newptr;
	}

	template < class T >
	/**
	*	\brief Takes ownership of object with type "T" located by pointer '_valueptr' as resource with index '_Id'.
	*	Erase object with index '_Id' if it presented in current time. Accepts nullptr objects.
	*	\param[in]	_valueptr	Pointer to resource.
	*	\param[in]	_Id			Identificator of new object.
	*	\throw nothrow
	*	\return Shared pointer of type "T" to new object on success and to nullptr on error.
	**/
	virtual inline auto newObject(T* const _valueptr, const Index _Id) NOEXCEPT -> decltype(std::shared_ptr<CONCEPT_CLEAR_TYPE_T(T)>()){
		CONCEPT_NOT_CVPR(T, "ASSERTION_ERROR::POLYMORPHIC_MAP::newObject::Provided type \"T\" must not be constant/volatile pointer or reference.")
		CONCEPT_CLEAR_TYPE(T,_ObjType)
		CONCEPT_DERIVED(_ObjType, Base, "ASSERTION_ERROR::POLYMORPHIC_MAP::newObject::Provided type \"T\" must be derived from \"Base\".")
		std::shared_ptr<_ObjType> _newptr(nullptr);
		//Find object with id '_Id'
		auto _iterator = storage.find(_Id);
		if (_iterator != storage.end())
			storage.erase(_iterator);
		_newptr.reset((_ObjType*)_valueptr);
		storage[_Id] = _newptr;
		return _newptr;
	}

	template < class T >
	/**
	*	\brief Performs an attempt to create '_count' objects by calling newObject on copy of '_value'.
	*	Increment result on every successful (nothrow) creation.
	*	\param[in]	_value		Constant reference to be used in copy construction of new objects.
	*	\param[in]	_Id			Array of identificators.
	*	\param[out]	_result		Array of shared pointers to new objects.
	*	\param[in]	_count		Count of objects to be created.
	*	\param[in]	_strategy	Defines strategy of new object allocation.
	*	\param[out]	_success	[Optional] Per object status of successful creation.
	*	\throw nothrow
	*	\return Count of successfully allocated objects.
	**/
	virtual inline unsigned int newObject(	const T& _value, const Index _Id[], std::shared_ptr<T> _result[], const unsigned int _count,
											const allocateStrategy _strategy = allocateStrategy::DEFAULT, bool _success[] = nullptr)
	{
		CONCEPT_NOT_CVPR(T, "ASSERTION_ERROR::POLYMORPHIC_MAP::newObject::Provided type \"T\" must not be constant/volatile pointer or reference.")
		CONCEPT_COPY_CONSTRUCTIBLE(T, "ASSERTION_ERROR::POLYMORPHIC_MAP::newObject::Provided type \"T\" must be copy constructible.")
		CONCEPT_DERIVED(_ObjType, Base, "ASSERTION_ERROR::POLYMORPHIC_MAP::newObject::Provided type \"T\" must be derived from \"Base\".")
		if (!_count)
			return _count;
		//Counter of allocated objects
		unsigned int _allocated = 0;
		for (unsigned int _index = 0; _index < _count; _index++) {
			try { _result[_index] = std::move(newObject(std::move(T(_value)), _Id[_index], _strategy)); }
			catch (...) { 
				if (_success)
					_success[_index] = false;
				continue;
			}
			if (_success)
				_success[_index] = true;
			_allocated++;
		}
		return _allocated;
	}

	template < class T >
	/**
	*	\brief Performs an attempt to create '_count' objects by calling newObject on copy of deferenced '_valueptr'.
	*	Increment result on every successful (nothrow) creation.
	*	\param[in]	_valueptr	Constant pointer to the resource to be copied.
	*	\param[in]	_Id			Array of identificators.
	*	\param[out]	_result		Array of shared pointers to new objects.
	*	\param[in]	_count		Count of objects to be created.
	*	\param[in]	_strategy	Defines strategy of new object allocation.
	*	\param[out]	_success	[Optional] Per object status of successful creation.
	*	\throw nothrow
	*	\return Count of successfully allocated objects.
	**/
	virtual inline unsigned int newObject(	T* const _valueptr, const Index _Id[], std::shared_ptr<T> _result[], const unsigned int _count,
											const allocateStrategy _strategy = allocateStrategy::DEFAULT, bool _success[] = nullptr) 
	{
		return newObject(*_valueptr, _Id, _result, _count, _strategy, _success);
	}

	template < class T >
	/**
	*	\brief Performs an attempt to copy-construct new object with index '_Id' from object with index '_sourceId' and type "T".
	*	If object '_sourceId' doesn't exist return shared pointer to nullptr.
	*	Erase object with index '_Id' if it presented in current time before creating new one.
	*	\param[in]	_sourceId	Identifier of the object from which the copy is made.
	*	\param[in]	_Id			New object identifier.
	*	\param[in]	_strategy	Defines strategy of new object allocation.
	*	\param[in]	_defptr		Parameter to make template overload possible.
	*	\throw std::logic_error On exception in object copy-constructor or operator new or dynamic_pointer_cast error.
	*	\throw std::bad_alloc On not enougth memory in make_shared operation.
	*	\return Shared pointer of type "T" to new object on success and to nullptr on error.
	**/
	virtual inline auto newCopy(const Index _sourceId, const Index _Id, const allocateStrategy _strategy = allocateStrategy::NON, T* const _defptr = nullptr)
								-> decltype(std::shared_ptr<CONCEPT_CLEAR_TYPE_T(T)>())
	{
		CONCEPT_CLEAR_TYPE(T, _ObjType)
		CONCEPT_DERIVED(_ObjType, Base, "ASSERTION_ERROR::POLYMORPHIC_MAP::newCopy::Provided type \"T\" must be derived from \"Base\".")
		CONCEPT_COPY_CONSTRUCTIBLE(_ObjType, "ASSERTION_ERROR::POLYMORPHIC_MAP::newCopy::Provided type \"T\" must be copy constuctible.")
		std::shared_ptr<_ObjType> _newptr(nullptr);
		if (_sourceId == _Id)
			return _newptr;
		auto _sourceIterator = storage.find(_sourceId);
		if (_sourceIterator == storage.end()) {
			#ifdef DEBUG_POLYMORPHICMAP
				DEBUG_NEW_MESSAGE("ERROR::POLYMORPHIC_MAP::newCopy")
					DEBUG_WRITE2("\tMessage: Invalid '_sourceId': ", _sourceId);
				DEBUG_END_MESSAGE
			#endif
			return _newptr;
		}
		auto _destIterator = storage.find(_Id);
		//_sourceIterator stays unaffected:
		//Entry: http://en.cppreference.com/w/cpp/container/map/erase
		if (_destIterator != storage.end()) 
			storage.erase(_destIterator);
		try {
			if ((_strategy == allocateStrategy::NON &&
				(_sourceIterator->second.get()->*_getAllocStrategy)() == allocateStrategy::BIG) ||
				_strategy == allocateStrategy::BIG)
			{
				_newptr.reset(new _ObjType(*(std::dynamic_pointer_cast<_ObjType>(_sourceIterator->second))));
			} else {
				_newptr = std::move(std::make_shared(*(std::dynamic_pointer_cast<_ObjType>(_sourceIterator->second))));
			}
		}
		//std::make_shared exception : not enougth memory
		catch (const std::bad_alloc&) { throw; }
		//Warp up external exception to std::logic_error
		catch (const std::exception& e) { throw std::logic_error(e.what()); }
		//Provide any other throw with std::logic_error
		catch (...) { throw std::logic_error("ERROR::POLYMORPHIC_MAP::newCopy::Object creation error."); }
		storage[_Id] = _newptr;
		return _newptr;
	}

	template < class T >
	/**
	*	\brief Performs an attempt to copy-construct new object from object with index '_sourceId' in place of object with index '_destId'.
	*	If object '_sourceId' or '_destId' doesn't exist return shared pointer to nullptr.
	*	Performs check on '_sourceId' and '_destId' objects, then creates new one.
	*	\param[in]	_sourceId	Identifier of the object from which the copy is made.
	*	\param[in]	_destId		Identifier of object rewritten with new copy.
	*	\param[in]	_strategy	Defines strategy of new object allocation.
	*	\param[in]	_defptr		Parameter to make template overload possible.
	*	\throw std::logic_error On exception in object copy-constructor or operator new or dynamic_pointer_cast error.
	*	\throw std::bad_alloc On not enougth memory in make_shared operation.
	*	\return Shared pointer of type "T" to new object on success and to nullptr on error.
	**/
	virtual inline auto copyObject(	const Index _sourceId, const Index _destId,
									const allocateStrategy _strategy = allocateStrategy::NON, T* const _defptr = nullptr) 
									-> decltype(std::shared_ptr<CONCEPT_CLEAR_TYPE_T(T)>())
	{
		CONCEPT_CLEAR_TYPE(T, _ObjType)
		CONCEPT_DERIVED(_ObjType, Base, "ASSERTION_ERROR::POLYMORPHIC_MAP::copyObject::Provided type \"T\" must be derived from \"Base\".")
		CONCEPT_COPY_CONSTRUCTIBLE(_ObjType, "ASSERTION_ERROR::POLYMORPHIC_MAP::copyObject::Provided type \"T\" must be copy constuctible.")
		std::shared_ptr<_ObjType> _newptr(nullptr);
		if (_sourceId == _destId)
			return _newptr;
		auto _sourceIterator = storage.find(_sourceId);
		if (_sourceIterator == storage.end()) {
			#ifdef DEBUG_POLYMORPHICMAP
				DEBUG_NEW_MESSAGE("ERROR::POLYMORPHIC_MAP::copyObject")
					DEBUG_WRITE2("\tMessage: Invalid '_sourceId': ", _sourceId);
				DEBUG_END_MESSAGE
			#endif
			return _newptr;
		}
		if (!storage.erase(_destId)) {
			#ifdef DEBUG_POLYMORPHICMAP
				DEBUG_NEW_MESSAGE("ERROR::POLYMORPHIC_MAP::copyObject")
					DEBUG_WRITE3("\tMessage: Object with '_destId': ", _destId, " doesn't exists.");
				DEBUG_END_MESSAGE
			#endif
			return _newptr;
		}
		try { 
			if ((_strategy == allocateStrategy::NON &&
				(_sourceIterator->second.get()->*_getAllocStrategy)() == allocateStrategy::BIG) ||
				_strategy == allocateStrategy::BIG)
			{
				_newptr.reset(new _ObjType(*(std::dynamic_pointer_cast<_ObjType>(_sourceIterator->second))));
			} else {
				_newptr = std::move(std::make_shared(*(std::dynamic_pointer_cast<_ObjType>(_sourceIterator->second))));
			}
		}
		//std::make_shared exception : not enougth memory
		catch (const std::bad_alloc&) { throw; }
		//Warp up external exception to std::logic_error
		catch (const std::exception& e) { throw std::logic_error(e.what()); }
		//Provide any other throw with std::logic_error
		catch (...) { throw std::logic_error("ERROR::POLYMORPHIC_MAP::copyObject::Object creation error."); }
		storage[_destId] = _newptr;
		return _newptr;
	}

	template < class T >
	/**
	*	\brief Moves object '_sourceId' to place of object '_destId'.
	*	Performs swap and erase strategy.
	*	Erase object with index '_destId' if it presented in current time.
	*	\param[in]	_sourceId	Identifier of the object being moved.
	*	\param[in]	_destId		Identifier of overwritten object.
	*	\param[in]	_defptr		Parameter to make template overload possible.
	*	\throw nothrow
	*	\return Shared pointer of type "T" to moved object on success and to nullptr on error.
	**/
	virtual inline std::shared_ptr<T> moveObject(const Index _sourceId, const Index _destId, T* const _defptr = nullptr) {
		CONCEPT_DERIVED(T, Base, "ASSERTION_ERROR::POLYMORPHIC_MAP::moveObject::Provided type \"T\" must be derived from \"Base\".")
		if (_sourceId == _destId)
			return std::shared_ptr<T>(nullptr);
		auto _sourceIterator = storage.find(_sourceId);
		if (_sourceIterator == storage.end()) {
			#ifdef DEBUG_POLYMORPHICMAP
				DEBUG_NEW_MESSAGE("ERROR::POLYMORPHIC_MAP::moveObject")
					DEBUG_WRITE2("\tMessage: Invalid '_sourceId': ", _sourceId);
				DEBUG_END_MESSAGE
			#endif
			return std::shared_ptr<T>(nullptr);
		}
		auto _destIterator = storage.find(_destId);
		if (_destIterator == storage.end()) {
			storage[_destId] = std::move(_sourceIterator->second);
			_destIterator = storage.find(_destId);
		} else {
			_destIterator->second.swap(_sourceIterator->second);
		}
		storage.erase(_sourceIterator);
		return std::move(std::dynamic_pointer_cast<T>(_destIterator->second));
	}

	template < class T >
	/**
	*	\brief Setups new resource by move-constructing from '_value' in place of object with index '_Id'.
	*	If object with index '_Id' not presented - calls newObject else resets saved pointer to own new object 
	*	of type "T" created with move-construction from '_value'.
	*	\param[in]	_value		Move reference to object.
	*	\param[in]	_Id			Identificator of new object.
	*	\param[in]	_strategy	Defines strategy of new object allocation.
	*	\throw std::logic_error On exception in object move-constructor or operator new.
	*	\throw std::bad_alloc On not enougth memory in make_shared operation.
	*	\return Shared pointer of type "T" to new object on success and to nullptr on error.
	**/
	virtual inline auto setObject(	T&& _value, const Index _Id, const allocateStrategy _strategy = allocateStrategy::DEFAULT)
									-> decltype(std::shared_ptr<CONCEPT_CLEAR_TYPE_T(T)>())
	{
		CONCEPT_CLEAR_TYPE(T, _ObjType)
		CONCEPT_UNREF(T, _value, "ASSERTION_ERROR::POLYMORPHIC_MAP::setObject::Provided '_value' is not rvalue or lvalue reference.")
		CONCEPT_DERIVED(_ObjType, Base, "ASSERTION_ERROR::POLYMORPHIC_MAP::setObject::Provided type \"T\" must be derived from \"Base\".")
		CONCEPT_CONSTRUCTIBLE_F(_ObjType, T, _value, "ASSERTION_ERROR::POLYMORPHIC_MAP::setObject::Provided type \"T\" must be constructible from '_value'.")
		auto _iterator = storage.find(_Id);
		if (_iterator == storage.end()) {
			try { return std::move(newObject(std::forward<T>(_value), _Id, _strategy)); }
			catch (...) { throw; }
		}
		try {
			if (_strategy == allocateStrategy::BIG) {
				_iterator->second.reset(new _ObjType(std::forward<T>(_value)));
			} else {
				_iterator->second = std::move(std::make_shared<_ObjType>(std::forward<T>(_value)));
			}
		}
		//std::make_shared exception : not enougth memory
		catch (const std::bad_alloc&) { throw; }
		//Warp up external exception to std::logic_error
		catch (const std::exception& e) { throw std::logic_error(e.what()); }
		//Provide any other throw with std::logic_error
		catch (...) { throw std::logic_error("ERROR::POLYMORPHIC_MAP::setObject::Object creation error."); }
		return std::move(std::dynamic_pointer_cast<_ObjType>(_iterator->second));
	}

	template < class T >
	/**
	*	\brief Takes ownership of resource with type "T" located by pointer '_valueptr'.
	*	Resource with index '_Id' is overwritten with newly obtained object.
	*	If object with index '_Id' not presented - calls newObject else resets saved pointer to own object pointed by '_valueptr'.
	*	Accepts nullptr objects.
	*	\param[in]	_valueptr	Pointer to resource.
	*	\param[in]	_Id			Identificator of new object.
	*	\throw nothrow
	*	\return Shared pointer of type "T" to new object on success and to nullptr on error.
	**/
	virtual inline auto setObject(T* const _valueptr, const Index _Id) NOEXCEPT -> decltype(std::shared_ptr<CONCEPT_CLEAR_TYPE_T(T)>()){
		CONCEPT_NOT_CVPR(T, "ASSERTION_ERROR::POLYMORPHIC_MAP::setObject::Provided type \"T\" must not be constant/volatile pointer or reference.")
		CONCEPT_CLEAR_TYPE(T,_ObjType)
		CONCEPT_DERIVED(_ObjType, Base, "ASSERTION_ERROR::POLYMORPHIC_MAP::setObject::Provided type \"T\" must be derived from \"Base\".")
		auto _iterator = storage.find(_Id);
		if (_iterator == storage.end())
			return std::move(newObject(_valueptr, _Id));
		_iterator->second.reset((_ObjType*)_valueptr);
		return std::move(std::dynamic_pointer_cast<_ObjType>(_iterator->second));
	}

	template < class T >
	/**
	*	\brief Performs an attempt to setup '_count' objects by calling setObject on copy of '_value'.
	*	Increment result on every successful (nothrow) creation.
	*	\param[in]	_value		Constant reference to be used in copy construction of new objects.
	*	\param[in]	_Id			Array of identificators of objects that will be replaced.
	*	\param[out]	_result		Array of shared pointers to new objects.
	*	\param[in]	_count		Count of objects to be created.
	*	\param[in]	_strategy	Defines strategy of new object allocation.
	*	\param[out]	_success	[Optional] Per object status of successful creation.
	*	\throw nothrow
	*	\return Count of successfully allocated objects.
	**/
	virtual inline unsigned int setObject(	const T& _value, const Index _Id[], std::shared_ptr<T> _result[], const unsigned int _count,
											const allocateStrategy _strategy = allocateStrategy::DEFAULT, bool _success[] = nullptr)
	{
		CONCEPT_NOT_CVPR(T, "ASSERTION_ERROR::POLYMORPHIC_MAP::setObject::Provided type \"T\" must not be constant/volatile pointer or reference.")
		CONCEPT_COPY_CONSTRUCTIBLE(T, "ASSERTION_ERROR::POLYMORPHIC_MAP::setObject::Provided type \"T\" must be copy constructible.")
		CONCEPT_DERIVED(T, Base, "ASSERTION_ERROR::POLYMORPHIC_MAP::setObject::Provided type \"T\" must be derived from \"Base\".")
		if (!_count)
			return 0;
		unsigned int _allocated = 0;
		for (unsigned int _index = 0; _index < _count; _index++) {
			try { _result[_index] = std::move(setObject(std::move(T(_value)), _Id[_index], _strategy)); }
			catch (...) { 
				if (_success)
					_success[_index] = false;
				continue;
			}
			if (_success)
				_success[_index] = true;
			_allocated++;
		}
		return _allocated;
	}

	template < class T >
	/**
	*	\brief Performs an attempt to setup '_count' objects by calling setObject on copy of deferenced '_valueptr'.
	*	Increment result on every successful (nothrow) creation.
	*	\param[in]	_valueptr	Constant pointer to the resource to be copied.
	*	\param[in]	_Id			Array of identificators.
	*	\param[out]	_result		Array of shared pointers to new objects.
	*	\param[in]	_count		Count of objects to be created.
	*	\param[in]	_strategy	Defines strategy of new object allocation.
	*	\param[out]	_success	[Optional] Per object status of successful creation.
	*	\throw nothrow
	*	\return Count of successfully allocated objects.
	**/
	virtual inline unsigned int setObject(	T* const _valueptr, const Index _Id[], std::shared_ptr<T> _result[], const unsigned int _count,
											const allocateStrategy _strategy = allocateStrategy::DEFAULT, bool _success[] = nullptr) 
	{
		return setObject(*_valueptr, _Id, _result, _count, _strategy, _success);
	}

	template < class T >
	/**
	*	\brief Performs an attempt to get shared pointer to object with index '_Id'.
	*	Performs dynamic cast on copy of pointer stored under index '_Id'.
	*	\param[in]	_Id		Identificator of stored object.
	*	\param[in]	_defptr	Parameter to make template overload possible.
	*	\throw nothrow
	*	\return Shared pointer to object with index '_Id' or to nullptr on error.
	**/
	virtual inline std::shared_ptr<T> getObject(const Index _Id, T* const _defptr = nullptr) NOEXCEPT{
		CONCEPT_NOT_PR(T, "ASSERTION_ERROR::POLYMORPHIC_MAP::getObject::Provided type \"T\" must not be pointer or reference type.")
		CONCEPT_DERIVED(T, Base, "ASSERTION_ERROR::POLYMORPHIC_MAP::getObject::Provided type \"T\" must be derived from \"Base\".")
		try { return std::move(std::dynamic_pointer_cast<T>(storage.at(_Id))); }
		catch (const std::out_of_range& e) {
			#ifdef DEBUG_POLYMORPHICMAP
				DEBUG_NEW_MESSAGE("ERROR::POLYMORPHIC_MAP::getObject")
					DEBUG_WRITE3("\tMessage: Identifier '_Id': ", _Id, " doesn't exist.");
				DEBUG_END_MESSAGE
			#endif
		}
		catch (...) {
			#ifdef DEBUG_POLYMORPHICMAP
				DEBUG_NEW_MESSAGE("ERROR::POLYMORPHIC_MAP::getObject")
					DEBUG_WRITE1("\tMessage: Unknown error.");
				DEBUG_END_MESSAGE
			#endif
		}
		return std::shared_ptr<T>(nullptr);
	}

	/**
	*	\brief Safely delete stored object.
	*	Erases stored under index '_Id' shared pointer from storage.
	*	\param[in]	_Id	Identificator of resource to be deleted.
	*	\throw nothrow
	*	\return True on successfull deletion, false on else.
	**/
	virtual inline bool deleteObject(const Index _Id) {
		storage.erase(_Id);
		return true;
	}
};
#endif
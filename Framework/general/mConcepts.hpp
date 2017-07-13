#ifndef CONCEPTS_H
#define CONCEPTS_H "[multy@mConcepts.hpp]"
/**
*	DESCRIPTION:
*		Module contains implementation of some compile-time macro - concepts
*		to check template function type parameters.
*	AUTHOR:
*		Mikhail Demchenko
*		mailto:dev.echo.mike@gmail.com
*		https://github.com/echo-Mike
**/
#include <type_traits>

//Create '_new' type by removing cv- and ref/pointer qualifiers from type '_old'.
#define CONCEPT_CLEAR_TYPE(_old, _new)	using _new = std::remove_pointer_t<std::decay_t<_old>>; \
										static_assert(!std::is_pointer<_new>::value, "ASSERTION_ERROR::CONCEPTS::Type clear failure. Generated type is a pointer.");
//Create type by removing cv- and ref/pointer qualifiers from type '_Type', returns new type on it place.
#define CONCEPT_CLEAR_TYPE_T(_Type) std::remove_pointer_t<std::decay_t<_Type>>

//Check that provided type '_der' is derived from '_base'.
#define CONCEPT_DERIVED(_der, _base, _msg) static_assert(std::is_base_of<_base, _der>::value, _msg);

//Check that provided '_value' is proper rvalue-ref or lvalue-ref of type '_Type' : cv- _Type&& or cv- _Type&.
#define CONCEPT_UNREF(_Type, _value, _msg) static_assert(	std::is_rvalue_reference<decltype(_value)>::value && std::is_same<_Type, std::remove_reference_t<decltype(_value)>>::value || \
															std::is_lvalue_reference<decltype(_value)>::value && std::is_same<_Type, decltype(_value)>::value, _msg);

//Check that provided type '_toConstructType' is constructible from '_value' by forwarding it with type '_forwardType'.
#define CONCEPT_CONSTRUCTIBLE_F(_toConstructType, _forwardType, _value, _msg) static_assert(std::is_constructible<_toConstructType, decltype(std::forward<_forwardType>(_value))>::value, _msg);
//Check that provided type '_toConstructType' is constructible from '_value' by moving it.
#define CONCEPT_CONSTRUCTIBLE_M(_toConstructType, _value, _msg) static_assert(std::is_constructible<_toConstructType, decltype(std::move(_value))>::value, _msg);
//Check that provided type '_toConstructType' is constructible from '_value' CBV.
#define CONCEPT_CONSTRUCTIBLE(_toConstructType, _value, _msg) static_assert(std::is_constructible<_toConstructType, decltype(_value)>::value, _msg);
//Check that provided type '_type' is default constructible.
#define CONCEPT_DEFCONSTR(_type, _msg) static_assert(std::is_default_constructible<_type>::value, _msg);
//Check that provided type '_Type' is move constructible.
#define CONCEPT_MOVE_CONSTRUCTIBLE(_Type, _msg) static_assert(std::is_move_constructible<_Type>::value, _msg);
//Check that provided type '_Type' is nothrow move constructible.
#define CONCEPT_NOEXCEPT_MOVE_CONSTRUCTIBLE(_Type, _msg) static_assert(std::is_nothrow_move_constructible<_Type>::value, _msg);
//Check that provided type '_Type' is nothrow move constructible, returns bool on it's place.
#define CONCEPT_NOEXCEPT_MOVE_CONSTRUCTIBLE_V(_Type) std::is_nothrow_move_constructible<_Type>::value
//Check that provided type '_Type' is copy constructible.
#define CONCEPT_COPY_CONSTRUCTIBLE(_Type, _msg) static_assert(std::is_copy_constructible<_Type>::value, _msg);
//Check that provided type '_Type' is nothrow copy constructible.
#define CONCEPT_NOEXCEPT_COPY_CONSTRUCTIBLE(_Type, _msg) static_assert(std::is_nothrow_copy_constructible<_Type>::value, _msg);
//Check that provided type '_Type' is nothrow copy constructible, returns bool on it's place.
#define CONCEPT_NOEXCEPT_COPY_CONSTRUCTIBLE_V(_Type) std::is_nothrow_copy_constructible<_Type>::value

//Check that provided type '_Type' is not constant.
#define CONCEPT_NOT_CONSTANT(_Type, _msg) static_assert(!std::is_const<_Type>::value, _msg);
//Check that provided type '_Type' is not volatile.
#define CONCEPT_NOT_VOLATILE(_Type, _msg) static_assert(!std::is_volatile<_Type>::value, _msg);
//Check that provided type '_Type' is not reference.
#define CONCEPT_NOT_REFERENCE(_Type, _msg) static_assert(!std::is_reference<_Type>::value, _msg);
//Check that provided type '_Type' is not pointer.
#define CONCEPT_NOT_POINTER(_Type, _msg) static_assert(!std::is_pointer<_Type>::value, _msg);
//Check that provided type '_Type' is not constant or volatile.
#define CONCEPT_NOT_CV(_Type, _msg) static_assert(!(std::is_const<_Type>::value || std::is_volatile<_Type>::value), _msg);
//Check that provided type '_Type' is not reference or pointer.
#define CONCEPT_NOT_PR(_Type, _msg) static_assert(!(std::is_pointer<_Type>::value || std::is_reference<_Type>::value), _msg);
//Check that provided type '_Type' is not reference or pointer.
#define CONCEPT_NOT_RP(_Type, _msg) static_assert(!(std::is_pointer<_Type>::value || std::is_reference<_Type>::value), _msg);
//Check that provided type '_Type' is not constant/volatile and not reference.
#define CONCEPT_NOT_CVREFERENCE(_Type, _msg) static_assert(!(std::is_volatile<_Type>::value || std::is_const<_Type>::value || std::is_reference<_Type>::value), _msg);
//Check that provided type '_Type' is not constant/volatile and not pointer.
#define CONCEPT_NOT_CVPOINTER(_Type, _msg) static_assert(!(std::is_volatile<_Type>::value || std::is_const<_Type>::value || std::is_pointer<_Type>::value), _msg);
//Check that provided type '_Type' is not constant/volatile and not pointer or reference.
#define CONCEPT_NOT_CVPR(_Type, _msg) static_assert(!(std::is_volatile<_Type>::value || std::is_const<_Type>::value || std::is_reference<_Type>::value || std::is_pointer<_Type>::value), _msg);
//Check that provided type '_Type' is not constant/volatile and not pointer or reference.
#define CONCEPT_NOT_CVRP(_Type, _msg) static_assert(!(std::is_volatile<_Type>::value || std::is_const<_Type>::value || std::is_reference<_Type>::value || std::is_pointer<_Type>::value), _msg);

//Check that provided type '_Type' is integral type.
#define CONCEPT_INTEGRAL(_Type, _mag) static_assert(std::is_integral<_Type>::value, _msg);
#endif
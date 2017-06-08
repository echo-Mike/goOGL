#ifndef UNIFORMSTRUCT_H
#define UNIFORMSTRUCT_H "[0.0.5@CUniformStruct.h]"
/*
*	DESCRIPTION:
*		Module contains implementation of in-shader struct of uniform values handling classes and templates.
*	AUTHOR:
*		Mikhail Demchenko
*		mailto:dev.echo.mike@gmail.com
*		https://github.com/echo-Mike
*/
//STD
#include <string>
#include <vector>
#include <type_traits>
//GLEW
#include <GL/glew.h>
//OUR
#include "CUniforms.h"
//DEBUG
#ifdef DEBUG_UNIFORMSTRUCT
	#ifndef DEBUG_OUT
		#define DEBUG_OUT std::cout
	#endif
	#ifndef DEBUG_NEXT_LINE
		#define DEBUG_NEXT_LINE std::endl
	#endif
#endif

#ifndef STRUCT_STD_SHADER_VARIABLE_NAME
	//In shader variable name of value
	#define STRUCT_STD_SHADER_VARIABLE_NAME "structure"
#endif

/* Base class of storage for struct of uniform in-shader values.
*  Struct template definition: StructContainerBase
*/
template < class Interface >
struct StructContainerBase {
	static_assert(	std::has_virtual_destructor<Interface>::value,
					"ASSERT_ERROR::StructContainerBase::'Interface' class must have virtual destructor");

	std::vector<Interface*> data;
	std::string structName;
	int index;

	StructContainerBase() : structName(STRUCT_STD_SHADER_VARIABLE_NAME) {}

	StructContainerBase(StructContainerBase&& other) :	structName(std::move(other.structName)), 
														index(std::move(other.index)),
														data(std::move(other.data)) {}

	~StructContainerBase() {
		for (auto &v : data)
			delete v;
	}

	StructContainerBase& operator=(StructContainerBase other) {
		if (&other == this)
			return *this;
		std::swap(structName, other.structName);
		data.swap(other.data);
		return *this;
	}
/*
	template< class T >
	void newElement(T& _value) {
		static_assert(	std::is_base_of<Interface, T>::value, 
						"ASSERT_ERROR::StructContainerBase::Only objects of class 'T' derived from 'Interface' can be created");
		data.push_back(nullptr);
		data.back() = new T(std::move(_value));
	}
*/
	template< class T >
	void newElement(T* _valueptr) {
		static_assert(	std::is_base_of<Interface, T>::value, 
						"ASSERT_ERROR::StructContainerBase::Only objects of class 'T' derived from 'Interface' can be created");
		static_assert(	std::is_move_constructible<T>::value,
						"ASSERTION_ERROR::StructContainerBase::Provided class 'T' must be move-constructible.");
		data.push_back(nullptr);
		data.back() = new T(std::move(*_valueptr));
	}

	template< class T >
	void newElement(T&& _value) {
		static_assert(	std::is_base_of<Interface, T>::value, 
						"ASSERT_ERROR::StructContainerBase::Only objects of class 'T' derived from 'Interface' can be created");
		static_assert(	std::is_move_constructible<T>::value,
						"ASSERTION_ERROR::StructContainerBase::Provided class 'T' must be move-constructible.");
		data.push_back(nullptr);
		data.back() = new T(std::move(_value));
	}

	template< class T >
	void setElement(int _index, T&& _value) {
		static_assert(	std::is_base_of<Interface, T>::value, 
						"ASSERT_ERROR::StructContainerBase::Only objects of class 'T' derived from 'Interface' can be created");
		static_assert(	std::is_move_constructible<T>::value,
						"ASSERTION_ERROR::StructContainerBase::Provided class 'T' must be move-constructible.");
		delete data.at(_index);
		data.at(_index) = new T(std::move(_value));
	}

	virtual void operator() (std::string _structName) { return; }

	//Sets current struct as separate value
	void asValue() {
		index = -1;
		this->operator()(structName);
	}

	//Sets current struct as member of in-shader array of struct at position "_index"
	void asArrayMember(const int _index) {
		if (_index < 0) {
			asValue();
		} else {
			index = _index;
			std::string _buff(structName);
			this->operator()(structName + "[" + std::to_string(_index) + "]");
			structName = std::move(_buff);
		}
	}

	//Functionality of asArrayMember and asValue binded to operator[] call
	virtual void operator[] (const int _index) { asArrayMember(_index);	}
};

/* Base class of manual storage for struct of uniform in-shader values.
*  Struct definition: StructManualContainer
*/
struct StructManualContainer : public StructContainerBase<UniformManualInteface> {

	StructManualContainer() : StructContainerBase() {}

	StructManualContainer(StructManualContainer&& other) : StructContainerBase<UniformManualInteface>(std::move(other)) {}

	StructManualContainer& operator=(StructManualContainer other) {
		if (&other == this)
			return *this;
		StructContainerBase<UniformManualInteface>::operator=(other);
		return *this;
	}

	virtual void bindData() {
		for (auto &v : data) 
			v->bindData();
	}
};

/* Base class of automatic storage for struct of uniform in-shader values.
*  Struct definition: StructAutomaticContainer
*/
struct StructAutomaticContainer : public StructContainerBase<UniformAutomaticInteface> {

	StructAutomaticContainer() : StructContainerBase() {}

	StructAutomaticContainer(StructAutomaticContainer&& other) : StructContainerBase<UniformAutomaticInteface>(std::move(other)) {}

	StructAutomaticContainer& operator=(StructAutomaticContainer other) {
		if (&other == this)
			return *this;
		StructContainerBase<UniformAutomaticInteface>::operator=(other);
		return *this;
	}

	virtual void push() {
		for (auto &v : data)
			v->push();
	}

	virtual void pull() {
		for (auto &v : data)
			v->pull();
	}
};
#endif
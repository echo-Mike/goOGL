#ifndef UNIFORMSTRUCT_H
#define UNIFORMSTRUCT_H "[0.0.3@CUniformStruct.h]"
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

/* Base class of manual storage for struct of uniform in-shader values.
*  Class definition: StructManualContainer
*/
struct StructManualContainer {
	std::vector<UniformManualInteface*> data;
	std::string structName;

	StructManualContainer() : structName(STRUCT_STD_SHADER_VARIABLE_NAME) {}

	StructManualContainer& operator=(StructManualContainer other) {
		if (&other == this)
			return *this;
		std::swap(structName, other.structName);
		data.swap(other.data);
		return *this;
	}

	template< class T >
	void newElement(T _value) {
		data.push_back(nullptr);
		data.back() = new T(_value);
	}

	template< class T >
	void newElement(T* _valueptr) {
		data.push_back(nullptr);
		data.back() = new T(*_valueptr);
	}

	template< class T >
	void setElement(int _index, T _value) {
		delete data.at(_index);
		data.at(_index) = new T(_value);
	}

	virtual void bindData() {
		for (auto &v : data) 
			v->bindData();
	}
};

/* Base class of automatic storage for struct of uniform in-shader values.
*  Class definition: StructAutomaticContainer
*/
struct StructAutomaticContainer {
	std::vector<UniformAutomaticInteface*> data;
	std::string structName;

	StructAutomaticContainer() : structName(STRUCT_STD_SHADER_VARIABLE_NAME) {}

	StructAutomaticContainer& operator=(StructAutomaticContainer other) {
		if (&other == this)
			return *this;
		std::swap(structName, other.structName);
		data.swap(other.data);
		return *this;
	}

	template< class T >
	void newElement(T& _value) {
		data.push_back(nullptr);
		data.back() = new T(std::move(_value));
	}

	template< class T >
	void newElement(T* _valueptr) {
		data.push_back(nullptr);
		data.back() = new T(std::move(*_valueptr));
	}

	template< class T >
	void setElement(int _index, T& _value) {
		delete data.at(_index);
		data.at(_index) = new T(std::move(_value));
	}

	virtual void push() {
		for (auto &v : data)
			v->push();
	}
};
#endif
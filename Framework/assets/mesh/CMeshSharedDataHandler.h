#ifndef MESHSHAREDDATAHANDLER_H
#define MESHSHAREDDATAHANDLER_H "[0.0.5@CMeshSharedDataHandler.h]"
/*
*	DESCRIPTION:
*		Module contains implementation of object that handles shared data of all mesh instances.
*	AUTHOR:
*		Mikhail Demchenko
*		mailto:dev.echo.mike@gmail.com
*		https://github.com/echo-Mike
*/
//STD
#include <vector>
#include <type_traits>
//OUR
#include "assets\shader\CShader.h"
#include "assets\shader\CUniforms.h"
//DEBUG
#ifdef DEBUG_MESHSHAREDDATAHANDLER
	#ifndef DEBUG_OUT
		#define DEBUG_OUT std::cout
	#endif
	#ifndef DEBUG_NEXT_LINE
		#define DEBUG_NEXT_LINE std::endl
	#endif
#endif

/* Load controller for mesh data shared among all instances.
*  Actualy the logic of work of this class is the same as StructAutomaticContainer:
*		- load all shared values to shader one time for each draw call of mesh
*  So all stored data must be derived from UniformAutomaticInterface
*  This class was not derived from StructAutomaticContainer because of memory efficiency
*  Class definition: MeshSharedDataHandler
*/
template < class TShader = Shader >
struct MeshSharedDataHandler {
	//Dynamic container for data shared among all instances
	std::vector<UniformAutomaticInteface*> sharedData;

	MeshSharedDataHandler() {}

	MeshSharedDataHandler(MeshSharedDataHandler&& other) : sharedData(std::move(other.sharedData)) {}

	~MeshSharedDataHandler() {
		for (auto &v : sharedData)
			delete v;
	}

	MeshSharedDataHandler& operator=(MeshSharedDataHandler other) {
		if (&other == this)
			return *this;
		sharedData.swap(other.sharedData);
		return *this;
	}

	template < class T >
	void newSharedData(T* _valueptr) {
		static_assert(	std::is_base_of<UniformAutomaticInteface, T>::value, 
						"ASSERTION_ERROR::MeshSharedDataHandler::Only objects of class 'T' derived from 'UniformAutomaticInteface' can be created.");
		static_assert(	std::is_move_constructible<T>::value,
						"ASSERTION_ERROR::MeshSharedDataHandler::Provided class 'T' must be move-constructible.");
		sharedData.push_back(nullptr);
		sharedData.back() = new T(std::move(*_valueptr));
	}

	template < class T >
	void newSharedData(T&& _value) {
		static_assert(	std::is_base_of<UniformAutomaticInteface, T>::value, 
						"ASSERTION_ERROR::MeshSharedDataHandler::Only objects of class 'T' derived from 'UniformAutomaticInteface' can be created.");
		static_assert(	std::is_move_constructible<T>::value,
						"ASSERTION_ERROR::MeshSharedDataHandler::Provided class 'T' must be move-constructible.");
		sharedData.push_back(nullptr);
		sharedData.back() = new T(std::move(_value));
	}

	template < class T >
	void setSharedData(int _index, T&& _value) {
		static_assert(	std::is_base_of<UniformAutomaticInteface, T>::value, 
						"ASSERTION_ERROR::MeshSharedDataHandler::Only objects of class 'T' derived from 'UniformAutomaticInteface' can be created.");
		static_assert(	std::is_move_constructible<T>::value,
						"ASSERTION_ERROR::MeshSharedDataHandler::Provided class 'T' must be move-constructible.");
		try {
			delete sharedData.at(_index);
			sharedData.at(_index) = new T(std::move(_value));
		}
		catch (...) { throw; }
	}

	template < class T> //, void (T::* SetShader)(TShader*) = &T::setShader > //Only programmer can guarantee that T has setShader function
	void sharedDataSetShader(TShader* _shader, int _index) {
		static_assert(	std::is_base_of<UniformAutomaticInteface, T>::value, 
						"ASSERTION_ERROR::MeshSharedDataHandler::Only objects of class 'T' derived from 'UniformAutomaticInteface' can be created.");
		try {
			dynamic_cast<T*>(sharedData.at(_index))->setShader(_shader);
			//(dynamic_cast<T*>(sharedData.at(_index))->*SetShader)(_shader);
		}
		catch (...) { throw; }
	}

	virtual void push() {
		for (auto &v : sharedData)
			v->push();
	}

	virtual void pull() {
		for (auto &v : sharedData)
			v->pull();
	}
};
#endif
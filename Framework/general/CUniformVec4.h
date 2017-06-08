#ifndef UNIFORMVEC4_H
#define UNIFORMVEC4_H "[0.0.5@CUniformVec4.h]"
/*
*	DESCRIPTION:
*		Module contains implementation of in-shader uniform vec4f handling classes and templates based on uniform handling.
*	AUTHOR:
*		Mikhail Demchenko
*		mailto:dev.echo.mike@gmail.com
*		https://github.com/echo-Mike
*/
//STD
#include <string>
//GLEW
#include <GL/glew.h>
//GLM
#include <GLM/glm.hpp>
//OUR
#include "assets\shader\CUniforms.h"
#include "assets\shader\CShader.h"
//DEBUG
#ifdef DEBUG_UNIFORMVEC4
	#ifndef DEBUG_OUT
		#define DEBUG_OUT std::cout
	#endif
	#ifndef DEBUG_NEXT_LINE
		#define DEBUG_NEXT_LINE std::endl
	#endif
#endif

#ifndef VEC4_STD_SHADER_VARIABLE_NAME
	//In shader variable name of matrix
	#define VEC4_STD_SHADER_VARIABLE_NAME "vector4"
#endif

/* The automatic storage for uniform vec4f in-shader value.
*  Class template definition: Vec4AutomaticStorage
*/
template < class TVector = glm::vec4, class TShader = Shader >
class Vec4AutomaticStorage : public UniformAutomaticStorage<TVector, TShader> {
	typedef UniformAutomaticStorage<TVector, TShader> Base;
public:
	Vec4AutomaticStorage() : Base(&(TVector()), nullptr, VEC4_STD_SHADER_VARIABLE_NAME) {}

	Vec4AutomaticStorage(	TVector* _vector, TShader* _shader,
							const char* _name = VEC4_STD_SHADER_VARIABLE_NAME) :
							Base(_vector, _shader, _name) {}

	Vec4AutomaticStorage(	TVector* _vector, TShader* _shader,
							std::string _name = std::string(VEC4_STD_SHADER_VARIABLE_NAME)) :
							Base(_vector, _shader, _name) {}

	Vec4AutomaticStorage(const Vec4AutomaticStorage& other) : Base(other) {}

	Vec4AutomaticStorage(Vec4AutomaticStorage&& other) : Base(std::move(other)) {}
	
	//Bind vec4 to shader
	void bindUniform(GLint _location) {
		glUniform4f(_location, value.x, value.y, value.z, value.w);
	}
};

/* The automatic observer for uniform vec4f in-shader value.
*  Class template definition: Vec4AutomaticObserver
*/
template < class TVector = glm::vec4, class TShader = Shader >
class Vec4AutomaticObserver : public UniformAutomaticObserver<TVector, TShader> {
	typedef UniformAutomaticObserver<TVector, TShader> Base;
public:
	Vec4AutomaticObserver() : Base(nullptr, nullptr, VEC4_STD_SHADER_VARIABLE_NAME) {}

	Vec4AutomaticObserver(	TVector* _vector, TShader* _shader,
							const char* _name = VEC4_STD_SHADER_VARIABLE_NAME) :
							Base(_vector, _shader, _name) {}

	Vec4AutomaticObserver(	TVector* _vector, TShader* _shader,
							std::string _name = std::string(VEC4_STD_SHADER_VARIABLE_NAME)) :
							Base(_vector, _shader, _name) {}
	
	Vec4AutomaticObserver(const Vec4AutomaticObserver& other) : Base(other) {}

	Vec4AutomaticObserver(Vec4AutomaticObserver&& other) : Base(std::move(other)) {}

	//Bind vec4 to shader
	void bindUniform(GLint _location) {
		glUniform4f(_location, valueptr->x, valueptr->y, valueptr->z, valueptr->w);
	}
};

/* The manual storage for uniform vec4f in-shader value.
*  Class template definition: Vec4ManualStorage
*/
template <	class TVector = glm::vec4, class TShader = Shader,
			GLint(TShader::* _getUniformLocation)(const char*) = &TShader::getUniformLocation >
class Vec4ManualStorage : public UniformManualStorage<TVector, TShader> {
	typedef UniformManualStorage<TVector, TShader> Base;
public:
	Vec4ManualStorage() : Base(nullptr, nullptr, VEC4_STD_SHADER_VARIABLE_NAME) {}

	Vec4ManualStorage(	TVector* _vector, TShader* _shader,
						const char* _name = VEC4_STD_SHADER_VARIABLE_NAME) :
						Base(_vector, _shader, _name) {}

	Vec4ManualStorage(	TVector* _vector, TShader* _shader,
						std::string _name = std::string(VEC4_STD_SHADER_VARIABLE_NAME)) :
						Base(_vector, _shader, _name) {}

	Vec4ManualStorage(const Vec4ManualStorage& other) : Base(other) {}
	
	//Bind vec3 to shader
	void bindData() {
		GLint _location = (shader->*_getUniformLocation)(uniformName.c_str());
		if (_location == -1) { //Check if uniform not found
			#ifdef DEBUG_UNIFORMVEC4
				DEBUG_OUT << "ERROR::VEC4_MANUAL_STORAGE::bindData::UNIFORM_NAME_MISSING" << DEBUG_NEXT_LINE;
				DEBUG_OUT << "\tName: " << uniformName << DEBUG_NEXT_LINE;
			#endif
			return;
		}
		glUniform4f(_location, value.x, value.y, value.z, value.w);
	}
};
#endif
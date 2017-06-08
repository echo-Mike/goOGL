#ifndef UNIFORMVEC3_H
#define UNIFORMVEC3_H "[0.0.5@CUniformVec3.h]"
/*
*	DESCRIPTION:
*		Module contains implementation of in-shader uniform vec3f handling classes and templates based on uniform handling.
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
#ifdef DEBUG_UNIFORMVEC3
	#ifndef DEBUG_OUT
		#define DEBUG_OUT std::cout
	#endif
	#ifndef DEBUG_NEXT_LINE
		#define DEBUG_NEXT_LINE std::endl
	#endif
#endif

#ifndef VEC3_STD_SHADER_VARIABLE_NAME
	//In shader variable name of matrix
	#define VEC3_STD_SHADER_VARIABLE_NAME "vector3"
#endif

/* The automatic storage for uniform vec3f in-shader value.
*  Class template definition: Vec3AutomaticStorage
*/
template < class TVector = glm::vec3, class TShader = Shader >
class Vec3AutomaticStorage : public UniformAutomaticStorage<TVector, TShader> {
	typedef UniformAutomaticStorage<TVector, TShader> Base;
public:
	Vec3AutomaticStorage() : Base(&(TVector()), nullptr, VEC3_STD_SHADER_VARIABLE_NAME) {}

	Vec3AutomaticStorage(	TVector* _vector, TShader* _shader,
							const char* _name = VEC3_STD_SHADER_VARIABLE_NAME) :
							Base(_vector, _shader, _name) {}

	Vec3AutomaticStorage(	TVector* _vector, TShader* _shader,
							std::string _name = std::string(VEC3_STD_SHADER_VARIABLE_NAME)) :
							Base(_vector, _shader, _name) {}

	Vec3AutomaticStorage(const Vec3AutomaticStorage& other) : Base(other) {}
	
	Vec3AutomaticStorage(Vec3AutomaticStorage&& other) : Base(std::move(other)) {}

	//Bind vec3 to shader
	void bindUniform(GLint _location) {
		glUniform3f(_location, value.x, value.y, value.z);
	}
};

/* The automatic observer for uniform vec3f in-shader value.
*  Class template definition: Vec3AutomaticObserver
*/
template < class TVector = glm::vec3, class TShader = Shader >
class Vec3AutomaticObserver : public UniformAutomaticObserver<TVector, TShader> {
	typedef UniformAutomaticObserver<TVector, TShader> Base;
public:
	Vec3AutomaticObserver() : Base(nullptr, nullptr, VEC3_STD_SHADER_VARIABLE_NAME) {}

	Vec3AutomaticObserver(	TVector* _vector, TShader* _shader,
							const char* _name = VEC3_STD_SHADER_VARIABLE_NAME) :
							Base(_vector, _shader, _name) {}

	Vec3AutomaticObserver(	TVector* _vector, TShader* _shader,
							std::string _name = std::string(VEC3_STD_SHADER_VARIABLE_NAME)) :
							Base(_vector, _shader, _name) {}
	
	Vec3AutomaticObserver(const Vec3AutomaticObserver& other) : Base(other) {}

	Vec3AutomaticObserver(Vec3AutomaticObserver&& other) : Base(std::move(other)) {}

	//Bind vec3 to shader
	void bindUniform(GLint _location) {
		glUniform3f(_location, valueptr->x, valueptr->y, valueptr->z);
	}
};

/* The manual storage for uniform vec3f in-shader value.
*  Class template definition: Vec3ManualStorage
*/
template <	class TVector = glm::vec3, class TShader = Shader,
			GLint(TShader::* _getUniformLocation)(const char*) = &TShader::getUniformLocation >
class Vec3ManualStorage : public UniformManualStorage<TVector, TShader> {
	typedef UniformManualStorage<TVector, TShader> Base;
public:
	Vec3ManualStorage() : Base(nullptr, nullptr, VEC3_STD_SHADER_VARIABLE_NAME) {}

	Vec3ManualStorage(	TVector* _vector, TShader* _shader,
						const char* _name = VEC3_STD_SHADER_VARIABLE_NAME) :
						Base(_vector, _shader, _name) {}

	Vec3ManualStorage(	TVector* _vector, TShader* _shader,
						std::string _name = std::string(VEC3_STD_SHADER_VARIABLE_NAME)) :
						Base(_vector, _shader, _name) {}

	Vec3ManualStorage(const Vec3ManualStorage& other) : Base(other) {}

	Vec3ManualStorage(Vec3ManualStorage&& other) : Base(std::move(other)) {}
	
	//Bind vec3 to shader
	void bindData() {
		GLint _location = (shader->*_getUniformLocation)(uniformName.c_str());
		if (_location == -1) { //Check if uniform not found
			#ifdef DEBUG_UNIFORMVEC3
				DEBUG_OUT << "ERROR::VEC3_MANUAL_STORAGE::bindData::UNIFORM_NAME_MISSING" << DEBUG_NEXT_LINE;
				DEBUG_OUT << "\tName: " << uniformName << DEBUG_NEXT_LINE;
			#endif
			return;
		}
		glUniform3f(_location, value.x, value.y, value.z);
	}
};

/* The automatic observer for uniform vec3f in-shader value.
*  Class template definition: Vec3AutomaticObserver
*/
template <	class TVector = glm::vec3, class TShader = Shader,
			GLint(TShader::* _getUniformLocation)(const char*) = &TShader::getUniformLocation >
class Vec3ManualObserver : public UniformManualObserver<TVector, TShader> {
	typedef UniformManualObserver<TVector, TShader> Base;
public:
	Vec3ManualObserver() : Base(nullptr, nullptr, VEC3_STD_SHADER_VARIABLE_NAME) {}

	Vec3ManualObserver(	TVector* _vector, TShader* _shader,
						const char* _name = VEC3_STD_SHADER_VARIABLE_NAME) :
						Base(_vector, _shader, _name) {}

	Vec3ManualObserver(	TVector* _vector, TShader* _shader,
						std::string _name = std::string(VEC3_STD_SHADER_VARIABLE_NAME)) :
						Base(_vector, _shader, _name) {}
	
	Vec3ManualObserver(const Vec3ManualObserver& other) : Base(other) {}

	Vec3ManualObserver(Vec3ManualObserver&& other) : Base(std::move(other)) {}

	//Bind vec3 to shader
	void bindData() {
		GLint _location = (shader->*_getUniformLocation)(uniformName.c_str());
		if (_location == -1) { //Check if uniform not found
			#ifdef DEBUG_UNIFORMVEC3
				DEBUG_OUT << "ERROR::VEC3_MANUAL_OBSERVER::bindData::UNIFORM_NAME_MISSING" << DEBUG_NEXT_LINE;
				DEBUG_OUT << "\tName: " << uniformName << DEBUG_NEXT_LINE;
			#endif
			return;
		}
		glUniform3f(_location, valueptr->x, valueptr->y, valueptr->z);
	}
};
#endif
#ifndef UNIFORMVEC3_H
#define UNIFORMVEC3_H "[0.0.1@CUniformVec3.h]"
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
#include "assets/shader/CUniforms.h"
//DEBUG
#ifdef DEBUG_UNIFORMVEC3
	#ifndef DEBUG_OUT
		#define DEBUG_OUT std::cout
	#endif
	#ifndef DEBUG_NEXT_LINE
		#define DEBUG_NEXT_LINE std::endl
	#endif
#endif

/* The automatic storage for uniform vec3f in-shader value.
*  Class template definition: Vec3Handler
*/
#ifndef VEC3_HANDLER_STD_SHADER_VARIABLE_NAME
	//In shader variable name of matrix
#define VEC3_HANDLER_STD_SHADER_VARIABLE_NAME "vector3"
#endif

template < class TVector = glm::vec3, class TShader = Shader >
class Vec3Handler : public UniformHandler<TVector, TShader> {
public:
	Vec3Handler() : UniformHandler(TVector(), nullptr, VEC3_HANDLER_STD_SHADER_VARIABLE_NAME) {}

	Vec3Handler(TVector _vector, TShader* _shader,
				const char* _name = VEC3_HANDLER_STD_SHADER_VARIABLE_NAME) :
				UniformHandler(_vector, _shader, _name) {}

	Vec3Handler(TVector _vector, TShader* _shader,
				std::string _name = std::string(VEC3_HANDLER_STD_SHADER_VARIABLE_NAME)) :
				UniformHandler(_vector, _shader, _name) {}
	
	//Bind vec3 to shader
	void bindUniform(GLint _location) {
		glUniform3f(_location, value.x, value.y, value.z);
	}
};

#ifndef VEC3_LOADER_STD_SHADER_VARIABLE_NAME
	//In shader variable name of matrix
	#define VEC3_LOADER_STD_SHADER_VARIABLE_NAME "vector3"
#endif

/* The manual storage for uniform vec3f in-shader value.
*  Class template definition: Vec3Loader
*/
template <	class TVector = glm::vec3, class TShader = Shader,
			GLint (TShader::* _getUniformLocation)(const char*) = &TShader::getUniformLocation >
class Vec3Loader : public UniformLoader<TVector, TShader> {
public:
	Vec3Loader() : UniformHandler(TVector(), nullptr, VEC3_LOADER_STD_SHADER_VARIABLE_NAME) {}

	Vec3Loader(	TVector _vector, TShader* _shader,
				const char* _name = VEC3_LOADER_STD_SHADER_VARIABLE_NAME) :
				UniformHandler(_vector, _shader, _name) {}

	Vec3Loader(	TVector _vector, TShader* _shader,
				std::string _name = std::string(VEC3_LOADER_STD_SHADER_VARIABLE_NAME)) :
				UniformHandler(_vector, _shader, _name) {}
	
	//Bind matrix to shader
	void bindData() {
		GLint _location = (shader->*_getUniformLocation)(uniformName.c_str());
		if (_location == -1) { //Check if uniform not found
			#ifdef DEBUG_UNIFORMVEC3
				DEBUG_OUT << "ERROR::VEC3_LOADER::bindData::UNIFORM_NAME_MISSING" << DEBUG_NEXT_LINE;
				DEBUG_OUT << "\tName: " << uniformName.c_str() << DEBUG_NEXT_LINE;
			#endif
			return;
		}
		glUniform3f(_location, value.x, value.y, value.z);
	}
};
#endif
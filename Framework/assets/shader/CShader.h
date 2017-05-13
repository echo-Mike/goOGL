#ifndef SHADER_H
#define SHADER_H "[0.0.4@CShader.h]"
/*
*	DESCRIPTION:
*		Module contains implementation of shader class.
*	AUTHOR:
*		Mikhail Demchenko
*		mailto:dev.echo.mike@gmail.com
*		https://github.com/echo-Mike
*	PAIRED WITH: CShader.cpp
*/
//STD
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
//std::map
#include <map>
//std::pair, std::make_pair
#include <utility>
//GLEW
#include <GL/glew.h>
//OUR
#include "CUniforms.h"
//DEBUG

#ifdef DEBUG_SHADER
	#ifndef DEBUG_OUT
		#define DEBUG_OUT std::cout
	#endif
	#ifndef DEBUG_NEXT_LINE
		#define DEBUG_NEXT_LINE std::endl
	#endif
#else 
	#ifndef DEBUG_SHADERCPP
		#define DEBUG_SHADERCPP
	#endif
	#define DEBUG_OUT std::cout
	#define DEBUG_NEXT_LINE std::endl
#endif

//Class definition: Shader
class Shader {
	//NO STDCONSTRUCT, NO MOVECONSTRUCT, NO COPYCONSTRUCT
	Shader() = delete;
	Shader(const Shader& s) = delete;
	Shader(const Shader&& s) = delete;
	//Uniform container
	std::map<std::string, UniformAutomaticInteface*> uniforms;
	//The program ID //SPO ID
	GLuint Program;
public:
	//Vertex and Fragment shader paths
	std::string vpath, fpath;

	//Returns SPO Id
	GLuint getShaderId() { return Program; }

	//Returns result of uniform search: location detemined by name
	GLint getUniformLocation(const char* _uniformName) {
		GLint _location;
		_location = glGetUniformLocation(Program, _uniformName);
		if (_location == -1) { //Check if uniform not found
			#ifdef DEBUG_SHADER
				DEBUG_OUT << "ERROR::SHADER::getUniformLocation::UNIFORM_NAME_MISSING"<< DEBUG_NEXT_LINE;
				DEBUG_OUT << "\tName: " << _uniformName << DEBUG_NEXT_LINE;
				DEBUG_OUT << "\tFPath: " << fpath << DEBUG_NEXT_LINE;
			#endif
		}
		return _location;
	}

	//Returns result of uniform search: location detemined by name
	GLint getUniformLocation(std::string &_uniformName) {
		GLint _location;
		_location = glGetUniformLocation(Program, _uniformName.c_str());
		if (_location == -1) { //Check if uniform not found
			#ifdef DEBUG_SHADER
				DEBUG_OUT << "ERROR::SHADER::getUniformLocation::UNIFORM_NAME_MISSING"<< DEBUG_NEXT_LINE;
				DEBUG_OUT << "\tName: " << _uniformName << DEBUG_NEXT_LINE;
				DEBUG_OUT << "\tFPath: " << fpath << DEBUG_NEXT_LINE;
			#endif
		}
		return _location;
	}

	Shader(const GLchar* vertexPath, const GLchar* fragmentPath) :	vpath(vertexPath), 
																	fpath(fragmentPath), 
																	Program(0)
	{ 
		Reload(); 
	}
	
	~Shader() {
		//Prevent external values from destructor calls
		for (auto &v : uniforms)
			v.second = nullptr;
		glDeleteProgram(this->Program); 
	}

	void newUniform(std::string &_uniformName, UniformAutomaticInteface* _handler) {
		uniforms[_uniformName] = _handler;
	}

	void deleteUniform(const char* _key) { uniforms.erase(std::string(_key)); }

	void deleteUniform(std::string &_key) { uniforms.erase(_key); }
	
	//Find uniform in queue by it's name
	bool find(std::string &_key) { return uniforms.find(_key) != uniforms.end(); }

	//Find uniform in queue by pointer to it
	bool find(UniformAutomaticInteface* _toFind) {
		for (auto &_value : uniforms)
			if (_value.second == _toFind)
				return true;
		return false;
	}

	void Use();

	void Reload();
};
#endif
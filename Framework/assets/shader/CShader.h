#ifndef SHADER_H
#define SHADER_H "[0.0.1@CShader.h]"
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
#endif

//Class definition: Shader
class Shader {	
	//NO STDCONSTRUCT, NO MOVECONSTRUCT, NO COPYCONSTRUCT
	Shader() = delete;
	Shader(const Shader& s) = delete;
	Shader(const Shader&& s) = delete;
	//Vertex and Fragment shader paths
	std::string vpath, fpath;
	//Uniform container
	std::map<int, std::pair<std::string, UniformHandlerInteface*>> uniforms;
	//The program ID //SPO ID
	GLuint Program;
	//NOT SAFE SOLUTION: OVERFLOW
	//Id counter for current shader
	int incrementId;
public:
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
			#endif
		}
		return _location;
	}

	Shader(const GLchar* vertexPath, const GLchar* fragmentPath) :	vpath(vertexPath), 
																	fpath(fragmentPath), 
																	Program(0), 
																	incrementId(0) 
	{ 
		Reload(); 
	}
	
	~Shader() {
		//WEAK DESISION: destructor calls on const ptr?
		//Prevent external values from destructor calls
		for (auto &v : uniforms)
			v.second.second = nullptr;
		glDeleteProgram(this->Program); 
	}

	int newUniform(const char* _uniformName, UniformHandlerInteface* _handler) {
		uniforms[incrementId] = std::move(std::make_pair(std::string(_uniformName), _handler));
		return incrementId++;
	}

	void deleteUniform(int _index) { uniforms.erase(_index); }

	void Use();

	void Reload();
};
#endif
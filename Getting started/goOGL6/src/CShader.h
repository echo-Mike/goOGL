#ifndef SHADER_H
#define SHADER_H
//STD
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
//std::vector
#include <vector>
//std::tuple, std::make_tuple
#include <tuple>
//GLEW
//Include glew to get all the required OpenGL headers
#include <GL/glew.h>
//DEBUG
#ifdef GEBUG_SHADER
	#ifndef DEBUG_OUT
		#define DEBUG_OUT std::cout
	#endif
	#ifndef DEBUG_NEXT_LINE
		#define DEBUG_NEXT_LINE std::endl
	#endif
#endif
//Class definition: Shader
class Shader {
public:
	enum uniformTypes {
		UNIFORM1F,
		UNIFORM1FV,
		UNIFORM1I,
		UNIFORM1IV,
		UNIFORM1UI,
		UNIFORM1UIV,
		UNIFORM2F,
		UNIFORM2FV,
		UNIFORM2I,
		UNIFORM2IV,
		UNIFORM2UI,
		UNIFORM2UIV,
		UNIFORM3F,
		UNIFORM3FV,
		UNIFORM3I,
		UNIFORM3IV,
		UNIFORM3UI,
		UNIFORM3UIV,
		UNIFORM4F,
		UNIFORM4FV,
		UNIFORM4I,
		UNIFORM4IV,
		UNIFORM4UI,
		UNIFORM4UIV,
		UNIFORMMATRIX2FV,
		UNIFORMMATRIX2X3FV,
		UNIFORMMATRIX2X4FV,
		UNIFORMMATRIX3FV,
		UNIFORMMATRIX3X2FV,
		UNIFORMMATRIX3X4FV,
		UNIFORMMATRIX4FV,
		UNIFORMMATRIX4X2FV,
		UNIFORMMATRIX4X3FV
	};
private:
	Shader() = delete;
	Shader(const Shader& s) = delete;
	Shader(const Shader&& s) = delete;
	std::string vpath, fpath;
	std::vector<std::tuple<std::string, void*, uniformTypes>> uniforms;
	// The program ID //SPO ID
	GLuint Program;
public:
	//Returns SPO Id
	GLuint getShaderId() { return Program;  }

	Shader(const GLchar* vertexPath, const GLchar* fragmentPath) : vpath(vertexPath), fpath(fragmentPath), Program(0) { Reload(); }
	
	~Shader() {
		//Prevent external values from destructor calls
		for (auto v : uniforms)
			std::get<1>(v) = nullptr;
		glDeleteProgram(this->Program); 
	}

	//Push uniform value to uniform loader
	void pushUniform(const char *uniformName, void *value, uniformTypes type) {
		uniforms.push_back(std::make_tuple(std::string(uniformName), value, type));
	}

	//Push uniform value to uniform loader
	void pushUniform(std::string &uniformName, void *value, uniformTypes type) {
		uniforms.push_back(std::make_tuple(uniformName, value, type));
	}

	//Pop uniform value from uniform loader
	void popUniform() {
		if (!uniforms.empty()) {
			//Prevent external value from destructor call
			std::get<1>(uniforms.back()) = nullptr;
			uniforms.pop_back();
		}
	}

	//Set pointer to current value of value
	void setUniform(void *value, int index = 0) {
		try {
			std::get<1>(uniforms.at(index)) = value;
		}
		catch (std::length_error e) {
			#ifdef GEBUG_SHADER
				DEBUG_OUT << "ERROR::SHADER::setUniform::OUT_OF_RANGE\n\tCan't access uniform by index: "<< index << DEBUG_NEXT_LINE;
				DEBUG_OUT << "Error string: " << e.what() << DEBUG_NEXT_LINE;
			#endif
		}
		catch (...) {
			#ifdef GEBUG_SHADER
				DEBUG_OUT << "ERROR::SHADER::setUniform::UNKONWN"<< DEBUG_NEXT_LINE;
			#endif
				throw;
		}
	}

	void Use();

	void Reload();

};

//Bind shader program to OpenGL and update all uniforms
void Shader::Use() {
	glUseProgram(this->Program);
	GLint _location = 0;
	for (auto &_value : uniforms) {
		_location = glGetUniformLocation(Program, std::get<0>(_value).c_str());
		if (_location == -1) { //Check if uniform not found
			#ifdef GEBUG_SHADER
				DEBUG_OUT << "ERROR::SHADER::Use::UNIFORM_NAME_MISSING\n\tName: " << std::get<0>(_value) << DEBUG_NEXT_LINE;
				DEBUG_OUT << "\tUniform type: " << std::get<2>(_value) << DEBUG_NEXT_LINE;
			#endif
			continue;
		}
		switch (std::get<2>(_value)) {
		case UNIFORMMATRIX4FV:
			glUniformMatrix4fv(_location, 1, GL_FALSE, glm::value_ptr(*((glm::mat4 *)std::get<1>(_value))));
			break;
		default:
			break;
		}
	}
}

//Reload shader from disk (read, build and link)
void Shader::Reload() {
	if (Program)
		glDeleteProgram(Program);
	// 1. Retrieve the vertex/fragment source code from filePath
	std::string vertexCode;
	std::string fragmentCode;
	std::ifstream vShaderFile;
	std::ifstream fShaderFile;
	// ensures ifstream objects can throw exceptions:
	vShaderFile.exceptions(std::ifstream::badbit);
	fShaderFile.exceptions(std::ifstream::badbit);
	try {
		// Open files
		vShaderFile.open(vpath);
		fShaderFile.open(fpath);
		std::stringstream vShaderStream, fShaderStream;
		// Read file's buffer contents into streams
		vShaderStream << vShaderFile.rdbuf();
		fShaderStream << fShaderFile.rdbuf();
		// close file handlers
		vShaderFile.close();
		fShaderFile.close();
		// Convert stream into GLchar array
		vertexCode = vShaderStream.str();
		fragmentCode = fShaderStream.str();
	}
	catch (std::ifstream::failure e) {
		#ifdef GEBUG_SHADER
			DEBUG_OUT << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << DEBUG_NEXT_LINE;
		#endif
	}
	const GLchar* vShaderCode = vertexCode.c_str();
	const GLchar* fShaderCode = fragmentCode.c_str();

	// 2. Compile shaders
	GLuint vertex, fragment;
	GLint success;
	GLchar infoLog[512];
	// Vertex Shader
	vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &vShaderCode, NULL);
	glCompileShader(vertex);
	// Print compile errors if any
	glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		#ifdef GEBUG_SHADER
			glGetShaderInfoLog(vertex, 512, NULL, infoLog);
			DEBUG_OUT << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << DEBUG_NEXT_LINE;
		#endif
	};
	// Similiar for Fragment Shader
	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &fShaderCode, NULL);
	glCompileShader(fragment);
	glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		#ifdef GEBUG_SHADER
			glGetShaderInfoLog(fragment, 512, NULL, infoLog);
			DEBUG_OUT << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << DEBUG_NEXT_LINE;
		#endif
	}
	// Shader Program
	this->Program = glCreateProgram();
	glAttachShader(this->Program, vertex);
	glAttachShader(this->Program, fragment);
	glLinkProgram(this->Program);
	// Print linking errors if any
	glGetProgramiv(this->Program, GL_LINK_STATUS, &success);
	if (!success)
	{
		#ifdef GEBUG_SHADER
			glGetProgramInfoLog(this->Program, 512, NULL, infoLog);
			DEBUG_OUT << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << DEBUG_NEXT_LINE;
		#endif
	}
	// Delete the shaders as they're linked into our program now and no longer necessery
	glDeleteShader(vertex);
	glDeleteShader(fragment);
}
#endif
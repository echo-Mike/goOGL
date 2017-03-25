#ifndef SHADER_H
#define SHADER_H
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
public:
	//Type for uniform update function
	typedef void(*const UpdateFunction)(GLuint);
private:	
	//NO STDCONSTRUCT, NO MOVECONSTRUCT, NO COPYCONSTRUCT
	Shader() = delete;
	Shader(const Shader& s) = delete;
	Shader(const Shader&& s) = delete;
	//Vertex and Fragment shader paths
	std::string vpath, fpath;
	//Uniform container
	std::map<int, std::pair<std::string, UpdateFunction>> uniforms;
	// The program ID //SPO ID
	GLuint Program;
	//NOT SAFE SOLUTION: OVERFLOW
	//Id counter for current shader
	int incrementId = 0;
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

	Shader(const GLchar* vertexPath, const GLchar* fragmentPath) : vpath(vertexPath), fpath(fragmentPath), Program(0) { Reload(); }
	
	~Shader() {
		//Prevent external values from destructor calls
		//for (auto v : uniforms)
			//v.second = nullptr;
		glDeleteProgram(this->Program); 
	}

	int newUniform(const char* _uniformName, UpdateFunction _update) {
		uniforms[incrementId] = std::move(std::make_pair(std::string(_uniformName), _update));
		return incrementId++;
	}

	void deleteUniform(int _index) { uniforms.erase(_index); }

	/*
	//Push uniform value to uniform loader
	int pushUniform(const char *uniformName, void *value, uniformTypes type) {
		uniforms.push_back(std::make_tuple(std::string(uniformName), value, type));
		return uniforms.size()-1;
	}

	//Push uniform value to uniform loader
	int pushUniform(std::string &uniformName, void *value, uniformTypes type) {
		uniforms.push_back(std::make_tuple(uniformName, value, type));
		return uniforms.size()-1;
	}

	//Push uniform value to uniform loader
	int pushUniform(const char *uniformName, void *value, int type) {
		return pushUniform(uniformName, value, (uniformTypes)type);
	}

	//Push uniform value to uniform loader
	int pushUniform(std::string &uniformName, void *value, int type) {
		return pushUniform(uniformName, value, (uniformTypes)type);
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
	*/

	void Use();

	void Reload();

};

//Bind shader program to OpenGL and update all uniforms
void Shader::Use() {
	glUseProgram(this->Program);
	GLint _location = 0;
	for (auto &_value : uniforms) {
		_location = glGetUniformLocation(Program, _value.second.first.c_str());
		if (_location == -1) { //Check if uniform not found
			#ifdef DEBUG_SHADER
				DEBUG_OUT << "ERROR::SHADER::Use::UNIFORM_NAME_MISSING"<< DEBUG_NEXT_LINE;
				DEBUG_OUT << "\tName: " << _value.first << DEBUG_NEXT_LINE;
			#endif
			continue;
		}
		(*_value.second.second)(_location);
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
		#ifdef DEBUG_SHADER
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
	if (!success) {
		#ifdef DEBUG_SHADER
			glGetShaderInfoLog(vertex, 512, NULL, infoLog);
			DEBUG_OUT << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << DEBUG_NEXT_LINE;
		#endif
	};
	// Similiar for Fragment Shader
	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &fShaderCode, NULL);
	glCompileShader(fragment);
	glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
	if (!success) {
		#ifdef DEBUG_SHADER
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
	if (!success) {
		#ifdef DEBUG_SHADER
			glGetProgramInfoLog(this->Program, 512, NULL, infoLog);
			DEBUG_OUT << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << DEBUG_NEXT_LINE;
		#endif
	}
	// Delete the shaders as they're linked into our program now and no longer necessery
	glDeleteShader(vertex);
	glDeleteShader(fragment);
}
#endif
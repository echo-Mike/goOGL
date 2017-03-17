#ifndef SHADER_H
#define SHADER_H
//STD
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
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
	Shader() = delete;
	Shader(const Shader& s) = delete;
	Shader(const Shader&& s) = delete;
	std::string vpath, fpath;
public:
	// The program ID //SPO ID
	GLuint Program;
	Shader(const GLchar* vertexPath, const GLchar* fragmentPath) : vpath(vertexPath), fpath(fragmentPath), Program(0) { Reload(); }
	~Shader() { glDeleteProgram(this->Program); }
	void Use() const { glUseProgram(this->Program); } 
	void Reload();
};

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
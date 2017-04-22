#include "CShader.h"
//Bind shader program to OpenGL and update all uniforms
void Shader::Use() {
	glUseProgram(Program);
	GLint _location = 0;
	for (auto &_value : uniforms) {
		_location = glGetUniformLocation(Program, _value.first.c_str());
		if (_location == -1) { //Check if uniform not found
			#ifdef DEBUG_SHADERCPP
				DEBUG_OUT << "ERROR::SHADER::Use::UNIFORM_NAME_MISSING"<< DEBUG_NEXT_LINE;
				DEBUG_OUT << "\tName: " << _value.first << DEBUG_NEXT_LINE;
			#endif
			continue;
		}
		_value.second->bindUniform(_location);
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
		#ifdef DEBUG_SHADERCPP
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
		#ifdef DEBUG_SHADERCPP
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
		#ifdef DEBUG_SHADERCPP
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
		#ifdef DEBUG_SHADERCPP
			glGetProgramInfoLog(this->Program, 512, NULL, infoLog);
			DEBUG_OUT << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << DEBUG_NEXT_LINE;
		#endif
	}
	// Delete the shaders as they're linked into our program now and no longer necessery
	glDeleteShader(vertex);
	glDeleteShader(fragment);
}
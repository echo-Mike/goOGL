#ifndef UNIFORMS_H
#define UNIFORMS_H
//STD
#include <string>
//GLEW
#include <GL/glew.h>
//OUR
#include "CShader.h"
//DEBUG
#ifdef DEBUG_UNIFORMS
	#ifndef DEBUG_OUT
		#define DEBUG_OUT std::cout
	#endif
	#ifndef DEBUG_NEXT_LINE
		#define DEBUG_NEXT_LINE std::endl
	#endif
#endif

#ifndef UNIFORM_HANDLER_STD_SHADER_VARIABLE_NAME
	//In shader variable name of value
	#define UNIFORM_HANDLER_STD_SHADER_VARIABLE_NAME "value"
#endif

//Class template definition: UniformHandler
template <	class T, class TShader = Shader,
			int (TShader::* NewUniform)(const char*, void(*)(GLuint)) = &TShader::newUniform,
			void (TShader::* DeleteUniform)(int) = &TShader::deleteUniform>
class UniformHandler {
	int uniformId;
protected:
	T value;
	TShader *shader;
	std::string uniformName;
public:
	//Bind uniform to shader
	virtual void bindUniform(GLint _location) { return; }
protected:
	void Build() {
		if (shader) {
			uniformId = (shader->*NewUniform)(uniformName.c_str(), &(this->bindUniform));
		} else {
			#ifdef DEBUG_UNIFORMS
				DEBUG_OUT << "ERROR::UNIFORM_HANDLER::Build::SHADER_MISSING" << DEBUG_NEXT_LINE;
			#endif	
		}
	}
public:
	UniformHandler() : value(), uniformId(-1), shader(nullptr), uniformName(UNIFORM_HANDLER_STD_SHADER_VARIABLE_NAME) {}

	UniformHandler(	T _value, TShader *_shader = nullptr, 
					std::string _uniformName = std::string(UNIFORM_HANDLER_STD_SHADER_VARIABLE_NAME)) :
					value(std::move(_value)), shader(_shader), 
					uniformName(std::move(_uniformName)), uniformId(-1) 
	{
		Build();
	}

	UniformHandler(T _value, TShader *_shader, const char* _uniformName) : UniformHandler(_value, _shader, std::string(_uniformName)) { }

	~UniformHandler() {
		//Clear shader callback for this uniform
		(shader->*DeleteUniform)(uniformId);
		//Protect shader from destructor call
		shader = nullptr; 
	}

	//Set new value to handle
	void newValue(T _value) { value = std::move(_value); }
};

#ifndef UNIFORM_LOADER_STD_SHADER_VARIABLE_NAME
	//In shader variable name of value
	#define UNIFORM_LOADER_STD_SHADER_VARIABLE_NAME "value"
#endif

//Class template definition: UniformLoader
template< class T, class TShader = Shader>
class UniformLoader {
protected:
	T value;
	TShader *shader;
	std::string dataName;
public:
	//Bind data to shader
	virtual void bindData() { return; }

	UniformLoader() : value(), shader(nullptr), dataName(UNIFORM_LOADER_STD_SHADER_VARIABLE_NAME) {}

	UniformLoader(T _value, TShader *_shader = nullptr,
				std::string _dataName = std::string(UNIFORM_LOADER_STD_SHADER_VARIABLE_NAME)) :
				value(std::move(_value)), shader(_shader), 
				dataName(std::move(_dataName)) { }

	UniformLoader(T _value, TShader *_shader, const char* _uniformName) : UniformLoader(_value, _shader, std::string(_uniformName)) { }

	~DataHandler() { /*Protect shader from destructor call*/ shader = nullptr; }

	//Set new value to handle
	void newValue(T _value) { value = std::move(_value); }
};
#endif
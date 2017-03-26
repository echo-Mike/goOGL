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

	//Push to shader handle queue
	void push() {
		if (shader) {
			uniformId = (shader->*NewUniform)(uniformName.c_str(), &(this->bindUniform));
		} else {
			#ifdef DEBUG_UNIFORMS
				DEBUG_OUT << "ERROR::UNIFORM_HANDLER::push::SHADER_MISSING" << DEBUG_NEXT_LINE;
			#endif	
		}
	}

	//Pull from shader handle queue
	void pull() {
		if (shader) {
			(shader->*DeleteUniform)(uniformId);
		} else {
			#ifdef DEBUG_UNIFORMS
				DEBUG_OUT << "ERROR::UNIFORM_HANDLER::pull::SHADER_MISSING" << DEBUG_NEXT_LINE;
			#endif	
		}
	}

	UniformHandler() : value(), uniformId(-1), shader(nullptr), uniformName(UNIFORM_HANDLER_STD_SHADER_VARIABLE_NAME) {}

	UniformHandler(	T _value, TShader *_shader = nullptr, 
					std::string _uniformName = std::string(UNIFORM_HANDLER_STD_SHADER_VARIABLE_NAME)) :
					value(std::move(_value)), shader(_shader), 
					uniformName(std::move(_uniformName)), uniformId(-1) 
	{
		push();
	}

	UniformHandler(T _value, TShader *_shader, const char* _uniformName) : UniformHandler(_value, _shader, std::string(_uniformName)) { }

	~UniformHandler() {
		//Clear shader callback for this uniform
		(shader->*DeleteUniform)(uniformId);
		//Protect shader from destructor call
		shader = nullptr; 
	}

	//Set new value to handle
	virtual void newValue(T _value) { value = std::move(_value); }

	//Shader pointer setup
	virtual void setShader(TShader *_shader) { shader = _shader; }

	//Set new in-shader name of variable
	virtual void setName(const char* newName) { uniformName = std::string(newName); }

	//Set new in-shader name of variable
	virtual void setName(std::string newName) { uniformName = std::move(newName); }

	//Get in-shader name of variable
	virtual std::string& getName() { return uniformName.substr(); }

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

	UniformLoader(T _value, TShader *_shader, const char* _uniformName) : UniformLoader(_value, _shader, std::string(_uniformName)) {}

	~UniformLoader() { /*Protect shader from destructor call*/ shader = nullptr; }

	//Set new value to handle
	virtual void newValue(T _value) { value = std::move(_value); }

	//Shader pointer setup
	virtual void setShader(TShader *_shader) { shader = _shader; }

	//Set new in-shader name of variable
	virtual void setName(const char* newName) { dataName = std::string(newName); }

	//Set new in-shader name of variable
	virtual void setName(std::string newName) { dataName = std::move(newName); }

	//Get in-shader name of variable
	virtual std::string& getName() { return dataName.substr(); }
};
#endif
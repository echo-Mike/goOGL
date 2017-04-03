#ifndef UNIFORMS_H
#define UNIFORMS_H
//STD
#include <string>
//GLEW
#include <GL/glew.h>
//OUR
//#include "CShader.h"
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

class Shader;

class UniformHandlerInteface {
public:
	//Bind uniform to shader
	virtual void bindUniform(GLint _location) = 0;
};

//Class template definition: UniformHandler
template <	class T, class TShader = Shader,
			int (TShader::* NewUniform)(const char*, UniformHandlerInteface*) = &TShader::newUniform,
			void (TShader::* DeleteUniform)(int) = &TShader::deleteUniform>
class UniformHandler : public UniformHandlerInteface {
	int uniformId;
protected:
	T value;
	TShader *shader;
	std::string uniformName;
public:
	//Type of handled value
	typedef T ValueType;
	//Type of handeled shader
	typedef TShader ShaderType;

	//Push to shader uniform handle queue of current saved shader
	void push() {
		if (shader) {
			uniformId = (shader->*NewUniform)(uniformName.c_str(), this);
		} else {
			#ifdef DEBUG_UNIFORMS
				DEBUG_OUT << "ERROR::UNIFORM_HANDLER::push::SHADER_MISSING" << DEBUG_NEXT_LINE;
			#endif	
		}
	}

	/*Push to shader uniform handle queue of shader defined by pointer
	* Returns uniforId from _shader
	*/
	int push(TShader *_shader) {
		if (_shader) {
			return (_shader->*NewUniform)(uniformName.c_str(), this);
		} else {
			#ifdef DEBUG_UNIFORMS
				DEBUG_OUT << "ERROR::UNIFORM_HANDLER::push::SHADER_MISSING" << DEBUG_NEXT_LINE;
			#endif	
		}
	}

	//Pull from shader uniform handle queue of current saved shader
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

	UniformHandler(	T _value, TShader *_shader = nullptr,
					const char* _uniformName = UNIFORM_HANDLER_STD_SHADER_VARIABLE_NAME) : 
					value(std::move(_value)), shader(_shader),
					uniformName(_uniformName), uniformId(-1)
	{
		push();
	}

	UniformHandler(const UniformHandler &other) :	value(other.value),				shader(other.shader), 
													uniformName(other.uniformName),	uniformId(-1) 
	{
		push();
	}

	UniformHandler(UniformHandler &&other) :	value(std::move(other.value)), 
												shader(std::move(other.shader)),
												uniformName(std::move(other.uniformName)),
												uniformId(std::move(other.uniformId)) 
	{
		//Negate the resource acquisition in shader in other that will be deleted soon
		other.uniformId = -1;
	}

	~UniformHandler() {
		//Clear shader callback for this uniform
		if (uniformId >= 0)
			pull();
		//Protect shader from destructor call
		shader = nullptr; 
	}

	UniformHandler& operator=(UniformHandler other) {
		if (&other == this)
			return *this;
		std::swap(shader, other.shader);
		std::swap(uniformName, other.uniformName);
		value = other.value;
		uniformId = -1;
		std::swap(uniformId, other.uniformId);
		return *this;
	}

	UniformHandler& operator=(UniformHandler &&other) {
		shader = std::move(other.shader);
		uniformName = std::move(other.uniformName);
		value = std::move(other.value);
		uniformId = std::move(other.uniformId);
		other.uniformId = -1;
		return *this;
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
	virtual std::string getName() { return uniformName.substr(); }

};

#ifndef UNIFORM_LOADER_STD_SHADER_VARIABLE_NAME
	//In shader variable name of value
	#define UNIFORM_LOADER_STD_SHADER_VARIABLE_NAME "value"
#endif

class UniformLoaderInteface {
public:
	//Bind uniform to shader
	virtual void bindData() = 0;
};

//Class template definition: UniformLoader
template< class T, class TShader = Shader>
class UniformLoader : public UniformLoaderInteface {
protected:
	T value;
	TShader *shader;
	std::string dataName;
public:
	//Type of handled value
	typedef T ValueType;
	//Type of handeled shader
	typedef TShader ShaderType;

	UniformLoader() : value(), shader(nullptr), dataName(UNIFORM_LOADER_STD_SHADER_VARIABLE_NAME) {}

	UniformLoader(	T _value, TShader *_shader = nullptr,
					std::string _dataName = std::string(UNIFORM_LOADER_STD_SHADER_VARIABLE_NAME)) :
					value(std::move(_value)), shader(_shader), 
					dataName(std::move(_dataName)) {}

	UniformLoader(	T _value, TShader *_shader = nullptr, 
					const char* _dataName = UNIFORM_LOADER_STD_SHADER_VARIABLE_NAME) :
					value(std::move(_value)), shader(_shader),
					dataName(_dataName) {}
	
	UniformLoader(const UniformLoader &other) : value(other.value),
												shader(other.shader), 
												uniformName(other.uniformName) {}

	UniformLoader(UniformLoader &&other) :	value(std::move(other.value)),
											shader(std::move(other.shader)),
											uniformName(std::move(other.uniformName)) {}

	~UniformLoader() { /*Protect shader from destructor call*/ shader = nullptr; }
	
	UniformLoader& operator=(UniformLoader other) {
		if (&other == this)
			return *this;
		std::swap(shader, other.shader);
		std::swap(uniformName, other.uniformName);
		value = other.value;
		return *this;
	}

	UniformLoader& operator=(UniformLoader &&other) {
		shader = std::move(other.shader);
		uniformName = std::move(other.uniformName);
		value = std::move(other.value);
		return *this;
	}

	//Set new value to handle
	virtual void newValue(T _value) { value = std::move(_value); }

	//Shader pointer setup
	virtual void setShader(TShader *_shader) { shader = _shader; }

	//Set new in-shader name of variable
	virtual void setName(const char* newName) { dataName = std::string(newName); }

	//Set new in-shader name of variable
	virtual void setName(std::string newName) { dataName = std::move(newName); }

	//Get in-shader name of variable
	virtual std::string getName() { return dataName.substr(); }
};
#endif
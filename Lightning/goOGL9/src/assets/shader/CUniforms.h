#ifndef UNIFORMS_H
#define UNIFORMS_H "[0.0.1@CUniforms.h]"
/*
*	DESCRIPTION:
*		Module contains implementation of in-shader uniform value handling classes and templates.
*	AUTHOR:
*		Mikhail Demchenko
*		mailto:dev.echo.mike@gmail.com
*		https://github.com/echo-Mike
*/
//STD
#include <string>
//GLEW
#include <GL/glew.h>
//DEBUG
#ifdef DEBUG_UNIFORMS
	#ifndef DEBUG_OUT
		#define DEBUG_OUT std::cout
	#endif
	#ifndef DEBUG_NEXT_LINE
		#define DEBUG_NEXT_LINE std::endl
	#endif
#endif

//Forwar declaration of shader calss used in project
class Shader;

#ifndef UNIFORM_STD_SHADER_VARIABLE_NAME
	//In shader variable name of value
	#define UNIFORM_STD_SHADER_VARIABLE_NAME "UniformValue"
#endif

/* Common storage class for future UniformHandler and UniformLoader
*  Class template definition: UniformValue
*/
template < class T, class TShader = Shader >
class UniformValue {
protected:
	T value;
	TShader *shader;
	std::string uniformName;
public:
	//Type of handled value
	typedef T ValueType;
	//Type of pointer to handled value
	typedef T* ValueTypePtr;
	//Type of handeled shader
	typedef TShader ShaderType;

	UniformValue() : value(), shader(nullptr), uniformName(UNIFORM_STD_SHADER_VARIABLE_NAME) {}

	UniformValue(	T &_value, TShader *_shader = nullptr,
					std::string _uniformName = std::string(UNIFORM_STD_SHADER_VARIABLE_NAME)) :
					value(_value), shader(_shader), 
					uniformName(std::move(_uniformName)) {}

	UniformValue(	T &_value, TShader *_shader = nullptr,
					const char* _uniformName = UNIFORM_STD_SHADER_VARIABLE_NAME) : 
					value(_value), shader(_shader),
					uniformName(_uniformName) {}

	UniformValue(const UniformValue &other) :	value(other.value), 
												shader(other.shader),
												uniformName(other.uniformName) {}

	UniformValue(UniformValue &&other) :	value(std::move(other.value)),
											shader(std::move(other.shader)),
											uniformName(std::move(other.uniformName)) {}

	~UniformValue() { /*Protect shader from destructor call*/ shader = nullptr; }

	//Copy-and_swap idiom copy-assign operator
	UniformValue& operator=(UniformValue other) {
		if (&other == this)
			return *this;
		std::swap(shader, other.shader);
		std::swap(uniformName, other.uniformName);
		//Trigger copy-assign of value type
		value = other.value;
		return *this;
	}

	UniformValue& operator=(UniformValue &&other) {
		shader = nullptr;
		shader = std::move(other.shader);
		uniformName = std::move(other.uniformName);
		//Trigger move-assign of value type
		value = std::move(other.value);
		return *this;
	}

	//Get copy of current healed value
	virtual T getValue() { return value; }

	//Set new value to handle
	virtual void setValue(T _value) { value = std::move(_value); }

	//Shader pointer setup
	virtual void setShader(TShader *_shader) { shader = _shader; }

	//Set new in-shader name of variable
	virtual void setName(const char* newName) { uniformName = std::move(std::string(newName)); }

	//Set new in-shader name of variable
	virtual void setName(std::string newName) { uniformName = std::move(newName); }

	//Get in-shader name of variable
	virtual std::string getName() { return uniformName.substr(); }
};

/* Common interface for all Uniform handlers.
*  Class abstract definition: UniformHandlerInteface
*/
class UniformHandlerInteface {
public:
	//Bind uniform to shader
	virtual void bindUniform(GLint _location) = 0;
};

/* The automatic storage for uniform in-shader value.
*  Class template definition: UniformHandler
*/
template <	class T, class TShader = Shader,
			int (TShader::* NewUniform)(const char*, UniformHandlerInteface*) = &TShader::newUniform,
			void (TShader::* DeleteUniform)(int) = &TShader::deleteUniform>
class UniformHandler : public UniformHandlerInteface, public UniformValue<T, TShader> {
	int uniformId;
public:
	//Push to shader uniform handle queue of current saved shader
	void push() {
		if (uniformId >= 0) {
			#ifdef DEBUG_UNIFORMS
				DEBUG_OUT << "WARNING::UNIFORM_HANDLER::push::ALREADY_BINDED" << DEBUG_NEXT_LINE;
				DEBUG_OUT << "\tMessage: Uniform handler alredy helds binding with other shader." << DEBUG_NEXT_LINE;
				DEBUG_OUT << "\tLast Id: " << uniformId << DEBUG_NEXT_LINE;
			#endif	
		}
		if (shader) {
			uniformId = (shader->*NewUniform)(uniformName.c_str(), this);
		} else {
			#ifdef DEBUG_UNIFORMS
				DEBUG_OUT << "ERROR::UNIFORM_HANDLER::push::SHADER_MISSING" << DEBUG_NEXT_LINE;
			#endif	
		}
	}

	/*Push to uniform handle queue of shader defined by pointer
	* Returns uniforId from _shader
	*/
	int push(TShader *_shader) {
		if (_shader) {
			return (_shader->*NewUniform)(uniformName.c_str(), this);
		} else {
			#ifdef DEBUG_UNIFORMS
				DEBUG_OUT << "ERROR::UNIFORM_HANDLER::push::SHADER_MISSING" << DEBUG_NEXT_LINE;
			#endif
			return -1;
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

	UniformHandler() : UniformValue(), uniformId(-1) {}

	UniformHandler(	T &_value, TShader *_shader = nullptr, 
					std::string _uniformName = std::string(UNIFORM_STD_SHADER_VARIABLE_NAME)) :
					UniformValue(_value, _shader, _uniformName), uniformId(-1)
	{
		push();
	}
	
	UniformHandler(	T &_value, TShader *_shader = nullptr, 
					const char* _uniformName = UNIFORM_STD_SHADER_VARIABLE_NAME) :
					UniformValue(_value, _shader, _uniformName), uniformId(-1)
	{
		push();
	}

	UniformHandler(const UniformHandler &other) : UniformValue(other), uniformId(-1) 
	{
		push();
	}

	UniformHandler(UniformHandler &&other) :	UniformValue(other),
												uniformId(std::move(other.uniformId)) 
	{
		//Negate the shader resource acquisition in "other" that will be deleted soon
		other.uniformId = -1;
	}

	~UniformHandler() {
		//Clear shader callback for this uniform
		if (uniformId >= 0)
			pull();
	}

	UniformHandler& operator=(UniformHandler other) {
		if (&other == this)
			return *this;
		UniformValue::operator=(other);
		uniformId = -1;
		std::swap(uniformId, other.uniformId);
		return *this;
	}

	UniformHandler& operator=(UniformHandler &&other) {
		UniformValue::operator=(other);
		uniformId = std::move(other.uniformId);
		other.uniformId = -1;
		return *this;
	}
};

/* Common interface for all Uniform loaders.
*  Class abstract definition: UniformLoaderInteface
*/
class UniformLoaderInteface {
public:
	//Bind uniform to shader
	virtual void bindData() = 0;
};

/* The manual storage for uniform in-shader value.
*  Class template definition: UniformLoader
*/
template< class T, class TShader = Shader>
class UniformLoader : public UniformLoaderInteface, public UniformValue<T, TShader> {
public:
	UniformLoader() : UniformValue() {}

	UniformLoader(	T &_value, TShader *_shader = nullptr,
					std::string _dataName = std::string(UNIFORM_STD_SHADER_VARIABLE_NAME)) :
					UniformValue(_value, _shader, _dataName) {}

	UniformLoader(	T &_value, TShader *_shader = nullptr, 
					const char* _dataName = UNIFORM_STD_SHADER_VARIABLE_NAME) :
					UniformValue(_value, _shader, _dataName) {}
	
	UniformLoader(const UniformLoader &other) : UniformLoader(other) {}

	UniformLoader(UniformLoader &&other) : UniformLoader(other) {}

	~UniformLoader() {}
};
#endif
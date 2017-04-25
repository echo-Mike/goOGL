#ifndef UNIFORMS_H
#define UNIFORMS_H "[0.0.3@CUniforms.h]"
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

/* Base class for future UniformStorage and UniformObserver
*  Class template definition: UniformStorage
*/
template < class TShader = Shader >
class UniformBase {
protected:
	TShader *shader;
	std::string uniformName;
public:
	UniformBase() : shader(nullptr), uniformName(UNIFORM_STD_SHADER_VARIABLE_NAME) {}

	UniformBase(TShader *_shader,
				std::string _uniformName = std::string(UNIFORM_STD_SHADER_VARIABLE_NAME)) :
				shader(_shader), uniformName(std::move(_uniformName)) {}

	UniformBase(TShader *_shader,
				const char* _uniformName = UNIFORM_STD_SHADER_VARIABLE_NAME) : 
				shader(_shader), uniformName(_uniformName) {}

	UniformBase(const UniformBase &other) : shader(other.shader),
											uniformName(other.uniformName) {}

	UniformBase(UniformBase &&other) : 	shader(std::move(other.shader)),
										uniformName(std::move(other.uniformName)) {}

	~UniformBase() { /*Protect shader from destructor call*/ shader = nullptr; }

	//Copy-and_swap idiom copy-assign operator
	UniformBase& operator=(UniformBase other) {
		if (&other == this)
			return *this;
		std::swap(shader, other.shader);
		std::swap(uniformName, other.uniformName);
		return *this;
	}

	UniformBase& operator=(UniformBase &&other) {
		shader = nullptr;
		shader = std::move(other.shader);
		uniformName = std::move(other.uniformName);
		return *this;
	}

	//Shader pointer setup
	virtual void setShader(TShader *_shader) { shader = _shader; }

	//Set new in-shader name of variable
	virtual void setName(const char* newName) { uniformName = std::move(std::string(newName)); }

	//Set new in-shader name of variable
	virtual void setName(std::string newName) { uniformName = std::move(newName); }

	//Get in-shader name of variable
	virtual std::string getName() { return uniformName.substr(); }
};

/* Common storage class for future UniformAutomaticStorage and UniformManualStorage
*  Class template definition: UniformStorage
*/
template < class T, class TShader = Shader >
class UniformStorage : public UniformBase<TShader> {
protected:
	T value;
public:
	//Type of handled value
	typedef T ValueType;
	//Type of pointer to handled value
	typedef T* ValueTypePtr;
	//Type of handeled shader
	typedef TShader ShaderType;

	UniformStorage() : UniformBase(), value() {}

	UniformStorage(	T *_value, TShader *_shader = nullptr,
					std::string _uniformName = std::string(UNIFORM_STD_SHADER_VARIABLE_NAME)) :
					UniformBase(_shader, _uniformName), value(*_value) {}

	UniformStorage(	T *_value, TShader *_shader = nullptr,
					const char* _uniformName = UNIFORM_STD_SHADER_VARIABLE_NAME) : 
					UniformBase(_shader, _uniformName), value(*_value) {}

	UniformStorage(const UniformStorage &other) : UniformBase(other), value(other.value) {}

	UniformStorage(UniformStorage &&other) : UniformBase(other), value(std::move(other.value)) {}

	~UniformStorage() {}

	//Copy-and_swap idiom copy-assign operator
	UniformStorage& operator=(UniformStorage other) {
		if (&other == this)
			return *this;
		UniformBase::operator=(other);
		//Trigger copy-assign of value type
		value = other.value;
		return *this;
	}

	UniformStorage& operator=(UniformStorage &&other) {
		UniformBase::operator=(other);
		//Trigger move-assign of value type
		value = std::move(other.value);
		return *this;
	}

	//Get copy of current healed value
	virtual T getValue() { return value; }

	//Set new value to handle
	virtual void setValue(T _value) { value = std::move(_value); }
};

/* Common observer class for future UniformAutomaticObserver and UniformManualObserver
*  Class template definition: UniformObserver
*/
template < class T, class TShader = Shader >
class UniformObserver : public UniformBase<TShader> {
protected:
	const T* valueptr;
public:
	//Type of handled value
	typedef T ValueType;
	//Type of pointer to handled value
	typedef T* ValueTypePtr;
	//Type of handeled shader
	typedef TShader ShaderType;

	UniformObserver() : UniformBase(), valueptr(nullptr) {}

	UniformObserver(T* _valueptr, TShader *_shader = nullptr,
					std::string _uniformName = std::string(UNIFORM_STD_SHADER_VARIABLE_NAME)) :
					UniformBase(_shader, _uniformName), valueptr(_valueptr) {}

	UniformObserver(T* _valueptr, TShader *_shader = nullptr,
					const char* _uniformName = UNIFORM_STD_SHADER_VARIABLE_NAME) :
					UniformBase(_shader, _uniformName), valueptr(_valueptr) {}

	UniformObserver(const UniformObserver &other) : UniformBase(other), valueptr(other.valueptr) {}

	UniformObserver(UniformObserver &&other) : UniformBase(other), valueptr(std::move(other.valueptr)) {}

	~UniformObserver() { /*Protect value from destructor call*/ valueptr = nullptr; }

	//Copy-and_swap idiom copy-assign operator
	UniformObserver& operator=(UniformObserver other) {
		if (&other == this)
			return *this;
		UniformBase::operator=(other);
		std::swap(valueptr, other.valueptr);
		return *this;
	}

	UniformObserver& operator=(UniformObserver &&other) {
		UniformBase::operator=(other);
		valueptr = nullptr;
		valueptr = std::move(other.valueptr);
		return *this;
	}

	//Get const ptr to currently observed value
	virtual const T* getValue() { return valueptr; }

	//Set new value to observe
	virtual void setValue(T* _valueptr) { valueptr = std::move(_valueptr); }
};

/* Common interface for all automatic Uniform handlers.
*  Class abstract definition: UniformAutomaticInteface
*/
class UniformAutomaticInteface {
public:
	//Bind uniform to shader
	virtual void bindUniform(GLint _location) = 0;
	//Push uniform to shader queue
	virtual void push() = 0;
	//Pull uniform from shader queue
	virtual void pull() = 0;
};

/* Common base class for all automatic Uniform handlers.
*  Class template definition: UniformAutomatic
*/
template <	template<class, class> class TBase, class T, class TShader = Shader,
			void (TShader::* NewUniform)(std::string&, UniformAutomaticInteface*) = &TShader::newUniform,
			void (TShader::* DeleteUniform)(std::string&) = &TShader::deleteUniform >
class UniformAutomatic : public UniformAutomaticInteface, public TBase<T, TShader> {
	typedef TBase<T, TShader> Base;
public:
	//Push to shader uniform handle queue of current saved shader
	virtual void push() {
		if (shader) {
			(shader->*NewUniform)(uniformName, this);
		} else {
			#ifdef DEBUG_UNIFORMS
				DEBUG_OUT << "ERROR::UNIFORM_AUTOMATIC::push::SHADER_MISSING" << DEBUG_NEXT_LINE;
			#endif	
		}
	}

	//Push to uniform handle queue of shader defined by pointer
	virtual void push(TShader *_shader) {
		if (_shader) {
			(_shader->*NewUniform)(uniformName, this);
		} else {
			#ifdef DEBUG_UNIFORMS
				DEBUG_OUT << "ERROR::UNIFORM_AUTOMATIC::push::SHADER_NOT_PROVIDED" << DEBUG_NEXT_LINE;
			#endif
		}
	}

	//Pull from shader uniform handle queue of current saved shader
	virtual void pull() {
		if (shader) {
			(shader->*DeleteUniform)(uniformName);
		} else {
			#ifdef DEBUG_UNIFORMS
				DEBUG_OUT << "ERROR::UNIFORM_AUTOMATIC::pull::SHADER_MISSING" << DEBUG_NEXT_LINE;
			#endif	
		}
	}

	UniformAutomatic() : Base() {}

	UniformAutomatic(	T *_value, TShader *_shader = nullptr,
						std::string _uniformName = std::string(UNIFORM_STD_SHADER_VARIABLE_NAME)) :
						Base(_value, _shader, _uniformName) { push(); }
	
	UniformAutomatic(	T *_value, TShader *_shader = nullptr,
						const char* _uniformName = UNIFORM_STD_SHADER_VARIABLE_NAME) :
						Base(_value, _shader, _uniformName) { push(); }

	UniformAutomatic(const UniformAutomatic &other) : Base(other) {	push();	}

	UniformAutomatic(UniformAutomatic &&other) : Base(other) {}

	~UniformAutomatic() { /*Clear shader callback for this uniform*/ pull(); }

	UniformAutomatic& operator=(UniformAutomatic other) {
		if (&other == this)
			return *this;
		Base::operator=(other);
		return *this;
	}

	UniformAutomatic& operator=(UniformAutomatic &&other) {
		Base::operator=(other);
		return *this;
	}

	//Shader pointer setup
	virtual void setShader(TShader* _shader) {
		if (shader)
			pull();
		Base::setShader(_shader);
		push();
	}

	//Set new in-shader name of variable
	virtual void setName(std::string _newName) {
		if (shader)
			pull();
		Base::setName(std::move(_newName));
		push();
	}
};

/* The automatic storage for uniform in-shader value.
*  Class template definition: UniformAutomaticStorage
*/
template < class T, class TShader = Shader >
using UniformAutomaticStorage = UniformAutomatic<UniformStorage, T, TShader>;

/* The automatic observer for uniform in-shader value.
*  Class template definition: UniformAutomaticObserver
*/
template < class T, class TShader = Shader >
using UniformAutomaticObserver = UniformAutomatic<UniformObserver, T, TShader>;

/* Common interface for all manual Uniform handlers.
*  Class abstract definition: UniformManualInteface
*/
class UniformManualInteface {
public:
	//Bind uniform to shader
	virtual void bindData() = 0;
};

/* Common base class for all manual Uniform handlers.
*  Class template definition: UniformManual
*/
template < template<class, class> class TBase, class T, class TShader = Shader >
class UniformManual : public UniformManualInteface, public TBase<T, TShader> {
	typedef TBase<T, TShader> Base;
public:
	UniformManual() : Base() {}

	UniformManual(	T *_value, TShader *_shader = nullptr,
					std::string _dataName = std::string(UNIFORM_STD_SHADER_VARIABLE_NAME)) :
					Base(_value, _shader, _dataName) {}

	UniformManual(	T *_value, TShader *_shader = nullptr, 
					const char* _dataName = UNIFORM_STD_SHADER_VARIABLE_NAME) :
					Base(_value, _shader, _dataName) {}
	
	UniformManual(const UniformManual &other) : Base(other) {}

	UniformManual(UniformManual &&other) : Base(other) {}

	~UniformManual() {}
};

/* The manual storage for uniform in-shader value.
*  Class template definition: UniformManualStorage
*/
template < class T, class TShader = Shader >
using UniformManualStorage = UniformManual<UniformStorage, T, TShader>;

/* The manual observer for uniform in-shader value.
*  Class template definition: UniformManualObserver
*/
template < class T, class TShader = Shader >
using UniformManualObserver = UniformManual<UniformObserver, T, TShader>;
#endif
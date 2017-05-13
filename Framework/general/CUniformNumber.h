#ifndef UNIFORMNUMBER_H
#define UNIFORMNUMBER_H "[0.0.4@CUniformNumber.h]"
/*
*	DESCRIPTION:
*		Module contains implementation of in-shader uniform number (float, double, etc.)
*		handling classes and templates based on uniform handling.
*	AUTHOR:
*		Mikhail Demchenko
*		mailto:dev.echo.mike@gmail.com
*		https://github.com/echo-Mike
*/
//STD
#include <string>
#include <type_traits>
//GLEW
#include <GL/glew.h>
//OUR
#include "assets/shader/CUniforms.h"
//DEBUG
#ifdef DEBUG_UNIFORMNUMBER
	#ifndef DEBUG_OUT
		#define DEBUG_OUT std::cout
	#endif
	#ifndef DEBUG_NEXT_LINE
		#define DEBUG_NEXT_LINE std::endl
	#endif
#endif

#ifndef NUMBER_STD_SHADER_VARIABLE_NAME
	//In shader variable name of matrix
	#define NUMBER_STD_SHADER_VARIABLE_NAME "number"
#endif

/* The automatic storage for uniform number in-shader value.
*  Class template definition: NumberManualStorage
*/
template <	class T = float, class TShader = Shader >
class NumberAutomaticStorage : public UniformAutomaticStorage<T, TShader> {
	typedef UniformAutomaticStorage<T, TShader> Base;
public:
	NumberAutomaticStorage() : Base(nullptr, nullptr, NUMBER_STD_SHADER_VARIABLE_NAME) {}

	NumberAutomaticStorage(	T* _value, TShader* _shader,
							const char* _name = NUMBER_STD_SHADER_VARIABLE_NAME) :
							Base(_value, _shader, _name) {}

	NumberAutomaticStorage(	T* _value, TShader* _shader,
							std::string _name = std::string(NUMBER_STD_SHADER_VARIABLE_NAME)) :
							Base(_value, _shader, _name) {}

	NumberAutomaticStorage(const NumberAutomaticStorage& other) : Base(other) {}

	NumberAutomaticStorage(NumberAutomaticStorage&& other) : Base(std::move(other)) {}

	//Bind number to shader
	void bindUniform(GLint _location) {
		if (std::is_same<T, int>::value) {
			glUniform1i(_location, value);
		} else 	if (std::is_same<T, unsigned int>::value) {
			glUniform1ui(_location, value);
		} else  if (std::is_floating_point<T>::value)
			glUniform1f(_location, value);
	}
};

/* The manual storage for uniform number in-shader value.
*  Class template definition: NumberManualStorage
*/
template <	class T = float, class TShader = Shader, 
			GLint(TShader::* _getUniformLocation)(const char*) = &TShader::getUniformLocation >
class NumberManualStorage : public UniformManualStorage<T, TShader> {
	typedef UniformManualStorage<T, TShader> Base;
public:
	NumberManualStorage() : Base(nullptr, nullptr, NUMBER_STD_SHADER_VARIABLE_NAME) {}

	NumberManualStorage(T* _value, TShader* _shader,
						const char* _name = NUMBER_STD_SHADER_VARIABLE_NAME) :
						Base(_value, _shader, _name) {}

	NumberManualStorage(T* _value, TShader* _shader,
						std::string _name = std::string(NUMBER_STD_SHADER_VARIABLE_NAME)) :
						Base(_value, _shader, _name) {}

	NumberManualStorage(const NumberManualStorage& other) : Base(other) {}

	NumberManualStorage(NumberManualStorage&& other) : Base(std::move(other)) {}

	void bindData() {
		GLint _location = (shader->*_getUniformLocation)(uniformName.c_str());
		if (_location == -1) { //Check if uniform not found
			#ifdef DEBUG_UNIFORMNUMBER
				DEBUG_OUT << "ERROR::NUMBER_MANUAL_STORAGE::bindData::UNIFORM_NAME_MISSING" << DEBUG_NEXT_LINE;
				DEBUG_OUT << "\tName: " << uniformName << DEBUG_NEXT_LINE;
			#endif
			return;
		}

	#ifdef FWCPP17
		if constexpr (std::is_same_v<T, int>) {
			glUniform1i(_location, value);
		} else 	if constexpr (std::is_same_v<T, unsigned int>) {
			glUniform1ui(_location, value);
		} else  if constexpr (std::is_floating_point_v<T>) {
			glUniform1f(_location, value);
		}
	#else
		if (std::is_same<T, int>::value) {
			glUniform1i(_location, value);
		} else 	if (std::is_same<T, unsigned int>::value) {
			glUniform1ui(_location, value);
		} else  if (std::is_floating_point<T>::value) {
			glUniform1f(_location, value);
		}
	#endif
	}
};
#endif
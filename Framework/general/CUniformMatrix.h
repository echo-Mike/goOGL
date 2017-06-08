#ifndef UNIFORMMATRIX_H
#define UNIFORMMATRIX_H "[0.0.5@CUniformMatrix.h]"
/*
*	DESCRIPTION:
*		Module contains implementation of in-shader uniform matrix4f handling classes and templates based on uniform handling.
*	AUTHOR:
*		Mikhail Demchenko
*		mailto:dev.echo.mike@gmail.com
*		https://github.com/echo-Mike
*/
//STD
#include <string>
//GLEW
#include <GL/glew.h>
//OUR
#include "assets/shader/CUniforms.h"
#include "ImprovedMath.h"
//DEBUG
#ifdef DEBUG_UNIFORMMATRIX
	#ifndef DEBUG_OUT
		#define DEBUG_OUT std::cout
	#endif
	#ifndef DEBUG_NEXT_LINE
		#define DEBUG_NEXT_LINE std::endl
	#endif
#endif

#ifndef MATRIX_STD_SHADER_VARIABLE_NAME
	//In shader variable name of matrix
	#define MATRIX_STD_SHADER_VARIABLE_NAME "matrix"
#endif

/* The automatic storage for uniform matrix4f in-shader value.
*  Class template definition: MatrixAutomaticStorage
*/
template <	class TMatrix = our::mat4, class TShader = Shader,
			GLfloat* (TMatrix::* _getValuePtr)(void) = &TMatrix::getValuePtr>
class MatrixAutomaticStorage : public UniformAutomaticStorage<TMatrix, TShader> {
	typedef UniformAutomaticStorage<TMatrix, TShader> Base;
public:
	//NOT SAFE SOLUTION: Direct member access
	//Control over transpose operation on matrix loading
	GLuint transposeOnLoad = GL_FALSE;

	MatrixAutomaticStorage() : Base(&(TMatrix()), nullptr, MATRIX_STD_SHADER_VARIABLE_NAME) {}

	MatrixAutomaticStorage(	TMatrix* _matrix, TShader* _shader,
							const char* _name = MATRIX_STD_SHADER_VARIABLE_NAME) :
							Base(_matrix, _shader, _name) {}

	MatrixAutomaticStorage(	TMatrix* _matrix, TShader* _shader,
							std::string _name = std::string(MATRIX_STD_SHADER_VARIABLE_NAME)) :
							Base(_matrix, _shader, _name) {}
	
	MatrixAutomaticStorage(const MatrixAutomaticStorage& other) : Base(other) {}

	MatrixAutomaticStorage(MatrixAutomaticStorage&& other) : Base(std::move(other)) {}

	//Bind matrix to shader
	void bindUniform(GLint _location) {
		glUniformMatrix4fv(_location, 1, transposeOnLoad, (value.*_getValuePtr)());
	}
};

/* The manual storage for uniform matrix4f in-shader value.
*  Class template definition: MatrixManualStorage
*/
template <	class TMatrix = our::mat4, class TShader = Shader,
			GLfloat* (TMatrix::* _getValuePtr)(void) = &TMatrix::getValuePtr,
			GLint (TShader::* _getUniformLocation)(const char*) = &TShader::getUniformLocation>
class MatrixManualStorage : public UniformManualStorage<TMatrix, TShader> {
	typedef UniformManualStorage<TMatrix, TShader> Base;
public:
	//NOT SAFE SOLUTION: Direct member access
	//Control over transpose operation on matrix loading
	GLuint transposeOnLoad = GL_FALSE;

	MatrixManualStorage() : Base(&(TMatrix()), nullptr, MATRIX_STD_SHADER_VARIABLE_NAME) {}

	MatrixManualStorage(TMatrix* _matrix, TShader* _shader,
						const char* _name = MATRIX_STD_SHADER_VARIABLE_NAME) :
						Base(_matrix, _shader, _name) {}

	MatrixManualStorage(TMatrix* _matrix, TShader* _shader,
						std::string _name = std::string(MATRIX_STD_SHADER_VARIABLE_NAME)) :
						Base(_matrix, _shader, _name) {}

	MatrixManualStorage(const MatrixManualStorage& other) : Base(other) {}

	MatrixManualStorage(MatrixManualStorage&& other) : Base(std::move(other)) {}
	
	//Bind matrix to shader
	void bindData() {
		GLint _location = (shader->*_getUniformLocation)(uniformName.c_str());
		if (_location == -1) { //Check if uniform not found
			#ifdef DEBUG_UNIFORMMATRIX
				DEBUG_OUT << "ERROR::MATRIX_MANUAL_STORAGE::bindData::UNIFORM_NAME_MISSING" << DEBUG_NEXT_LINE;
				DEBUG_OUT << "\tName: " << uniformName << DEBUG_NEXT_LINE;
			#endif
			return;
		}
		glUniformMatrix4fv(_location, 1, transposeOnLoad, (value.*_getValuePtr)());
	}
};
#endif
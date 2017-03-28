#ifndef UNIFORMMATRIX_H
#define UNIFORMMATRIX_H
//STD
#include <string>
//GLEW
#include <GL/glew.h>
//OUR
#include "CUniforms.h"
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

#ifndef MATRIX_HANDLER_STD_SHADER_VARIABLE_NAME
	//In shader variable name of matrix
	#define MATRIX_HANDLER_STD_SHADER_VARIABLE_NAME "matrix"
#endif

//Class template definition: MatrixHandler
template <	class TMatrix = our::mat4, class TShader = Shader,
			GLfloat* (TMatrix::* _getValuePtr)(void) = &TMatrix::getValuePtr>
class MatrixHandler : public UniformHandler<TMatrix, TShader> {
public:
	//NOT SAFE SOLUTION: Direct member access
	//Control over transpose operation on matrix loading
	GLuint transposeOnLoad = GL_FALSE;

	MatrixHandler() : UniformHandler(glm::mat4(), nullptr, MATRIX_HANDLER_STD_SHADER_VARIABLE_NAME) {}

	MatrixHandler(	TMatrix _matrix, TShader* _shader,
					const char* _name = MATRIX_HANDLER_STD_SHADER_VARIABLE_NAME) :
					UniformHandler(_matrix, _shader, _name) {}

	MatrixHandler(	TMatrix _matrix, TShader* _shader,
					std::string _name = std::string(MATRIX_HANDLER_STD_SHADER_VARIABLE_NAME)) :
					UniformHandler(_matrix, _shader, _name) {}
	
	//Bind matrix to shader
	void bindUniform(GLuint _location) const {
		glUniformMatrix4fv(_location, 1, transposeOnLoad, (value.*_getValuePtr)());
	}
};

#ifndef MATRIX_LOADER_STD_SHADER_VARIABLE_NAME
	//In shader variable name of matrix
	#define MATRIX_LOADER_STD_SHADER_VARIABLE_NAME "matrix"
#endif

//Class template definition: MatrixLoader
template <	class TMatrix = our::mat4, class TShader = Shader,
			GLfloat* (TMatrix::* _getValuePtr)(void) = &TMatrix::getValuePtr,
			GLint (TShader::* _getUniformLocation)(const char*) = &TShader::getUniformLocation>
class MatrixLoader : public UniformLoader<TMatrix, TShader> {
public:
	//NOT SAFE SOLUTION: Direct member access
	//Control over transpose operation on matrix loading
	GLuint transposeOnLoad = GL_FALSE;

	MatrixLoader() : UniformLoader(TMatrix(), nullptr, MATRIX_LOADER_STD_SHADER_VARIABLE_NAME) {}

	MatrixLoader(	TMatrix _matrix, TShader* _shader,
					const char* _name = MATRIX_LOADER_STD_SHADER_VARIABLE_NAME) :
					UniformLoader(_matrix, _shader, _name) {}

	MatrixLoader(	TMatrix _matrix, TShader* _shader,
					std::string _name = std::string(MATRIX_LOADER_STD_SHADER_VARIABLE_NAME)) :
					UniformLoader(_matrix, _shader, _name) {}
	
	//Bind matrix to shader
	void bindData() {
		GLint _location = (shader->*_getUniformLocation)(dataName.c_str());
		if (_location == -1) { //Check if uniform not found
			#ifdef DEBUG_UNIFORMMATRIX
				DEBUG_OUT << "ERROR::MATRIX_LOADER::bindData::UNIFORM_NAME_MISSING" << DEBUG_NEXT_LINE;
				DEBUG_OUT << "\tName: " << dataName.c_str() << DEBUG_NEXT_LINE;
			#endif
			return;
		}
		glUniformMatrix4fv(_location, 1, transposeOnLoad, (value.*_getValuePtr)());
	}
};
#endif
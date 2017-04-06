//STD
#include <iostream>
#include <string>
#include <array>

//GLEW
//satic linking for GLEW lib
#define GLEW_STATIC
#include <GL/glew.h>

//GLFW
#include <GLFW/glfw3.h>

//SOIL
#include <SOIL/SOIL.h>

//GLM
#include <GLM/glm.hpp>
#include <GLM/GTC/matrix_transform.hpp>
#include <GLM/GTC/type_ptr.hpp>

//OUR
#include "debug.h"
#include "assets/shader/CUniforms.h"
#include "general/ImprovedMath.h"
#include "general/CUniformMatrix.h"
#include "assets/textures/CUniformTexture.h"
#include "assets/shader/CShader.h"
#include "assets/model/CSimpleModel.h"
#include "assets/model/CSeparateModel.h"
#include "assets/model/CCombinedModel.h"
#include "assets/textures/Ctexture.h"
#include "scene/CCamera.h"
#include "data.h"

using std::cout;
using std::endl;

//Key callback for GLFW
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
//Mouse callback for GLFW
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
//Scroll callback for GLFW
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

//Declaration of InstanceData class
class InstanceData : public InstanceDataInterface {
public:
	MatrixLoader<> modelMatrix;

	InstanceData() : modelMatrix() {}

	InstanceData(our::mat4 _matrix, Shader *_shader, const char* _name) : modelMatrix(_matrix, _shader, _name) {}

	void bindData() { modelMatrix.bindData(); }

	void operator() (our::mat4 _matrix, Shader *_shader, const char* _name) {
		modelMatrix.setValue(_matrix);
		modelMatrix.setName(_name);
		modelMatrix.setShader(_shader);
	}

	void operator() (our::mat4 _matrix) { modelMatrix.setValue(_matrix); }
};

#ifndef VEC3_HANDLER_STD_SHADER_VARIABLE_NAME
	//In shader variable name of matrix
#define VEC3_HANDLER_STD_SHADER_VARIABLE_NAME "vector3"
#endif

template <	class TVector = glm::vec3, class TShader = Shader >
class Vec3Handler : public UniformHandler<TVector, TShader> {
public:
	Vec3Handler() : UniformHandler(TVector(), nullptr, VEC3_HANDLER_STD_SHADER_VARIABLE_NAME) {}

	Vec3Handler(TVector _vector, TShader* _shader,
				const char* _name = VEC3_HANDLER_STD_SHADER_VARIABLE_NAME) :
				UniformHandler(_vector, _shader, _name) {}

	Vec3Handler(TVector _vector, TShader* _shader,
				std::string _name = std::string(VEC3_HANDLER_STD_SHADER_VARIABLE_NAME)) :
				UniformHandler(_vector, _shader, _name) {}
	
	//Bind vec3 to shader
	void bindUniform(GLint _location) {
		glUniform3f(_location, value.x, value.y, value.z);
	}
};

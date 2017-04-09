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
#include "assets/shader/CUniformStruct.h"
#include "general/ImprovedMath.h"
#include "general/CUniformMatrix.h"
#include "general/CUniformVec3.h"
#include "general/CUniformNumber.h"
#include "general/CUniformStructCollection.h"
#include "assets/textures/CUniformTexture.h"
#include "assets/shader/CShader.h"
#include "assets/model/CSimpleModel.h"
#include "assets/model/CSeparateModel.h"
#include "assets/model/CCombinedModel.h"
#include "assets/textures/Ctexture.h"
#include "scene/CCamera.h"
#include "data.h"
#include "assets/material/HMaterialCollection.h"

using std::cout;
using std::endl;

//Key callback for GLFW
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
//Mouse callback for GLFW
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
//Scroll callback for GLFW
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

struct CommonInstance : public InstanceDataInterface {
	MatrixManualStorage<> modelMatrix;
	
	CommonInstance() : modelMatrix() {}

	CommonInstance(	our::mat4 _matrix, Shader *_shader, const char* _name) :
					modelMatrix(&_matrix, _shader, _name) {}

	void bindData() { modelMatrix.bindData(); }

	void operator() (our::mat4 _matrix, Shader *_shader, const char* _name) {
		modelMatrix.setValue(_matrix);
		modelMatrix.setName(_name);
		modelMatrix.setShader(_shader);
	}

	void operator() (our::mat4 _matrix) { modelMatrix.setValue(_matrix); }

	void operator() (Shader* _shader) { modelMatrix.setShader(_shader); }

	glm::vec3 getPosition() {
		return glm::vec3((glm::mat4)modelMatrix.getValue() * glm::vec4());
	}
};

//Declaration of InstanceData class
struct InstanceData : public CommonInstance {
	MaterialManualStorage material;

	InstanceData() : CommonInstance(), material() {}

	InstanceData(	our::mat4 _matrix, MaterialPOD _material,  Shader *_shader, 
					const char* _matName, const char* _structName) : 
					CommonInstance(_matrix, _shader, _matName),
					material(_material, _shader, _structName) {}

	void bindData() { 
		modelMatrix.bindData(); 
		material.bindData();
	}

	void operator() (	our::mat4 _matrix, MaterialPOD _material, Shader *_shader, 
						const char* _matName, const char* _structName) 
	{
		CommonInstance::operator()(_matrix, _shader, _matName);
		material(_material, _shader, _structName);
	}

	void operator() (MaterialPOD _material) { material(_material); }

	void operator() (Shader* _shader) { 
		material(_shader);
		CommonInstance::operator()(_shader);
	}
};

struct LightsourceData : public CommonInstance {
	LightsourceAutomaticStorage<> lightsource;

	LightsourceData() : CommonInstance(), lightsource() {}

	LightsourceData(our::mat4 _matrix, LightsourcePOD _light, Shader *_shader,
					const char* _matName, const char* _structName) : 
					CommonInstance(_matrix, _shader, _matName),
					lightsource(_light, _shader, _structName) {}

	void bindData() { 
		modelMatrix.bindData();
	}

	void operator() (	our::mat4 _matrix, LightsourcePOD _light, Shader *_shader, 
						const char* _matName, const char* _structName) 
	{
		CommonInstance::operator()(_matrix, _shader, _matName);
		lightsource(_light, _shader, _structName);
	}

	void operator() (LightsourcePOD _light) { lightsource(_light); }

	void operator() (Shader* _shader) { 
		lightsource(_shader);
		CommonInstance::operator()(_shader);
	}

	void push(Shader* _shader) {
		LightsourceAutomaticStorage<>::MemberInterface* _buff;
		for (auto &v : lightsource.data) {
			_buff = v;
			dynamic_cast<LightsourceAutomaticStorage<>::Member*>(_buff)->push(_shader);
		}
	}
};
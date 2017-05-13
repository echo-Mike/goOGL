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

#define GLM_ENABLE_EXPERIMENTAL
//GLM
#include <GLM/glm.hpp>
#include <GLM/GTC/matrix_transform.hpp>
#include <GLM/GTX/rotate_vector.hpp>
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
#include "general/CIndexPool.h"
#include "assets/textures/CUniformTexture.h"
#include "assets/shader/CShader.h"
#include "assets/model/CSimpleModel.h"
#include "assets/model/CSeparateModel.h"
#include "assets/model/CCombinedModel.h"
#include "assets/textures/Ctexture.h"
#include "scene/CCamera.h"
#include "data.h"
#include "assets/material/HMaterialCollection.h"
#include "assets\material\HDebugMaterial.h"
#include "assets\light\HDebugLight.h"

using std::cout;
using std::endl;

//Key callback for GLFW
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
//Mouse callback for GLFW
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
//Scroll callback for GLFW
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

struct CommonInstance : public InstanceDataInterface<Shader> {
	MatrixManualStorage<> modelMatrix;
	
	CommonInstance() : modelMatrix() {}

	CommonInstance(	our::mat4 _matrix, Shader *_shader, const char* _name) :
					modelMatrix(&_matrix, _shader, _name) {}

	CommonInstance(const CommonInstance& other) : modelMatrix(other.modelMatrix) {}

	virtual void bindData() { modelMatrix.bindData(); }

	virtual void setShader(Shader *_shader) { modelMatrix.setShader(_shader); }

	void operator() (our::mat4 _matrix, Shader *_shader, const char* _name) {
		modelMatrix.setValue(_matrix);
		modelMatrix.setName(_name);
		modelMatrix.setShader(_shader);
	}

	void operator() (our::mat4 _matrix) { modelMatrix.setValue(_matrix); }

	glm::vec3 getPosition() {
		return glm::vec3((glm::mat4)modelMatrix.getValue() * glm::vec4());
	}
};

struct InstanceData : public CommonInstance {
	TextureMaterialManualStorage<> material;

	InstanceData() : CommonInstance(), material() {}

	InstanceData(	our::mat4 _matrix, TextureMaterialPOD<> _material,  Shader *_shader, 
					const char* _matName, const char* _structName) : 
					CommonInstance(_matrix, _shader, _matName),
					material(_material, _shader, _structName) {}

	InstanceData(const InstanceData& other) : CommonInstance(other), material(other.material) {}

	void bindData() { 
		modelMatrix.bindData(); 
		material.bindData();
	}

	void setShader(Shader *_shader) { 
		material(_shader);
		modelMatrix.setShader(_shader); 
	}

	void operator() (	our::mat4 _matrix, TextureMaterialPOD<> _material, Shader *_shader, 
						const char* _matName, const char* _structName) 
	{
		CommonInstance::operator()(_matrix, _shader, _matName);
		material(_material, _shader, _structName);
	}

	void operator() (TextureMaterialPOD<> _material) { material(_material); }
};

struct PointLightInstance : public CommonInstance {
	Vec3ManualObserver<> lightColor;

	PointLightInstance() : CommonInstance(), lightColor() {}

	PointLightInstance(	our::mat4 _matrix, glm::vec3* _color, Shader *_shader,
						const char* _matName, const char* _colorName) :
						CommonInstance(_matrix, _shader, _matName),
						lightColor(_color, _shader, _colorName) {}

	PointLightInstance(const PointLightInstance& other) : CommonInstance(other), lightColor(other.lightColor) {}
	
	void setShader(Shader *_shader) {
		lightColor.setShader(_shader);
		modelMatrix.setShader(_shader);
	}

	void bindData() {
		modelMatrix.bindData();
		lightColor.bindData();
	}
};



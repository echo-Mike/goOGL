#ifndef IMPROVEDMATH_H
#define IMPROVEDMATH_H
//GLM
#include <GLM/glm.hpp>
#include <GLM/GTC/type_ptr.hpp>
//GLEW
#include <GL/glew.h>

//Namespace declaration: our
namespace our {
	//Class definition: Matrix4GLMHelper
	class Matrix4GLMHelper {
	public:
		//NOT SAFE SOLUTION
		glm::mat4 value;

		//Std constructor
		Matrix4GLMHelper() : value(glm::mat4()) {}

		//Copy constructor
		Matrix4GLMHelper(Matrix4GLMHelper& _helper) : value(_helper.value) {}

		//Move constructor
		Matrix4GLMHelper(Matrix4GLMHelper&& _helper) : value(std::move(_helper.value)) {}

		Matrix4GLMHelper(glm::mat4 _matrix) : value(std::move(_matrix)) {}

		//Get value pointer for OpenGL
		GLfloat* getValuePtr() {
			//Solution from GTC/type_ptr.inl
			return &(value[0].x);
		}
		//TODO: make all available operators for glm::mat4, then make value private
	};

	//Define a short type name
	typedef Matrix4GLMHelper mat4;
	
	//Our perspective matrix builder
	mat4 perspective(float fovy, float aspect, float near, float far) {
		return mat4(glm::perspective(fovy, aspect, near, far));
	}

	//Our otho matrix builder
	mat4 ortho(float left, float right, float bottom, float top, float near, float far) {
		return mat4(glm::ortho(left, right, bottom, top, near, far));
	}

	//Our lookAt matrix builder
	mat4 lookAt(glm::vec3 &eye, glm::vec3 &center, glm::vec3 &up) {
		return mat4(glm::lookAt(eye, center, up));
	}
}

#endif
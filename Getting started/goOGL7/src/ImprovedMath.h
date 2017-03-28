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

		operator glm::mat4() {
			return value;
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

	//Our rotate with our::mat4 as argument
	mat4 rotate(mat4 const &m, float angle, glm::vec3 const &axis) {
		return glm::rotate(m.value, angle, axis);
	}

	//Our rotate with glm::mat4 as argument
	mat4 rotate(glm::mat4 const &m, float angle, glm::vec3 const &axis) {
		return glm::rotate(m, angle, axis);
	}

	//Our translate with our::mat4 as argument
	mat4 translate(mat4 const &m, glm::vec3 const &v) {
		return glm::translate(m.value, v);
	}

	//Our translate with glm::mat4 as argument
	mat4 translate(glm::mat4 const &m, glm::vec3 const &v) {
		return glm::translate(m, v);
	}

	//Our scale with our::mat4 as argument
	mat4 scale(mat4 const &m, glm::vec3 const &v) {
		return glm::scale(m.value, v);
	}

	//Our scale with glm::mat4 as argument
	mat4 scale(glm::mat4 const &m, glm::vec3 const &v) {
		return glm::scale(m, v);
	}
}

#endif
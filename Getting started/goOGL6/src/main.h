//STD
#include <iostream>
#include <string>

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

//Our
#include "debug.h"
#include "CShader.h"
#include "Ctexture.h"
#include "CSimpleModel.h"
using std::cout;
using std::endl;

//Key callback for GLFW
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
//GLEW
//satic linking for GLEW lib
#define GLEW_STATIC
#include <GL/glew.h>

//GLFW
#include <GLFW/glfw3.h>
#include <iostream>
#include <string>

//SOIL
#include <SOIL/SOIL.h>

//GLM
#include <GLM/glm.hpp>
#include <GLM/GTC/matrix_transform.hpp>
#include <GLM/GTC/type_ptr.hpp>

//Our
//#define DEBUG_TEXTURE
#include "CShader.h"
#include "Ctexture.h"
using std::cout;
using std::endl;

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
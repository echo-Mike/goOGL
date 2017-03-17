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

//Our
#define DEBUG_TEXTURE
#include "CShader.h"
#include "Ctexture.h"
using std::cout;
using std::endl;

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
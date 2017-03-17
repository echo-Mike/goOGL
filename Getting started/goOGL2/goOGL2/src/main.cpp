//GLEW
//satic linking for GLEW lib
#define GLEW_STATIC
#include <GL/glew.h>
//GLFW
#include <GLFW/glfw3.h>
#include <iostream>
//Our
#include "CShader.h"
using std::cout;
using std::endl;



void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);

//before CShader.h include void create_spo(GLuint &spo);

int main()
{
	//»Õ»÷»¿À»«¿÷»ﬂ
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	GLFWwindow *window = glfwCreateWindow(800, 600, "LearnOpenGL", nullptr, nullptr);
	if (window == nullptr){
		cout << "Failed to create GLFW window" << endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetKeyCallback(window, key_callback);
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK)
	{
		cout << "Failed to initialize GLEW" << endl;
		return -2;
	}
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);

	//—Œ«ƒ¿®Ã ÿ≈…ƒ≈–€
	Shader shaderProgram("C:\\Users\\123\\Desktop\\OpenGL\\PROJECTS\\GOOPENGL\\goOGL2\\goOGL2\\src\\shaders\\shader.vs",
						 "C:\\Users\\123\\Desktop\\OpenGL\\PROJECTS\\GOOPENGL\\goOGL2\\goOGL2\\src\\shaders\\shader.fs");
	/*Uniform titorial + before CShader.h include
	create_spo(shaderProgram);
	GLint vertexColorLocation = glGetUniformLocation(shaderProgram, "ourColor");
	GLfloat timeValue, greenValue;
	*/


	//ƒ¿ÕÕ€≈
	GLfloat vertices[] = {
		// Positions         // Colors
		0.5f, -0.5f, 0.0f,	1.0f, 0.0f, 0.0f,   // Bottom Right
		-0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f,   // Bottom Left
		0.0f, 0.5f, 0.0f,	0.0f, 0.0f, 1.0f    // Top 
	};
	/*ƒÎˇ ÚÛÚÓË‡Î‡ Ò ÔˇÏÓÛ„ÓÎ¸ÌÓÍÓÏ
	GLfloat vertices[] = {
		0.5f, 0.5f, 0.0f,  // Top Right
		0.5f, -0.5f, 0.0f,  // Bottom Right
		-0.5f, -0.5f, 0.0f,  // Bottom Left
		-0.5f, 0.5f, 0.0f   // Top Left 
	};
	GLuint indices[] = {  // Note that we start from 0!
		0, 1, 3,   // First Triangle
		1, 2, 3    // Second Triangle
	};
	*/

	//OGL Œ¡⁄≈ “€
	GLuint VBO;
	glGenBuffers(1, &VBO);

	//EBO Œ¡⁄≈ “ - ˝ÍÓÌÓÏËÚ ÏÂÒÚÓ ‰Îˇ ‚Â¯ËÌ
	GLuint EBO;
	glGenBuffers(1, &EBO);

	GLuint VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	{
		// 2. Copy our vertices array in a vertex buffer for OpenGL to use
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		/*ƒÎˇ ÚÛÚÓË‡Î‡ Ò ÔˇÏÓÛ„ÓÎ¸ÌÓÍÓÏ
		// 3. Copy our index array in a element buffer for OpenGL to use
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
		*/

		// 3. Then set the vertex attributes pointers
		//Position attribute
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(0);
		// Color attribute
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
		glEnableVertexAttribArray(1);
	}
	glBindVertexArray(0);
	
	//main loop
	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		//‰Îˇ wireframe
		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		/*Unifor tutorial
		//Shader Unifor value setup
		timeValue = glfwGetTime();
		greenValue = (sin(timeValue) / 2) + 0.5;
		glUseProgram(shaderProgram);
		//Load data to ourColor
		//glUniform4f(vertexColorLocation, 0.0f, greenValue, 0.0f, 1.0f);
		*/
		shaderProgram.Use();
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 3);
		glBindVertexArray(0);

		glfwSwapBuffers(window);
	}
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
	glfwTerminate();
	return 0;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	// When a user presses the escape key, we set the WindowShouldClose property to true, 
	// closing the application
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
}

/*Before CShader.h include
void create_spo(GLuint &spo)
{
	char *vertexShaderSource = "\n\
	#version 330 core\n\
	layout(location = 0) in vec3 position;\n\
	layout(location = 1) in vec3 color;\n\
	out vec3 ourColor;\n\
	void main()\n\
	{\n\
		gl_Position = vec4(position, 1.0);\n\
		ourColor = color;\n\
	}";
	GLuint vertexShader;
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);
	GLint success;
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	GLchar infoLog[512];
	if (!success)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << endl;
	}
	char *fragmentShaderSource = "\n\
	#version 330 core\n\
	in vec3 ourColor;\n\
	out vec4 color;\n\
	void main()\n\
	{\n\
		color = vec4(ourColor, 1.0);\n\
	}";
	GLuint fragmentShader;
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << endl;
	}
	spo = glCreateProgram();
	glAttachShader(spo, vertexShader);
	glAttachShader(spo, fragmentShader);
	glLinkProgram(spo);
	glGetProgramiv(spo, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(spo, 512, NULL, infoLog);
		cout << "ERROR::SHADER::PROGRAMM::LINKING_FAILED\n" << infoLog << endl;
	}
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
}*/
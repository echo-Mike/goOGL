#include "main.h"

static const std::string vspath("C:\\Users\\123\\Desktop\\OpenGL\\PROJECTS\\GOOPENGL\\goOGL6\\src\\shaders\\shader.vs");
static const std::string fspath("C:\\Users\\123\\Desktop\\OpenGL\\PROJECTS\\GOOPENGL\\goOGL6\\src\\shaders\\shader.fs");
static Shader *shaderProgram;
static const std::string tpath1("C:\\Users\\123\\Desktop\\OpenGL\\PROJECTS\\GOOPENGL\\goOGL6\\src\\data\\container.jpg");
static const std::string tpath2("C:\\Users\\123\\Desktop\\OpenGL\\PROJECTS\\GOOPENGL\\goOGL6\\src\\data\\wall.jpg");
static const std::string tpath3("C:\\Users\\123\\Desktop\\OpenGL\\PROJECTS\\GOOPENGL\\goOGL6\\src\\data\\awesomeface.png");
static Texture *container, *container2;
glm::vec3 axis(0.0, 0.0, 0.1);
static SimpleModel *model;


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

	//ƒ¿ÕÕ€≈

	GLfloat positions[] = {
		0.5f, 0.5f, 0.0f,
		0.5f, -0.5f, 0.0f,
		-0.5f, -0.5f, 0.0f,
		-0.5f, 0.5f, 0.0f
	};
	GLfloat colors[] = {
		1.0f, 0.0f, 0.0f, 
		0.0f, 1.0f, 0.0f, 
		0.0f, 0.0f, 1.0f, 
		1.0f, 1.0f, 0.0f
	};
	GLfloat texCoords[] = {
		1.0f, 1.0f,
		1.0f, 0.0f,
		0.0f, 0.0f,
		0.0f, 1.0f
	};
	GLuint indices[] = {  // Note that we start from 0!
		0, 1, 3, // First Triangle
		1, 2, 3  // Second Triangle
	};

	//—Œ«ƒ¿®Ã ÃŒƒ≈À‹
	model = new SimpleModel(6, positions, texCoords, colors, nullptr, indices);
	//—Œ«ƒ¿®Ã ÿ≈…ƒ≈–
	shaderProgram = new Shader(vspath.c_str(), fspath.c_str());
	model->setShader(shaderProgram);
	//œÂÂ‰‡∏Ï ÚÂÍÒÚÛ˚
	model->pushTexture(Texture(tpath1.c_str()));
	model->pushTexture(Texture(tpath3.c_str()));
	//«‡„ÛÊ‡ÂÏ ÚÂÍÒÚÛ˚
	model->loadAllTextures();
	//¡ËÌ‰ËÏ ‰ÂÈÒÚ‚Ëˇ ‚ VAO ÏÓ‰ÂÎË
	model->Build();
	
	GLfloat offset;
	glm::mat4 trans;
	model->pushTransformation(trans);
	model->pushTransformation(trans);

	//main loop
	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		offset = (float)glfwGetTime();

		trans = glm::translate(glm::mat4(), glm::vec3(0.5, -0.5, 0.0));
		trans = glm::rotate(trans, glm::radians(offset*10.0f), axis);
		model->setTransformation(trans);
		trans = glm::translate(glm::mat4(), glm::vec3(-0.5, 0.5, 0.0));
		trans = glm::scale(trans, glm::vec3(glm::sin(offset), glm::cos(offset), 0.0));
		model->setTransformation(trans, 1);

		model->drawInstances(0, 2);

		glfwSwapBuffers(window);
	}
	glfwTerminate();
	return 0;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	static bool path = false;
	if (action == GLFW_PRESS) {
		switch (key) {
		case GLFW_KEY_Q:
			glfwSetWindowShouldClose(window, GL_TRUE);
			break;
		case GLFW_KEY_R:
			shaderProgram->Reload();
			break;
		}
	}
}

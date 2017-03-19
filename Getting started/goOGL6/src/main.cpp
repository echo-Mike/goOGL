#include "main.h"

static const std::string vspath("C:\\Users\\123\\Desktop\\OpenGL\\PROJECTS\\GOOPENGL\\goOGL6\\src\\shaders\\shader.vs");
static const std::string fspath("C:\\Users\\123\\Desktop\\OpenGL\\PROJECTS\\GOOPENGL\\goOGL6\\src\\shaders\\shader.fs");
static Shader *shaderProgram;
static const std::string tpath1("C:\\Users\\123\\Desktop\\OpenGL\\PROJECTS\\GOOPENGL\\goOGL6\\src\\data\\container.jpg");
static const std::string tpath2("C:\\Users\\123\\Desktop\\OpenGL\\PROJECTS\\GOOPENGL\\goOGL6\\src\\data\\wall.jpg");
static const std::string tpath3("C:\\Users\\123\\Desktop\\OpenGL\\PROJECTS\\GOOPENGL\\goOGL6\\src\\data\\awesomeface.png");
static const std::string tpath4("C:\\Users\\123\\Desktop\\OpenGL\\PROJECTS\\GOOPENGL\\goOGL6\\src\\data\\cubelayout.png");
static CombinedModel *model;


int main()
{
	//�������������
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

	//������
	GLfloat cube_s = 10.0f;

	GLfloat cube[] = {
		-cube_s, -cube_s,  cube_s,
		 cube_s, -cube_s,  cube_s,
		 cube_s,  cube_s,  cube_s,
		-cube_s,  cube_s,  cube_s,
		-cube_s, -cube_s, -cube_s,
		 cube_s, -cube_s, -cube_s,
		 cube_s,  cube_s, -cube_s,
		-cube_s,  cube_s, -cube_s
	};
	GLfloat colors[] = {
		1.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 
		0.0f, 0.0f, 1.0f, 
		1.0f, 1.0f, 0.0f
	};
	GLfloat texCoords[] = {
		0.0f, 0.0f, 
		1.0f, 1.0f,
		1.0f, 0.0f,
		
		0.0f, 1.0f
	};
	GLuint indices[] = {
		0, 1, 2,  0, 2, 3, //Front 
		1, 5, 6,  1, 6, 2, //Right 
		5, 4, 7,  5, 7, 6, //Back 
		4, 0, 3,  4, 3, 7, //Left 
		3, 2, 6,  3, 6, 7, //Top 
		4, 5, 1,  4, 1, 0  //Bottom
	};

	//������� ������
	model = new CombinedModel(CombinedModel::Layout(0, 3, -1, 3, 3, 2, -1, 3, 36, 0, false), vertices);
	//������� ������
	shaderProgram = new Shader(vspath.c_str(), fspath.c_str());
	model->setShader(shaderProgram);
	//������� ��������
	model->pushTexture(Texture(tpath1.c_str()));
	model->pushTexture(Texture(tpath3.c_str()));
	//��������� ��������
	model->loadAllTextures();
	//������ �������� � VAO ������
	model->Build();


	GLfloat offset;
	//glm::mat4 trans = glm::rotate(glm::mat4(), glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	//model->pushTransformation(trans);
	model->genInstances();

	glm::mat4 view = glm::translate(glm::mat4(), glm::vec3(0.0f, 0.0f, -3.0f));
	shaderProgram->pushUniform("view", &view, Shader::UNIFORMMATRIX4FV);
	glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)width / height, 0.1f, 1000.0f);
	shaderProgram->pushUniform("projection", &projection, Shader::UNIFORMMATRIX4FV);

	//main loop
	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		//offset = (float)glfwGetTime();
		model->drawInstance();
		//DEBUG_OUT << glGetError() << DEBUG_NEXT_LINE;

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

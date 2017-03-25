#include "main.h"

static const std::string vspath("C:\\Users\\123\\Desktop\\OpenGL\\PROJECTS\\GOOPENGL\\goOGL7\\src\\shaders\\shader.vs");
static const std::string fspath("C:\\Users\\123\\Desktop\\OpenGL\\PROJECTS\\GOOPENGL\\goOGL7\\src\\shaders\\shader.fs");
static Shader *shaderProgram;
static const std::string tpath1("C:\\Users\\123\\Desktop\\OpenGL\\PROJECTS\\GOOPENGL\\goOGL7\\src\\data\\container.jpg");
static const std::string tpath2("C:\\Users\\123\\Desktop\\OpenGL\\PROJECTS\\GOOPENGL\\goOGL7\\src\\data\\wall.jpg");
static const std::string tpath3("C:\\Users\\123\\Desktop\\OpenGL\\PROJECTS\\GOOPENGL\\goOGL7\\src\\data\\awesomeface.png");
static const std::string tpath4("C:\\Users\\123\\Desktop\\OpenGL\\PROJECTS\\GOOPENGL\\goOGL7\\src\\data\\cubelayout.png");
static SimpleModel<> *model;


//Key handler array
bool keys[1024];

GLfloat deltaTime = 0.0f;	// Time between current frame and last frame
GLfloat lastFrame = 0.0f;  	// Time of last frame

GLfloat lastX = 400, lastY = 300;


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
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK)
	{
		cout << "Failed to initialize GLEW" << endl;
		return -2;
	}
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);
	//Enable z-buffer test
	glEnable(GL_DEPTH_TEST);
	//Disable mouse movement
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);


	//ƒ¿ÕÕ€≈
	glm::vec3 cubePositions[] = {
		glm::vec3( 0.0f,  0.0f,  0.0f), 
		glm::vec3( 2.0f,  5.0f, -15.0f), 
		glm::vec3(-1.5f, -2.2f, -2.5f),  
		glm::vec3(-3.8f, -2.0f, -12.3f),  
		glm::vec3( 2.4f, -0.4f, -3.5f),  
		glm::vec3(-1.7f,  3.0f, -7.5f),  
		glm::vec3( 1.3f, -2.0f, -2.5f),  
		glm::vec3( 1.5f,  2.0f, -2.5f), 
		glm::vec3( 1.5f,  0.2f, -1.5f), 
		glm::vec3(-1.3f,  1.0f, -1.5f)  
	};


	//—Œ«ƒ¿®Ã ÃŒƒ≈À‹
	model = new CombinedModel<>(CombinedModel<>::Layout(0, 3, -1, 3, 3, 2, -1, 3, 36, 0, false), vertices);
	//model = new SeparateModel(4, 6, vert, colors, texCoords, nullptr, ind);
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
	for (int _index = 0; _index < 10; _index++) {
		glm::mat4 _transform;
		_transform = glm::translate(_transform, cubePositions[_index]);
		_transform = glm::rotate(_transform, glm::radians(_index * 20.f), glm::vec3(1.0f, 0.3f, 0.5f));
		model->pushTransformation(_transform);
	}



	glm::mat4 view = glm::translate(glm::mat4(), glm::vec3(0.0f, 0.0f, -3.0f));
	int viewIndex = shaderProgram->pushUniform("view", &view, Shader::UNIFORMMATRIX4FV);
	glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)width / height, 0.1f, 1000.0f);
	int projectionIndex = shaderProgram->pushUniform("projection", &projection, Shader::UNIFORMMATRIX4FV);

	//main loop
	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();
		do_movement();

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		//Update view matrix
		view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
		shaderProgram->setUniform(&view, viewIndex);
		//Update projection matrix
		projection = glm::perspective(fov, (float)width / height, 0.1f, 1000.0f);
		shaderProgram->setUniform(&projection, projectionIndex);

		offset = (float)glfwGetTime();
		
		model->setTransformation(glm::rotate(glm::mat4(), glm::radians(10.0f * offset), glm::vec3(1.0f, 0.3f, 0.5f)));
		model->drawInstances(0, 10);
		//DEBUG_OUT << glGetError() << DEBUG_NEXT_LINE;

		glfwSwapBuffers(window);
		GLfloat currentFrame = (float)glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
	}
	glfwTerminate();
	return 0;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (action == GLFW_PRESS)
		keys[key] = true;
	if (action == GLFW_RELEASE)
		keys[key] = false;
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

void do_movement()
{
	// Camera controls
	GLfloat cameraSpeed = 1.0f * deltaTime;
	if (keys[GLFW_KEY_W])
		cameraPos += cameraSpeed * cameraFront;
	if (keys[GLFW_KEY_S])
		cameraPos -= cameraSpeed * cameraFront;
	if (keys[GLFW_KEY_A])
		cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	if (keys[GLFW_KEY_D])
		cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
	static bool firstMouse = true;
	if (firstMouse) {
		lastX = (GLfloat)xpos;
		lastY = (GLfloat)ypos;
		firstMouse = false;
	}
	GLfloat xoffset = (GLfloat)xpos - lastX;
	GLfloat yoffset = lastY - (GLfloat)ypos; // Reversed since y-coordinates range from bottom to top
	lastX = (GLfloat)xpos;
	lastY = (GLfloat)ypos;
	
	GLfloat sensitivity = 0.005f;
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	glm::vec3 cameraRight = glm::cross(cameraFront, cameraUp);

	//For plane-like cam
	//glm::quat temp = glm::angleAxis(yoffset, cameraRight) * glm::angleAxis(xoffset, cameraFront);
	//For fps like cam
	glm::quat temp = glm::angleAxis(yoffset, cameraRight) * glm::angleAxis(xoffset, -cameraUp);

	cameraFront = glm::normalize(temp * cameraFront);
	cameraUp = glm::normalize(temp * cameraUp);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
	if (fov >= 1.0f && fov <= 90.0f)
		fov -= (GLfloat)yoffset * 0.1f;
	if (fov <= 1.0f)
		fov = 1.0f;
	if (fov >= 90.0f)
		fov = 90.0f;
}
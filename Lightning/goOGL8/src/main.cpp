#include "main.h"

/*
static SimpleModel<> *model;
static Shader *shaderProgram;
static const std::string vspath(R"(C:\Users\123\Desktop\OpenGL\PROJECTS\GOOPENGL\goOGL8\src\GLSL\shader.vs)");
static const std::string fspath(R"(C:\Users\123\Desktop\OpenGL\PROJECTS\GOOPENGL\goOGL8\src\GLSL\shader.fs)");
static const std::string tpath1(R"(C:\Users\123\Desktop\OpenGL\PROJECTS\GOOPENGL\goOGL8\data\container.jpg)");
*/

static SimpleModel<> *WorldOrigin;
static Shader *WorldOriginShader;
static const std::string WorldOriginVSP(R"(C:\Users\123\Desktop\OpenGL\PROJECTS\GOOPENGL\goOGL8\src\GLSL\cuboid.vs)");
static const std::string WorldOriginFSP(R"(C:\Users\123\Desktop\OpenGL\PROJECTS\GOOPENGL\goOGL8\src\GLSL\cuboid.fs)");
static const std::string WorldOriginTPath(R"(C:\Users\123\Desktop\OpenGL\PROJECTS\GOOPENGL\goOGL8\data\cubelayout.png)");

static SimpleModel<> *cube;
static Shader *cubeShader;
static const std::string cubeVSP(R"(C:\Users\123\Desktop\OpenGL\PROJECTS\GOOPENGL\goOGL8\src\GLSL\cube.vs)");
static const std::string cubeFSP(R"(C:\Users\123\Desktop\OpenGL\PROJECTS\GOOPENGL\goOGL8\src\GLSL\cube.fs)");

static SimpleModel<> *lightCube;
static Shader *lightCubeShader;
static const std::string lightCubeVSP(R"(C:\Users\123\Desktop\OpenGL\PROJECTS\GOOPENGL\goOGL8\src\GLSL\lightCube.vs)");
static const std::string lightCubeFSP(R"(C:\Users\123\Desktop\OpenGL\PROJECTS\GOOPENGL\goOGL8\src\GLSL\lightCube.fs)");

static SimpleCamera *camera;

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

	//—Œ«ƒ¿®Ã ÃŒƒ≈À‹
	WorldOrigin = new CombinedModel<>(CombinedModel<>::Layout(0, 3, 5, 3, 3, 2, -1, 3, 36, 0, false), WorldOriginV);
	cube = new SeparateModel<>(8, 36, cubeV, nullptr, nullptr, nullptr, cubeI);
	lightCube = new SeparateModel<>(8, 36, cubeV, nullptr, nullptr, nullptr, cubeI);
	//—Œ«ƒ¿®Ã ÿ≈…ƒ≈–
	WorldOriginShader = new Shader(WorldOriginVSP.c_str(), WorldOriginFSP.c_str());
	cubeShader = new Shader(cubeVSP.c_str(), cubeFSP.c_str());
	lightCubeShader = new Shader(lightCubeVSP.c_str(), lightCubeFSP.c_str());

	WorldOrigin->setShader(WorldOriginShader);
	cube->setShader(cubeShader);
	lightCube->setShader(lightCubeShader);
	//œÂÂ‰‡∏Ï ÚÂÍÒÚÛ˚
	WorldOrigin->pushTexture(Texture(WorldOriginTPath.c_str()));
	//«‡„ÛÊ‡ÂÏ ÚÂÍÒÚÛ˚
	WorldOrigin->loadAllTextures();
	//¡ËÌ‰ËÏ ‰ÂÈÒÚ‚Ëˇ ‚ VAO ÏÓ‰ÂÎË
	WorldOrigin->Build();
	cube->Build();
	lightCube->Build();

	std::array<InstanceData, 3> instances;

	GLfloat offset;
	
	instances[0](our::mat4(), WorldOriginShader, "model");
	WorldOrigin->pushInstance(&instances[0]);

	instances[1](our::translate(glm::mat4(), glm::vec3(0.0f, 5.0f, 0.0f)), cubeShader, "model");
	cube->pushInstance(&instances[1]);
	Vec3Handler<> cubeColor(glm::vec3(1.0f, 0.5f, 0.31f), cubeShader, "objectColor");

	instances[2](our::translate(glm::mat4(), glm::vec3(5.0f, 5.0f, 0.0f)), lightCubeShader, "model");
	lightCube->pushInstance(&instances[2]);
	Vec3Handler<> lightColor(glm::vec3(1.0f, 1.0f, 1.0f), lightCubeShader, "lightColor");
	lightColor.push(cubeShader);


	camera = new SimpleCamera();
	camera->setPerspectiveData(glm::radians(45.0f), (float)width / height, 0.1f, 1000.0f);
	camera->setProjectionMode(SimpleCamera::ProjectionMode::MODE_PERSPECTIVE);
	camera->Setup(WorldOriginShader);
	camera->Setup(cubeShader);
	camera->Setup(lightCubeShader);
	camera->speed = 1.0f;
	camera->sensitivity.x = 0.1f;
	camera->sensitivity.y = 0.1f;
	camera->position.z = 5.0f;
	camera->position.y = 6.0f;

	//main loop
	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();
		camera->controlMovement(deltaTime, keys[GLFW_KEY_W], keys[GLFW_KEY_S], keys[GLFW_KEY_A], keys[GLFW_KEY_D]);

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		//Update camera state
		camera->Update();

		offset = (float)glfwGetTime();
		WorldOrigin->drawInstance();
		cube->drawInstance();
		lightCube->drawInstance();
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
			//shaderProgram->Reload();
			break;
		}
	}
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

	camera->controlView(deltaTime, xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
	static GLfloat fov = 45.0f;
	if (fov >= 1.0f && fov <= 90.0f)
		fov -= (GLfloat)yoffset * 0.1f;
	if (fov <= 1.0f)
		fov = 1.0f;
	if (fov >= 90.0f)
		fov = 90.0f;
	SimpleCamera::PerspectiveData _data(camera->getPrespectiveData());
	_data.fov = fov;
	camera->setPerspectiveData(_data);
	camera->updateProjection();
}
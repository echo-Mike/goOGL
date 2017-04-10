#include "main.h"

static SimpleModel<> *WorldOrigin;
static Shader *WorldOriginShader;
static const std::string WorldOriginVSP(R"(C:\Users\123\Desktop\OpenGL\PROJECTS\GOOPENGL\Lightning\goOGL10\src\GLSL\cuboid.vs)");
static const std::string WorldOriginFSP(R"(C:\Users\123\Desktop\OpenGL\PROJECTS\GOOPENGL\Lightning\goOGL10\src\GLSL\cuboid.fs)");
static const std::string WorldOriginTPath(R"(C:\Users\123\Desktop\OpenGL\PROJECTS\GOOPENGL\Assets\cubelayout.png)");

static SimpleModel<> *cube;
static Shader *cubeShader;
static const std::string cubeVSP(R"(C:\Users\123\Desktop\OpenGL\PROJECTS\GOOPENGL\Lightning\goOGL10\src\GLSL\cube.vs)");
static const std::string cubeFSP(R"(C:\Users\123\Desktop\OpenGL\PROJECTS\GOOPENGL\Lightning\goOGL10\src\GLSL\cube.fs)");

static SimpleModel<> *lightCube;
static Shader *lightCubeShader;
static const std::string lightCubeVSP(R"(C:\Users\123\Desktop\OpenGL\PROJECTS\GOOPENGL\Lightning\goOGL10\src\GLSL\lightCube.vs)");
static const std::string lightCubeFSP(R"(C:\Users\123\Desktop\OpenGL\PROJECTS\GOOPENGL\Lightning\goOGL10\src\GLSL\lightCube.fs)");

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
	if (window == nullptr) {
		cout << "Failed to create GLFW window" << endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK) {
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

	//—Œ«ƒ¿®Ã ÃŒƒ≈À‹
	WorldOrigin = new CombinedModel<>(CombinedModel<>::Layout(0, 3, 5, 3, 3, 2, -1, 3, 36, 0, false), WorldOriginV);
	cube = new CombinedModel<>(CombinedModel<>::Layout(0,3, -1,0, -1,0, 3,3, 36, 0, false), cubeWithN);
	lightCube = new SeparateModel<>(8, 36, cubeV, nullptr, nullptr, nullptr, cubeI);
	//—Œ«ƒ¿®Ã ÿ≈…ƒ≈–
	WorldOriginShader = new Shader(WorldOriginVSP.c_str(), WorldOriginFSP.c_str());
	cubeShader = new Shader(cubeVSP.c_str(), cubeFSP.c_str());
	lightCubeShader = new Shader(lightCubeVSP.c_str(), lightCubeFSP.c_str());

	WorldOrigin->setShader(WorldOriginShader);
	cube->setShader(cubeShader);
	lightCube->setShader(lightCubeShader);
	//œÂÂ‰‡∏Ï ÚÂÍÒÚÛ˚
	WorldOrigin->pushTexture(&Texture(WorldOriginTPath.c_str()));
	//«‡„ÛÊ‡ÂÏ ÚÂÍÒÚÛ˚
	WorldOrigin->loadAllTextures();
	//¡ËÌ‰ËÏ ‰ÂÈÒÚ‚Ëˇ ‚ VAO ÏÓ‰ÂÎË
	WorldOrigin->Build();
	cube->Build();
	lightCube->Build();

	std::array<InstanceDataInterface*, 26> instances;

	GLfloat offset = 0.0f;

	instances[0] = new CommonInstance(our::mat4(), WorldOriginShader, "model");
	WorldOrigin->pushInstance(instances[0]);
	

	LightsourcePOD light(	glm::vec3(5.0f, 5.0f, 0.0f), 
							glm::vec3(0.2f, 0.2f, 0.2f), 
							glm::vec3(0.5f, 0.5f, 0.5f), 
							glm::vec3(1.0f, 1.0f, 1.0f));
	Vec3AutomaticStorage<> lightColor(&glm::vec3(1.0f), lightCubeShader, "lightColor");
	glm::mat4 lightSourceScale = glm::scale(glm::mat4(), glm::vec3(0.1f));
	instances[1] = new LightsourceData(our::translate(lightSourceScale, light.position), light, nullptr, "model", "light");
	dynamic_cast<LightsourceData*>(instances[1])->modelMatrix.setShader(lightCubeShader);
	dynamic_cast<LightsourceData*>(instances[1])->push(cubeShader);
	lightCube->pushInstance(instances[1]);

	for (int _index = 0; _index  < 24; _index++) {
		instances[_index+2] = new InstanceData(our::translate(glm::mat4(), positions[_index]), materials[_index], cubeShader, "model", "material");
		cube->pushInstance(instances[_index+2]);
	}
	
	camera = new SimpleCamera();
	camera->setPerspectiveData(glm::radians(100.0f), (float)width / height, 0.1f, 1000.0f);
	camera->setProjectionMode(SimpleCamera::ProjectionMode::MODE_PERSPECTIVE);
	camera->Setup(WorldOriginShader);
	camera->Setup(cubeShader);
	camera->Setup(lightCubeShader);
	camera->speed = 1.5f;
	camera->sensitivity.x = 0.15f;
	camera->sensitivity.y = 0.15f;
	camera->position.z = 5.0f;
	camera->position.y = 6.0f;
	camera->lockUp = true;
	Vec3AutomaticObserver<> viewPosition(&(camera->position), cubeShader, "viewPos");

	//main loop
	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();
		camera->controlMovement(deltaTime,	keys[GLFW_KEY_W], keys[GLFW_KEY_S], 
											keys[GLFW_KEY_A], keys[GLFW_KEY_D], 
											keys[GLFW_KEY_SPACE], keys[GLFW_KEY_LEFT_SHIFT]);

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		//Update camera state
		camera->Update();

		offset += deltaTime;
		LightsourceData* _buffptr = dynamic_cast<LightsourceData*>(instances[1]);
		_buffptr->lightsource(glm::vec3(3.0f, 5.0f, 0.0f) + 2.5f * glm::vec3(0.0f, glm::sin(offset), glm::cos(offset)), LightsourcePOD::POSITION);
		_buffptr->modelMatrix.setValue(glm::translate(glm::mat4(), _buffptr->lightsource.getValue(LightsourcePOD::POSITION)) * lightSourceScale);
		lightColor.setValue(glm::vec3(glm::sin(glfwGetTime() * 2.0f), glm::sin(glfwGetTime() * 0.7f), glm::sin(glfwGetTime()*1.3f)));
		_buffptr->lightsource(lightColor.getValue() * glm::vec3(0.2f), LightsourcePOD::AMBIENT);
		_buffptr->lightsource(lightColor.getValue() * glm::vec3(0.5f), LightsourcePOD::DIFFUSE);

		offset = (float)glfwGetTime();
		WorldOrigin->drawInstance();
		cube->drawInstances(0, 24);
		lightCube->drawInstance();

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
			WorldOriginShader->Reload();
			lightCubeShader->Reload();
			cubeShader->Reload();
			break;
		case GLFW_KEY_E:
			camera->lockUp = !camera->lockUp;
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
	if (keys[GLFW_KEY_LEFT_CONTROL]) {
		SimpleCamera::PerspectiveData _data(camera->getPrespectiveData());
		_data.fov -= (GLfloat)yoffset * 0.1f;
		camera->setPerspectiveData(_data);
		camera->updateProjection();
		return;
	}
	camera->speed += (GLfloat)yoffset * 0.5f;
}
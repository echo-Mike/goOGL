//GLEW
//satic linking for GLEW lib
#define GLEW_STATIC
#include <GL/glew.h>
//GLFW
#include <GLFW/glfw3.h>
#include <iostream>

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);

int main()
{
	using std::cout;
	using std::endl;

	//������������� glfw
	glfwInit();
	//��������� glfw
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	//������ �� ������������� ���������� ������� opengl
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	//���� ��� �� ����� ���:
	// glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	
	//�������� ����� � ogl �������� ��� ����
	GLFWwindow *window = glfwCreateWindow(800, 600, "LearnOpenGL", nullptr, nullptr);
	if (window == nullptr)
	{
		cout << "Failed to create GLFW window" << endl;
		glfwTerminate();
		return -1;
	}
	//���������� �������� ���� window ������� ��� ������� ������
	glfwMakeContextCurrent(window);
	//������������ callback �� ������� key_callback
	glfwSetKeyCallback(window, key_callback);

	//������������� glew
	//�������� ��������� ����������� ogl ������
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK)
	{
		cout << "Failed to initialize GLEW" << endl;
		return -1;
	}
	
	//������� ������� ���� �� glew
	int width, height; 
	glfwGetFramebufferSize(window, &width, &height);
	//��������� ������� glviewport � ����������� � �����������
	//glViewport specifies the affine transformation of x and y from normalized device coordinates(-1;1) to window coordinates
	glViewport(0, 0, width, height);
	
	//�������� ����������� � ������������� �����������
	GLfloat vertices[] = {
		-0.5f, -0.5f, 0.0f,
		0.5f, -0.5f, 0.0f,
		0.0f, 0.5f, 0.0f
	};
	//VBO - ����������� ���������, �������� ������� ������ � ������ ����������
	GLuint VBO;
	//������ �������� ������, �� �� ������ ����� �������, �� ����� ���� ��������� � ������� �� ��� ��� ���� ��� ����� ������ ����
	glGenBuffers(1, &VBO);
	//����� ��������� ���������� GL_ARRAY_BUFFER 
	//����������� ����� � ������ �������
	//������ � VAO: glBindBuffer(GL_ARRAY_BUFFER, VBO);
	//��������� ����� �������
	//������ � VAO: glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	//��� � ����� � ogl ������ 1 GL_ARRAY_BUFFER ��� ��� ��� �������� ����� �����������


	//������ ��������� �����, �� � ����������� ogl �� �� ��������� �� �������
	char *vertexShaderSource = "\n\
		#version 330 core\n\
		layout(location = 0) in vec3 position;\n\
		void main()\n\
		{\n\
			gl_Position = vec4(position.x, position.y, position.z, 1.0);\n\
		}";
	//��� ��� ���������� �������
	GLuint vertexShader;
	//������ ������
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	//������ ���� ������� � ���������� �������
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	//�������� � ��������
	glCompileShader(vertexShader);
	//�������� ����������
	GLint success;
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	GLchar infoLog[512];
	if (!success)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << endl;
	}

	//�� ��� �� ������� ����� ����������� ������
	char *fragmentShaderSource = "\n\
		#version 330 core\n\
		out vec4 color;\n\
		void main()\n\
		{\n\
			color = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n\
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

	//��������� ���� - �������� ��������� ���������
	/*A shader program object is the final linked version of multiple shaders combined. 
	To use the recently compiled shaders we have to link them to a shader program object and then 
	activate this shader program when rendering objects. The activated shader program's shaders will 
	be used when we issue render calls.*/
	GLuint shaderProgram;
	shaderProgram = glCreateProgram();
	//����� ������������ ��������� ��������� ������� � ������� ��������� ���������
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	//������� ��
	glLinkProgram(shaderProgram);
	//�������� �� ������
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		cout << "ERROR::SHADER::PROGRAMM::LINKING_FAILED\n" << infoLog << endl;
	}
	//����� �������� ���������� ������� ��� ������ �� �����
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	//������ ���������� ��������� ��������� ��������, ������ ����� ���������� ����� ��������� ����� ��
	glUseProgram(shaderProgram);

	//��������� ���� - ������� ������ ������ � ���������� ��������� ���������
	/*
	������� �� ����������:
	0 - � ��������� ������� �� ��������� ��� ����� ��� layout (location = 0), ����� ������� �� ������� ���� ��������� ������
	3 - ������ �������� � ������ (��� vec3)
	GL_FLOAT - ����� ��� ������ ��������
	GL_FALSE - ����� ��������������� ������ ��� ���
	3 * sizeof(GLfloat) - ������ ������ ���������� 1 ��������� � ������
	(GLvoid*)0 - ������ �� �������� ������������� ������
	*/
	//������ � VAO: glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	//��������� ������������� �������� �� location = 0
	//������ � VAO: glEnableVertexAttribArray(0);

	/*��� ��� ����� - �����������, �� ��������� �� ������ ��� - �� ��������, �� ����� ���� ������� � VAO(vertex array object)
	� ������:
	A vertex array object stores the following:
		Calls to glEnableVertexAttribArray or glDisableVertexAttribArray.
		Vertex attribute configurations via glVertexAttribPointer.
		Vertex buffer objects associated with vertex attributes by calls to glVertexAttribPointer.
	*/
	//�������� ���� ����� ��� ������ �������
	GLuint VAO;
	glGenVertexArrays(1, &VAO);
	//�������� ���� �������� � VAO
	glBindVertexArray(VAO);
		// 2. Copy our vertices array in a buffer for OpenGL to use
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		// 3. Then set our vertex attributes pointers
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(0);
	//4. Unbind the VAO
	glBindVertexArray(0);

	//main loop
	while (!glfwWindowShouldClose(window))
	{
		//��� ������, �������������� glfw
		glfwPollEvents();
		//���� �������� �� ����������

		//��������� ���� ��������, ����� �� ������ ������ ���, �� ��������������� ��� �������� ���������
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		//�������� �������� �����
		glClear(GL_COLOR_BUFFER_BIT);

		//������ ��� ����������� ��������� �������, VAO, VBO(�� ����)
		glUseProgram(shaderProgram);
		glBindVertexArray(VAO);
		//���������������� ���� ������� �� VAO ��� ��������� ������������, ������� � 0 �������, ���������� 3 �������
		glDrawArrays(GL_TRIANGLES, 0, 3);
		glBindVertexArray(0);

		//���� ���������� � ���������� ������ winndow
		glfwSwapBuffers(window);
	}
	//����������� ������ glfw � ���������
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
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

	//инициализация glfw
	glfwInit();
	//настройка glfw
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	//запрет на использование устаревших функций opengl
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	//если мак то нужно ещё:
	// glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	
	//Создадим октно и ogl контекст для него
	GLFWwindow *window = glfwCreateWindow(800, 600, "LearnOpenGL", nullptr, nullptr);
	if (window == nullptr)
	{
		cout << "Failed to create GLFW window" << endl;
		glfwTerminate();
		return -1;
	}
	//установить контекст окна window текущим для данного потока
	glfwMakeContextCurrent(window);
	//устанавиваем callback на функцию key_callback
	glfwSetKeyCallback(window, key_callback);

	//инициализация glew
	//включить поддержку современных ogl техник
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK)
	{
		cout << "Failed to initialize GLEW" << endl;
		return -1;
	}
	
	//получим размеры окна от glew
	int width, height; 
	glfwGetFramebufferSize(window, &width, &height);
	//установим размеры glviewport в соответсвии с полученными
	//glViewport specifies the affine transformation of x and y from normalized device coordinates(-1;1) to window coordinates
	glViewport(0, 0, width, height);
	
	//создадим триугольник в нормированных координатах
	GLfloat vertices[] = {
		-0.5f, -0.5f, 0.0f,
		0.5f, -0.5f, 0.0f,
		0.0f, 0.5f, 0.0f
	};
	//VBO - специальная структура, хранящая вертекс данные в памяти видеокарты
	GLuint VBO;
	//Создаёт буферный объект, но не создаёт самих буферов, их может быть несколько в объекте до тех пор пока они имеют разные типы
	glGenBuffers(1, &VBO);
	//Буфер вертексов называется GL_ARRAY_BUFFER 
	//Привязываем буфер к нашему объекту
	//Теперь в VAO: glBindBuffer(GL_ARRAY_BUFFER, VBO);
	//заполняем буфер данными
	//Теперь в VAO: glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	//как я понял у ogl только 1 GL_ARRAY_BUFFER так что его посоянно нужно перегружать


	//Создаём вершинный шейдр, тк в современном ogl он по умолчанию не задаётся
	char *vertexShaderSource = "\n\
		#version 330 core\n\
		layout(location = 0) in vec3 position;\n\
		void main()\n\
		{\n\
			gl_Position = vec4(position.x, position.y, position.z, 1.0);\n\
		}";
	//Имя для шейдерного объекта
	GLuint vertexShader;
	//Создаём объект
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	//Биндим сорс шейдера к созданному объекту
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	//Компилим в рантайме
	glCompileShader(vertexShader);
	//Проверка компиляции
	GLint success;
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	GLchar infoLog[512];
	if (!success)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << endl;
	}

	//По той же причине пишем фрагментный шейдер
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

	//Следующий этап - создание шейдерной программы
	/*A shader program object is the final linked version of multiple shaders combined. 
	To use the recently compiled shaders we have to link them to a shader program object and then 
	activate this shader program when rendering objects. The activated shader program's shaders will 
	be used when we issue render calls.*/
	GLuint shaderProgram;
	shaderProgram = glCreateProgram();
	//Далее присоединяем созданные шейдерные объекты к объекту шейдерной программы
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	//Линкуем их
	glLinkProgram(shaderProgram);
	//Проверка на ошибки
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		cout << "ERROR::SHADER::PROGRAMM::LINKING_FAILED\n" << infoLog << endl;
	}
	//После линковки шейдеровые объекты нам больше не нужны
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	//Теперь заставляем шейдерную программу работать, каждый вызов прорисовки будет проходить через неё
	glUseProgram(shaderProgram);

	//Последний этап - связать данные вершин с атрибутами шейдерной программы
	/*
	Разберём по параметрам:
	0 - в вершинном шейдере мы указывали это место как layout (location = 0), таким образом мы говорим куда присылать данные
	3 - размер атрибута в штуках (там vec3)
	GL_FLOAT - задаём тип данных атрибута
	GL_FALSE - хотим нормализировать данные или нет
	3 * sizeof(GLfloat) - размер память занимаемой 1 атрибутом в байтах
	(GLvoid*)0 - оффсет по которому располагаются данные
	*/
	//Теперь в VAO: glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	//Разрешаем использование атрибута по location = 0
	//Теперь в VAO: glEnableVertexAttribArray(0);

	/*Все эти этапы - великолепны, но повторять их каждый раз - не комильфо, по этому чать убирают в VAO(vertex array object)
	а именно:
	A vertex array object stores the following:
		Calls to glEnableVertexAttribArray or glDisableVertexAttribArray.
		Vertex attribute configurations via glVertexAttribPointer.
		Vertex buffer objects associated with vertex attributes by calls to glVertexAttribPointer.
	*/
	//Создадим один такой для нашего объекта
	GLuint VAO;
	glGenVertexArrays(1, &VAO);
	//Начинаем бинд операций в VAO
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
		//все ивенты, обрабатываемые glfw
		glfwPollEvents();
		//наши действия по рендерингу

		//установим цвет отчистки, можно не делать каждый раз, он устанавливается для текущего контекста
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		//почистим цветовой буфер
		glClear(GL_COLOR_BUFFER_BIT);

		//Рисуем наш триугольник используя шейдеры, VAO, VBO(не явно)
		glUseProgram(shaderProgram);
		glBindVertexArray(VAO);
		//Интерпритировать весь биндинг из VAO как рисование риугольников, начиная с 0 вершины, нарисовать 3 вершины
		glDrawArrays(GL_TRIANGLES, 0, 3);
		glBindVertexArray(0);

		//свап первичного и вторичного буфера winndow
		glfwSwapBuffers(window);
	}
	//освобождаем ресрсы glfw и остальные
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
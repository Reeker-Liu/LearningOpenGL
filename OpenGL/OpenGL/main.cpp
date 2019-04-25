#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

const unsigned int WIDTH = 800;
const unsigned int HEIGHT = 600;

const int BUFFER_SIZE = 512;

int success;
char infoLog[BUFFER_SIZE];

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

const char* vertexShaderSource =
"#version 330 core\n"
"layout(location = 0) in vec3 aPos;\n"
"void main()\n"
"{\n"
"	gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
"}\0";

const char* fragmentShaderSource =
"#version 330 core\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"	FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
"}\0";

int main()
{
	//init glfw
	glfwInit();

	//set opongl version 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

	//use core profile
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	//fix compilation on OS X
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	//create a window
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "OpenGL", NULL, NULL);
	if (!window)
	{
		std::cout << "failed to create window" << std::endl;
		//call before exit to restore resources
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	//register the callback func for changing window size
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	/*
	*func can be registered as callback funcs
	*the process should be done after creating windows and before starting rendering loop
	*/


	//init glad to manage opengl func ptr
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "failed to init glad" << std::endl;
		return -1;
	}


	//start to build shader program

	//vertex shader
	int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	//shader id, source number, source code, xx?
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);
	//check shader compile result
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertexShader, BUFFER_SIZE, NULL, infoLog);
		std::cout << "failed to compile vertex shader: " << infoLog << std::endl;
	}

	//fragment shader
	int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);
	//check compile result
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShader, BUFFER_SIZE, NULL, infoLog);
		std::cout << "failed to compile fragment shader: " << infoLog << std::endl;
	}

	//link shaders
	int shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);
	//check link result
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(shaderProgram, BUFFER_SIZE, NULL, infoLog);
		std::cout << "failed to link shaders: " << infoLog << std::endl;
	}
	//after link, the shader can be released
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	//set up vertices
	float vertices[] =
	{
		-0.5f, 0.5f, 0.0f, //lt
		-0.5f, -0.5f, 0.0f, //lb
		0.5f, 0.5f, 0.0f, //rt
		0.5f, -0.5f, 0.0f, //rb
	};
	//set up indices
	unsigned int indices[] =
	{
		0, 1, 2,
		1, 2, 3
	};


	//generate and bind VAO VBO EBO
	unsigned int VBO, VAO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

	//send data to buffer
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	//configure vertex attribute
	//location = 0, vec3 is 3, type is float, no need to normalize, stride, offset with type void*
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// note that this is allowed, the call to glVertexAttribPointer registered VBO 
	//as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
	//glBindBuffer(GL_ARRAY_BUFFER, 0);

	// You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
	// VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
	//glBindVertexArray(0);

	//draw in wireframe polygons
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	//rendering loop
	//check whether the window is closed
	while (!glfwWindowShouldClose(window))
	{
		//check input
		processInput(window);

		//rendering operations
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);	//func set gl state
		glClear(GL_COLOR_BUFFER_BIT);	//func use gl state

		//draw triangle
		glUseProgram(shaderProgram);
		glBindVertexArray(VAO);
		//premitive type, start index, vertices number
		//glDrawArrays(GL_TRIANGLES, 0, 3);
		//premitive type, vertices number, indice type, offset
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		//double buffer used to avoid flicker, when output the front buffers , the back buffers are used to /render/
		glfwSwapBuffers(window);
		//check whether there are I/O events happened and handle them by callback func
		glfwPollEvents();
	}

	glfwTerminate();

	return 0;
}

/*
*make sure the viewport matches the new window dimensions
*this func will also be called on init
*/
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	//note: opengl viewport can be smaller than the glfw window
	glViewport(0, 0, width, height);
}


void processInput(GLFWwindow* window)
{
	//check  whether the esc key is pressed
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

#include "shader.h"

const unsigned int WIDTH = 800;
const unsigned int HEIGHT = 600;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);


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
	Shader shaderProgram1("shader/v.glsl", "shader/f1.glsl");
	Shader shaderProgram2("shader/v.glsl", "shader/f2.glsl");


	//set up vertices
	float vertices1[] =
	{
		-0.5f, 0.5f, 0.0f, //lt
		-0.5f, -0.5f, 0.0f, //lb
		0.5f, 0.5f, 0.0f, //rt
	};
	float vertices2[] =
	{	//position			//color
		-0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f,//lb
		0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f,//rt
		0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f//rb
	};
	//set up indices
	unsigned int indices[] =
	{
		0, 1, 2,
		1, 2, 3
	};


	//generate and bind VAO VBO EBO
	unsigned int VBO[2], VAO[2];
	glGenVertexArrays(2, VAO);
	glGenBuffers(2, VBO);

	glBindVertexArray(VAO[0]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices1), vertices1, GL_STATIC_DRAW);
	//configure vertex attribute
	//location = 0, vec3 is 3, type is float, if need to normalize, stride, offset with type void*
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	
	glBindVertexArray(VAO[1]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices2), vertices2, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	//glGenBuffers(1, &EBO);
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);


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
		shaderProgram1.use();

		float timeValue = glfwGetTime();
		float colorValue1 = sin(timeValue) / 2.0f + 0.5;
		float colorValue2 = cos(timeValue) / 2.0f + 0.5;
		shaderProgram1.set4Float("myColor", colorValue1, colorValue2, 0.0f, 1.0f);

		glBindVertexArray(VAO[0]);
		//premitive type, start index, vertices number
		glDrawArrays(GL_TRIANGLES, 0, 3);
		//premitive type, vertices number, indice type, offset
		//glDrawElements(GL_TRIANGLES, 5, GL_UNSIGNED_INT, 0);

		shaderProgram2.use();
		glBindVertexArray(VAO[1]);
		glDrawArrays(GL_TRIANGLES, 0, 3);

		//double buffer used to avoid flicker, when output the front buffers , the back buffers are used to /render/
		glfwSwapBuffers(window);
		//check whether there are I/O events happened and handle them by callback func
		glfwPollEvents();
	}

	//deallocate resources
	glDeleteVertexArrays(2, VAO);
	glDeleteBuffers(2, VBO);

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
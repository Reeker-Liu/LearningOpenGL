#include <iostream>
#include <algorithm>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "stb_image.h"

#include "shader.h"
#include "camera.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xPos, double yPos);
void scroll_callback(GLFWwindow* window, double xOffset, double yOffset);
void processInput(GLFWwindow* window);

unsigned int loadTexture(char const * path);

const unsigned int WIDTH = 600;
const unsigned int HEIGHT = 600;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

bool firstMouse = true;
float lastX = WIDTH / 2;
float lastY = HEIGHT / 2;

Camera camera(glm::vec3(0.0f, 0.0f, 3.0f), true);
glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

bool showMatrix = false;
float lastChange = 0.0f;

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

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

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


	glEnable(GL_DEPTH_TEST);

	//start to build shader program
	Shader shader("shader/vlight.glsl", "shader/flight.glsl");
	Shader lightShader("shader/vlight.glsl", "shader/f_light.glsl");


	//set up vertices
	float vertices[] = {
		// positions          // normals           // texture coords
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
	};

	//generate and bind VAO VBO EBO
	unsigned int VBO[1], VAO[2];//, EBO[1];
	glGenVertexArrays(2, VAO);
	glGenBuffers(1, VBO);
	//glGenBuffers(1, EBO);

	glBindVertexArray(VAO[0]);

	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[0]);
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	//configure vertex attribute
	//location = 0, vec3 is 3, type is float, if need to normalize, stride, offset with type void*
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);


	glBindVertexArray(VAO[1]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	shader.use();
	unsigned int diffuseMap = loadTexture("res/container2.png");
	unsigned int specularMap = loadTexture("res/container2_specular.png");
	unsigned int emissionMap = loadTexture("res/matrix.jpg");
	shader.setInt("material.diffuse", 0);
	shader.setInt("material.specular", 1);
	shader.setInt("material.emission", 2);


	//draw in wireframe polygons
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);


	//rendering loop
	//check whether the window is closed
	while (!glfwWindowShouldClose(window))
	{
		//per-frame
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		//check input
		processInput(window);

		//rendering operations
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);	//func set gl state
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	//func use gl state

		//draw 
		shader.use();
		shader.setFloat("material.shininess", 32.0f);
		shader.setVec3("viewPos", camera.position);

		shader.setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
		shader.setVec3("dirLight.ambient", 0.05f, 0.05f, 0.05f);
		shader.setVec3("dirLight.diffuse", 0.4f, 0.4f, 0.4f);
		shader.setVec3("dirLight.specular", 0.5f, 0.5f, 0.5f);

		shader.setVec3("pointLight.position", lightPos);
		shader.setVec3("pointLight.ambient", 0.05f, 0.05f, 0.05f);
		shader.setVec3("pointLight.diffuse", 0.8f, 0.8f, 0.8f);
		shader.setVec3("pointLight.specular", 1.0f, 1.0f, 1.0f);
		shader.setFloat("pointLight.constant", 1.0f);
		shader.setFloat("pointLight.linear", 0.09);
		shader.setFloat("pointLight.quadratic", 0.032);

		shader.setVec3("spotLight.position", camera.position);
		shader.setVec3("spotLight.direction", camera.front);
		shader.setVec3("spotLight.ambient", 0.05f, 0.05f, 0.05f);
		shader.setVec3("spotLight.diffuse", 0.8f, 0.8f, 0.8f);
		shader.setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
		shader.setFloat("spotLight.constant", 1.0f);
		shader.setFloat("spotLight.linear", 0.09);
		shader.setFloat("spotLight.quadratic", 0.032);
		shader.setFloat("spotLight.cutoff", glm::cos(glm::radians(10.0f)));
		shader.setFloat("spotLight.outerCutoff", glm::cos(glm::radians(15.0f)));

		glm::mat4 model(1.0f);
		model = glm::translate(model, glm::vec3(-6.0, 0.0, 0.0));
		glm::mat4 view = camera.getViewMatrix();
		shader.setMat4("view", view);
		glm::mat4 projection = glm::perspective(glm::radians(camera.zoom), (float)WIDTH / (float)HEIGHT, 0.1f, 100.0f);
		shader.setMat4("projection", projection);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, diffuseMap);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, specularMap);
		glActiveTexture(GL_TEXTURE2);
		if (showMatrix)
		{
			glBindTexture(GL_TEXTURE_2D, emissionMap);
		}
		else
		{
			glBindTexture(GL_TEXTURE_2D, 0);
		}

		for (int i = 0; i < 8; ++i)
		{
			model = glm::translate(model, glm::vec3(2.0f, 0.0, 0.0));
			shader.setMat4("model", model);
			glBindVertexArray(VAO[0]);
			glDrawArrays(GL_TRIANGLES, 0, 36);	//premitive type, start index, vertices number
		}
		
		
		lightShader.use();
		model = glm::mat4(1.0f);
		model = glm::translate(model, lightPos);
		model = glm::scale(model, glm::vec3(0.2f));

		lightShader.setMat4("model", model);
		lightShader.setMat4("view", view);
		lightShader.setMat4("projection", projection);

		glBindVertexArray(VAO[1]);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		
		//glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);	//premitive type, vertices number, indice type, offset


		//double buffer used to avoid flicker, when output the front buffers , the back buffers are used to /render/
		glfwSwapBuffers(window);
		//check whether there are I/O events happened and handle them by callback func
		glfwPollEvents();
	}

	//deallocate resources
	glDeleteVertexArrays(2, VAO);
	glDeleteBuffers(1, VBO);
	//glDeleteBuffers(1, EBO);

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

	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
	{
	}

	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
	{
	}

	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
	{
	}

	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
	{
	}

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		camera.processKeyBoard(move_t::FORWARD, deltaTime);
	}

	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		camera.processKeyBoard(move_t::BACKWARD, deltaTime);
	}

	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		camera.processKeyBoard(move_t::RIGHT, deltaTime);
	}

	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		camera.processKeyBoard(move_t::LEFT, deltaTime);
	}

	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
	{
		std::cout << "press q" << std::endl;
		float current = glfwGetTime();
		if (current - lastChange > 0.5)
		{
			showMatrix = !showMatrix;
			lastChange = current;
		}
	}
}

void mouse_callback(GLFWwindow* window, double xPos, double yPos)
{
	if (firstMouse)
	{
		lastX = xPos;
		lastY = yPos;
		firstMouse = false;
	}

	float xOffset = xPos - lastX;
	float yOffset = lastY - yPos;
	lastX = xPos;
	lastY = yPos;

	camera.processMouseMovement(xOffset, yOffset);
}

void scroll_callback(GLFWwindow* window, double xOffset, double yOffset)
{
	camera.processMouseScroll(yOffset);
}

unsigned int loadTexture(char const * path)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}
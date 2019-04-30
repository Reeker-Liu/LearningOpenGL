#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class Shader
{
public:
	unsigned int ID;

	//generate shader program with specified shader files
	Shader(const char* vertexFilePath, const char* fragmentFilePath);

	//activate the shader program
	void use() { glUseProgram(ID); }

	void setBool(const std::string &name, bool value) const
	{
		setInt(name, (int)value);
	}

	void setInt(const std::string &name, int value) const
	{
		glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
	}

	void setFloat(const std::string &name, float value) const
	{
		glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
	}

	void set4Float(const std::string &name, float value1, float value2, float value3, float value4)
	{
		glUniform4f(glGetUniformLocation(ID, name.c_str()), value1, value2, value3, value4);
	}

	void setVec3(const std::string &name, float x, float y, float z)
	{
		glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z);
	}

	void setVec3(const std::string &name, glm::vec3 vec)
	{
		setVec3(name, vec.x, vec.y, vec.z);
	}

	void setMat4(const std::string &name, glm::mat4 data)
	{
		glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(data));
	}


private:
	//check shader compilation/linking errors
	void checkCompileError(unsigned int shader);
	void checkLinkError(unsigned int program);
};
#pragma once

#include <glad/glad.h>

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

private:
	//check shader compilation/linking errors
	void checkCompileError(unsigned int shader);
	void checkLinkError(unsigned int program);
};
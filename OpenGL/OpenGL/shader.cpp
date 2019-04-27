#include "shader.h"

Shader::Shader(const char* vertexFilePath, const char* fragmentFilePath)
{
	//fetch source code from files
	std::string vertexSource, fragmentSource;
	std::ifstream vertexFile, fragmentFile;
	vertexFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	fragmentFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

	try
	{
		vertexFile.open(vertexFilePath);
		fragmentFile.open(fragmentFilePath);
		std::stringstream vertexStream, fragmentStream;
		vertexStream << vertexFile.rdbuf();
		fragmentStream << fragmentFile.rdbuf();
		vertexFile.close();
		fragmentFile.close();
		vertexSource = vertexStream.str();
		fragmentSource = fragmentStream.str();
	}
	catch (std::ifstream::failure e)
	{
		std::cout << "failed to read shader file" << std::endl;
	}

	const char* vSource = vertexSource.c_str();
	const char* fSource = fragmentSource.c_str();
	unsigned int vertex, fragment;

	vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &vSource, NULL);
	glCompileShader(vertex);
	checkCompileError(vertex);

	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &fSource, NULL);
	glCompileShader(fragment);
	checkCompileError(fragment);

	ID = glCreateProgram();
	glAttachShader(ID, vertex);
	glAttachShader(ID, fragment);
	glLinkProgram(ID);
	checkLinkError(ID);

	glDeleteShader(vertex);
	glDeleteShader(fragment);
}

void Shader::checkCompileError(unsigned int shader)
{
	int success;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		const int BUFFER_SIZE = 1024;
		char infoLog[BUFFER_SIZE];
		glGetShaderInfoLog(shader, BUFFER_SIZE, NULL, infoLog);
		std::cout << "compile error: " << infoLog <<
			"\n -------------------------" << std::endl;
	}
}

void Shader::checkLinkError(unsigned int program)
{
	int success;
	glGetProgramiv(program, GL_LINK_STATUS, &success);
	if (!success)
	{
		const int BUFFER_SIZE = 1024;
		char infoLog[BUFFER_SIZE];
		glGetProgramInfoLog(program, BUFFER_SIZE, NULL, infoLog);
		std::cout << "link error: " << infoLog <<
			"\n -------------------------" << std::endl;
	}
}
#pragma once

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "shader.h"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>

using std::vector;
using std::string;

struct Vertex {
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec3 texCoord;
	glm::vec3 tangent;
	glm::vec3 bitangent;
};

enum class texture_t_t { DIFFUSE, SPECULAR, NORMAL, HEIGHT };
struct Texture {
	unsigned int id;
	texture_t_t texture_t;
	string path;
};

class Mesh {
public:
	vector<Vertex> vertices;
	vector<unsigned int> indices;
	vector<Texture> textures;
	unsigned int VAO;

	Mesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture> textures)
	{
		this->vertices = vertices;
		this->indices = indices;
		this->textures = textures;

		setupMesh();
	}

	void Draw(Shader shader)
	{
		unsigned int diffuseN = 0;
		unsigned int specularN = 0;
		unsigned int normalN = 0;
		unsigned int heightN = 0;
		int size = textures.size();
		for (int i = 0; i < size; ++i)
		{
			glActiveTexture(GL_TEXTURE0 + i);
			int number;
			string name;
			texture_t_t texture_t = textures[i].texture_t;

			switch (texture_t)
			{
			case texture_t_t::DIFFUSE:
				number = ++diffuseN;
				name = "texture_diffuse";
				break;
			case texture_t_t::SPECULAR:
				number = ++specularN;
				name = "texture_specular";
				break;
			case texture_t_t::NORMAL:
				number = ++normalN;
				name = "texture_normal";
				break;
			case texture_t_t::HEIGHT:
				number = ++heightN;
				name = "texture_height";
				break;
			default:
				break;
			}

			name = (name + std::to_string(number));
			glUniform1i(glGetUniformLocation(shader.ID, name.c_str()), i);
			glBindTexture(GL_TEXTURE_2D, textures[i].id);
		}

		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

		glBindVertexArray(0);
		glActiveTexture(GL_TEXTURE0);
	}
	

private:
	unsigned int VBO, EBO;

	void setupMesh()
	{
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &EBO);

		glBindVertexArray(VAO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
		glEnableVertexAttribArray(0);

		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
		glEnableVertexAttribArray(1);

		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoord));
		glEnableVertexAttribArray(2);

		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tangent));
		glEnableVertexAttribArray(3);

		glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, bitangent));
		glEnableVertexAttribArray(4);

		glBindVertexArray(0);
	}
};
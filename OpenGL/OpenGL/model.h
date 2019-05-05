#pragma once

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "stb_image.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "shader.h"
#include "mesh.h"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>

using std::vector;
using std::string;
using std::cout;
using std::endl;

unsigned int loadTexture(const string &path, const string &directory, bool gamma = false);

class Model
{
public:
	vector<Texture> textures_loaded;
	vector<Mesh> meshes;
	string directory;
	bool gammaCorrection;

	Model(const string &path, bool gamma = false) :gammaCorrection(gamma)
	{
		loadModel(path);
	}

	void draw(Shader &shader)
	{
		for (size_t i = 0; i < meshes.size(); ++i)
			meshes[i].draw(shader);
	}

private:
	void loadModel(const string &path)
	{
		Assimp::Importer importer;
		const aiScene *scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
		{
			cout << "failed to load model : " << importer.GetErrorString() << endl;
			return;
		}
		directory = path.substr(0, path.find_last_of("/"));
		cout << "model path : " << path << endl;
		cout << "directory : " << directory << endl;
		processNode(scene->mRootNode, scene);
	}

	void processNode(aiNode *node, const aiScene *scene)
	{
		cout << "start process node - " << node->mName.C_Str() << endl;
		for (size_t i = 0; i < node->mNumMeshes; ++i)
		{
			aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
			meshes.push_back(processMesh(mesh, scene));
		}
		for (size_t i = 0; i < node->mNumChildren; ++i)
		{
			processNode(node->mChildren[i], scene);
		}
		cout << "end process node - " << node->mName.C_Str() << endl;
	}

	Mesh processMesh(aiMesh *mesh, const aiScene *scene)
	{
		vector<Vertex> vertices;
		vector<unsigned int> indices;
		vector<Texture> textures;

		for (size_t i = 0; i < mesh->mNumVertices; ++i)
		{
			Vertex vertex;
			glm::vec3 v3;

			v3.x = mesh->mVertices[i].x;
			v3.y = mesh->mVertices[i].y;
			v3.z = mesh->mVertices[i].z;
			vertex.position = v3;

			v3.x = mesh->mNormals[i].x;
			v3.y = mesh->mNormals[i].y;
			v3.z = mesh->mNormals[i].z;
			vertex.normal = v3;

			if (mesh->mTextureCoords[0])
			{
				glm::vec2 v2;
				v2.x = mesh->mTextureCoords[0][i].x;
				v2.y = mesh->mTextureCoords[0][i].y;
				vertex.texCoord = v2;
			}
			else
				vertex.texCoord = glm::vec2(0.0f);

			v3.x = mesh->mTangents[i].x;
			v3.y = mesh->mTangents[i].y;
			v3.z = mesh->mTangents[i].z;
			vertex.tangent = v3;

			v3.x = mesh->mBitangents[i].x;
			v3.y = mesh->mBitangents[i].y;
			v3.z = mesh->mBitangents[i].z;
			vertex.bitangent = v3;

			vertices.push_back(vertex);
		}

		for (size_t i = 0; i < mesh->mNumFaces; ++i)
		{
			aiFace face = mesh->mFaces[i];
			for (size_t j = 0; j < face.mNumIndices; ++j)
				indices.push_back(face.mIndices[j]);
		}

		aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];

		vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, texture_t_t::DIFFUSE);
		textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

		vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, texture_t_t::SPECULAR);
		textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());

		//code wrong?
		vector<Texture> normalMaps = loadMaterialTextures(material, aiTextureType_NORMALS, texture_t_t::NORMAL);
		textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());

		vector<Texture> heightMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, texture_t_t::HEIGHT);
		textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());

		return Mesh(vertices, indices, textures);
	}

	vector<Texture> loadMaterialTextures(aiMaterial *material, aiTextureType type, texture_t_t texture_t)
	{
		vector<Texture> textures;
		for (size_t i = 0; i < material->GetTextureCount(type); ++i)
		{
			aiString str;
			material->GetTexture(type, i, &str);
			bool skip = false;
			for (unsigned int j = 0; j < textures_loaded.size(); ++j)
			{
				if (std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0)
				{
					textures.push_back(textures_loaded[j]);
					skip = true;
					break;
				}
			}
			if (!skip)
			{
				Texture texture;
				texture.id = loadTexture(str.C_Str(), this->directory);
				texture.texture_t = texture_t;
				texture.path = str.C_Str();
				textures.push_back(texture);
				textures_loaded.push_back(texture);
			}
		}
		return textures;
	}
};

unsigned int loadTexture(const string &path, const string &directory, bool gamma)
{
	string filename = directory + "/" + path;

	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char *data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
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

		cout << "finish loading texture from " << path << endl;
	}
	else
	{
		cout << "failed to load texture from " << path << endl;
	}

	stbi_image_free(data);
	return textureID;
}
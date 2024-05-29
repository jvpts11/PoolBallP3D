/*
 * Discipline: 3D Programming
 * Subject: Pool Balls 3D Project
 * Authors: João Tavares nº 21871, Diogo Silva nº 22369, Ademar Valente nº 23155, José Lourenço nº23496
 * Date: 17/05/2024
 * File: PoolLib.cpp
 * Description: Implementation of the PoolLib class.
 */

#pragma region Imports

#pragma comment(lib, "glew32s.lib")
#pragma comment(lib, "glfw3.lib")
#pragma comment(lib, "opengl32.lib")

#include <iostream>
#include <vector>
#include <string>
#include <fstream>

#define GLEW_STATIC
#include <GL\glew.h>

#define GLFW_USE_DWM_SWAP_INTERVAL
#include <GLFW\glfw3.h>

#include <glm\glm.hpp>
#include <glm\gtc\type_ptr.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\matrix_inverse.hpp>

#define TINYOBJLOADER_IMPLEMENTATION
#include "thirdParty/TinyObjLoader.h"

#define STB_IMAGE_IMPLEMENTATION
#include "thirdParty/StbImage.h"

#include "PoolLib.h"

#pragma endregion

namespace PoolLib
{
#pragma region Global Variables

	GLuint _shaderProgram;
	glm::mat4 _modelMatrix;
	glm::mat4 _viewMatrix;
	glm::mat4 _projectionMatrix;
	glm::mat3 _normalMatrix;

#pragma endregion

	void RenderBalls::Load(const std::string obj_model_filepath)
	{
		std::vector<float> vertices;
		
		tinyobj::attrib_t attrib;
		std::vector<tinyobj::shape_t> shapes;
		std::vector<tinyobj::material_t> materials;
		std::string warn, err;

		if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, obj_model_filepath.c_str()))
		{
			std::cout << warn << err << std::endl;
		}

		for (const auto& shape : shapes)
		{
			for (const auto& index : shape.mesh.indices)
			{
				// atributtes positions
				//vertices.push_back(attrib.vertices[3 * index.vertex_index + 0]);
				//vertices.push_back(attrib.vertices[3 * index.vertex_index + 1]);
				//vertices.push_back(attrib.vertices[3 * index.vertex_index + 2]);

				//// atributtes normals
				//vertices.push_back(attrib.normals[3 * index.normal_index + 0]);
				//vertices.push_back(attrib.normals[3 * index.normal_index + 1]);
				//vertices.push_back(attrib.normals[3 * index.normal_index + 2]);

				//// atributtes texture coordinates
				//vertices.push_back(attrib.texcoords[2 * index.texcoord_index + 0]);
				//vertices.push_back(1.0f - attrib.texcoords[2 * index.texcoord_index + 1]);

				glm::vec3 position = glm::vec3(
					attrib.vertices[3 * index.vertex_index + 0], 
					attrib.vertices[3 * index.vertex_index + 1], 
					attrib.vertices[3 * index.vertex_index + 2]);

				glm::vec3 normal = glm::vec3(
					attrib.normals[3 * index.normal_index + 0],
					attrib.normals[3 * index.normal_index + 1],
					attrib.normals[3 * index.normal_index + 2]);

				glm::vec2 texCoord = glm::vec2(
					attrib.texcoords[2 * index.texcoord_index + 0],
					1.0f - attrib.texcoords[2 * index.texcoord_index + 1]);

				vertices.push_back(position.x);
				vertices.push_back(position.y);
				vertices.push_back(position.z);

				vertices.push_back(normal.x);
				vertices.push_back(normal.y);
				vertices.push_back(normal.z);

				vertices.push_back(texCoord.x);
				vertices.push_back(texCoord.y);

			}
		}

		// store 3D model vertices
		_ballVertices.push_back(vertices);

		// store 3D model material
		std::string mtlFile = getMtlFromObj(obj_model_filepath.c_str());

		_materials.push_back(loadMaterial(mtlFile.c_str()));

		int lastIndex = _materials.size() - 1;

		// store 3D model texture
		std::string textureFile = _materials[lastIndex].map_kd;

		_textures.push_back(loadTexture(textureFile));
	}

	void RenderBalls::Install(void) {

		glGenVertexArrays(1, _ballVAO);
		glBindVertexArray(_ballVAO[0]);

		glGenBuffers(_numBalls, _ballVBOs);

		for (int i = 0; i < _numBalls; i++)
		{
			glBindBuffer(GL_ARRAY_BUFFER, _ballVBOs[i]);
			glBufferData(GL_ARRAY_BUFFER, _ballVertices[i].size() * sizeof(float), _ballVertices[i].data(), GL_STATIC_DRAW);

			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
			glEnableVertexAttribArray(0);

			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
			glEnableVertexAttribArray(1);

			glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
			glEnableVertexAttribArray(2);

			glBindVertexArray(0);
		}

		std::vector<GLuint> textNames (_numBalls);
		glGenTextures(_numBalls, textNames.data());

		for (int i = 0; i < _numBalls; i++)
		{
			glActiveTexture(GL_TEXTURE0 + i);
			glBindTexture(GL_TEXTURE_2D, textNames[i]);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, _textures[i].width, _textures[i].height, 0, _textures[i].nChannels == 4 ? GL_RGBA: GL_RGB, GL_UNSIGNED_BYTE, _textures[i].image);
			glGenerateMipmap(GL_TEXTURE_2D);
		}
	}

	void RenderBalls::Render(glm::vec3 position, glm::vec3 orientation)
	{
		glm::mat4 translatedModel = glm::translate(_modelMatrix, position);

		glm::mat4 rotatedModel = glm::rotate(translatedModel, glm::radians(orientation.x), glm::vec3(1.0f, 0.0f, 0.0f));
		rotatedModel = glm::rotate(rotatedModel, glm::radians(orientation.y), glm::vec3(0.0f, 1.0f, 0.0f));
		rotatedModel = glm::rotate(rotatedModel, glm::radians(orientation.z), glm::vec3(0.0f, 0.0f, 1.0f));

		glm::mat4 scaledModel = glm::scale(rotatedModel, glm::vec3(0.08f));

		glm::mat4 modelViewMatrix = _viewMatrix * scaledModel;

		GLint modelViewID = glGetUniformLocation(_shaderProgram, "ModelView");

		glProgramUniformMatrix4fv(_shaderProgram, modelViewID, 1, GL_FALSE, glm::value_ptr(modelViewMatrix));

		GLint texSamplerID = glGetUniformLocation(_shaderProgram, "sampler");
		glProgramUniform1i(_shaderProgram, texSamplerID, _id);
		
		GLint renderTex = glGetUniformLocation(_shaderProgram, "renderTex");
		glProgramUniform1i(_shaderProgram, renderTex, 1);


		loadMaterialUniforms(getMaterials()[_id], _shaderProgram);

		glBindVertexArray(0);
		glDrawArrays(GL_TRIANGLES, 0, getBallVertices()[_id].size() / 8);

	}

	std::string RenderBalls::getMtlFromObj(const char* obj_filepath) {

		std::string mtlFile;
		std::ifstream objFile(obj_filepath);
		std::string line;

		while (std::getline(objFile, line))
		{
			std::istringstream iss(line);
			std::string prefix;
			iss >> prefix;

			if (prefix == "mtllib")
			{
				iss >> mtlFile;
				break;
			}
		}
		return mtlFile;
	}

	Material RenderBalls::loadMaterial(const char* mtl_filepath)
	{
		std::string directory = "textures/";
		std::ifstream mtlFile(directory + mtl_filepath);
		if (!mtlFile)
		{
			std::cerr << "Error: Could not open file " << mtl_filepath << std::endl;
			return {};
		}
		std::string line;
		std::string materialName;

		PoolLib::Material material;
		while (std::getline(mtlFile, line))
		{
			if (line.empty() || line[0] == '#')
			{
				continue;
			}
			std::istringstream iss(line);
			std::string command;
			iss >> command;

			if (command == "Ns")
			{
				float value;
				iss >> value;
				material.ns = value;
			}
			else if (command == "Ka")
			{
				float r, g, b;
				iss >> r >> g >> b;
				material.ka = glm::vec3(r, g, b);
			}
			else if (command == "Kd")
			{
				float r, g, b;
				iss >> r >> g >> b;
				material.kd = glm::vec3(r, g, b);
			}
			else if (command == "Ks")
			{
				float r, g, b;
				iss >> r >> g >> b;
				material.ks = glm::vec3(r, g, b);
			}
			else if (command == "map_Kd")
			{
				std::string textureFile;
				iss >> textureFile;
				material.map_kd = textureFile;
			}
		}
		return material;
	}
	Texture RenderBalls::loadTexture(std::string texture_filepath)
	{
		Texture texture {};
		int width, height, nChannels;
		std::string directory = "textures/";
		std::string fullPath = directory + texture_filepath;
		unsigned char* image = stbi_load(fullPath.c_str(), &width, &height, &nChannels, 0);
		if (!image)
		{
			std::cerr << "Error: Could not load texture " << texture_filepath << std::endl;
			return {};
		}
		unsigned char* imageCopy = new unsigned char[width * height * nChannels];
		std::memcpy(imageCopy, image, width * height * nChannels);

		texture.width = width;
		texture.height = height;
		texture.nChannels = nChannels;
		texture.image = imageCopy;

		stbi_image_free(image);
		return texture;
	}
	void RenderBalls::loadMaterialUniforms(Material material, GLuint programShader) {
		glProgramUniform3fv(programShader, glGetProgramResourceLocation(programShader, GL_UNIFORM, "material.ambient"), 1, glm::value_ptr(material.ka));
		glProgramUniform3fv(programShader, glGetProgramResourceLocation(programShader, GL_UNIFORM, "material.diffuse"), 1, glm::value_ptr(material.kd));
		glProgramUniform3fv(programShader, glGetProgramResourceLocation(programShader, GL_UNIFORM, "material.specular"), 1, glm::value_ptr(material.ks));
		glProgramUniform1f(programShader, glGetProgramResourceLocation(programShader, GL_UNIFORM, "material.shininess"), material.ns);
	}

	void RenderBalls::loadLightingUniforms() {
		// ambient light source
		glProgramUniform3fv(_shaderProgram, glGetProgramResourceLocation(_shaderProgram, GL_UNIFORM, "ambientLight.ambient"), 1, glm::value_ptr(glm::vec3(7.0f)));

		// direccional light source
		glProgramUniform3fv(_shaderProgram, glGetProgramResourceLocation(_shaderProgram, GL_UNIFORM, "directionalLight.direction"), 1, glm::value_ptr(glm::vec3(0.5f, -0.5f, 0.0f)));
		glProgramUniform3fv(_shaderProgram, glGetProgramResourceLocation(_shaderProgram, GL_UNIFORM, "directionalLight.ambient"), 1, glm::value_ptr(glm::vec3(4.0f)));
		glProgramUniform3fv(_shaderProgram, glGetProgramResourceLocation(_shaderProgram, GL_UNIFORM, "directionalLight.diffuse"), 1, glm::value_ptr(glm::vec3(2.0f)));
		glProgramUniform3fv(_shaderProgram, glGetProgramResourceLocation(_shaderProgram, GL_UNIFORM, "directionalLight.specular"), 1, glm::value_ptr(glm::vec3(1.0f)));

		// fonte de luz pontual 
		glProgramUniform3fv(_shaderProgram, glGetProgramResourceLocation(_shaderProgram, GL_UNIFORM, "pointLight.position"), 1, glm::value_ptr(glm::vec3(1.0f, 1.0f, 0.0f)));
		glProgramUniform3fv(_shaderProgram, glGetProgramResourceLocation(_shaderProgram, GL_UNIFORM, "pointLight.ambient"), 1, glm::value_ptr(glm::vec3(0.6f)));
		glProgramUniform3fv(_shaderProgram, glGetProgramResourceLocation(_shaderProgram, GL_UNIFORM, "pointLight.diffuse"), 1, glm::value_ptr(glm::vec3(2.0f)));
		glProgramUniform3fv(_shaderProgram, glGetProgramResourceLocation(_shaderProgram, GL_UNIFORM, "pointLight.specular"), 1, glm::value_ptr(glm::vec3(1.0f)));
		glProgramUniform1f(_shaderProgram, glGetProgramResourceLocation(_shaderProgram, GL_UNIFORM, "pointLight.constant"), 1.0f);
		glProgramUniform1f(_shaderProgram, glGetProgramResourceLocation(_shaderProgram, GL_UNIFORM, "pointLight.linear"), 0.06f);
		glProgramUniform1f(_shaderProgram, glGetProgramResourceLocation(_shaderProgram, GL_UNIFORM, "pointLight.quadratic"), 0.02f);

		// fonte de luz conica
		glProgramUniform3fv(_shaderProgram, glGetProgramResourceLocation(_shaderProgram, GL_UNIFORM, "spotLight.position"), 1, glm::value_ptr(glm::vec3(0.0f, 2.5f, 0.0f)));
		glProgramUniform3fv(_shaderProgram, glGetProgramResourceLocation(_shaderProgram, GL_UNIFORM, "spotLight.direction"), 1, glm::value_ptr(glm::vec3(0.0f, -0.1f, 0.0f)));
		glProgramUniform3fv(_shaderProgram, glGetProgramResourceLocation(_shaderProgram, GL_UNIFORM, "spotLight.ambient"), 1, glm::value_ptr(glm::vec3(5.0f)));
		glProgramUniform3fv(_shaderProgram, glGetProgramResourceLocation(_shaderProgram, GL_UNIFORM, "spotLight.diffuse"), 1, glm::value_ptr(glm::vec3(1.0f)));
		glProgramUniform3fv(_shaderProgram, glGetProgramResourceLocation(_shaderProgram, GL_UNIFORM, "spotLight.specular"), 1, glm::value_ptr(glm::vec3(1.0f)));
		glProgramUniform1f(_shaderProgram, glGetProgramResourceLocation(_shaderProgram, GL_UNIFORM, "spotLight.constant"), 1.0f);
		glProgramUniform1f(_shaderProgram, glGetProgramResourceLocation(_shaderProgram, GL_UNIFORM, "spotLight.linear"), 0.09f);
		glProgramUniform1f(_shaderProgram, glGetProgramResourceLocation(_shaderProgram, GL_UNIFORM, "spotLight.quadratic"), 0.032f);
		glProgramUniform1f(_shaderProgram, glGetProgramResourceLocation(_shaderProgram, GL_UNIFORM, "spotLight.cutoff"), glm::cos(glm::radians(15.0f)));
		glProgramUniform1f(_shaderProgram, glGetProgramResourceLocation(_shaderProgram, GL_UNIFORM, "spotLight.outerCutOff"), glm::cos(glm::radians(20.0f)));

		glProgramUniform1i(_shaderProgram, glGetProgramResourceLocation(_shaderProgram, GL_UNIFORM, "lightModel"), 1);
	}
}

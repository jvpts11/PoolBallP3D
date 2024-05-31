/*
 * Discipline: 3D Programming
 * Subject: Pool Balls 3D Project
 * Authors: João Tavares nº 21871, Diogo Silva nº 22369, Ademar Valente nº 23155, José Lourenço nº23496
 * Date: 31/05/2024
 * File: PoolLib.cpp
 * Description: Implementation of the PoolLib library.
 */
//-----------------------------------------------------------------------------------------------------
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
// -----------------------------------------------------------------------------------------------------
#pragma region Library Implementation

namespace PoolLib
{
	#pragma region Global Variables

	GLuint _shaderProgram;
	glm::mat4 _modelMatrix;
	glm::mat4 _viewMatrix;
	glm::mat4 _projectionMatrix;
	glm::mat3 _normalMatrix;
	float _ballRadius = 0.08f;

	#pragma endregion
// -----------------------------------------------------------------------------------------------------
	#pragma region PoolLib Class Main Functions

	// Function that will run for each ball
	void RenderBalls::Load(const std::string obj_model_filepath) {

		std::vector<float> vertices;
		
		// On this projet we are using the tinyobjloader library to load the 3D models
		tinyobj::attrib_t attrib;
		std::vector<tinyobj::shape_t> shapes;
		std::vector<tinyobj::material_t> materials;
		std::string warn, err;

		// Load the 3D model and check for failures
		if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, obj_model_filepath.c_str()))
		{
			std::cout << warn << err << std::endl;
		}

		// Store 3D model vertice components
		for (const auto& shape : shapes)
		{
			for (const auto& index : shape.mesh.indices)
			{
				// Position
				glm::vec3 position = glm::vec3(
					attrib.vertices[3 * index.vertex_index + 0], 
					attrib.vertices[3 * index.vertex_index + 1], 
					attrib.vertices[3 * index.vertex_index + 2]);

				// Normal
				glm::vec3 normal = glm::vec3(
					attrib.normals[3 * index.normal_index + 0],
					attrib.normals[3 * index.normal_index + 1],
					attrib.normals[3 * index.normal_index + 2]);

				// Texture Coordinates
				glm::vec2 texCoord = glm::vec2(
					attrib.texcoords[2 * index.texcoord_index + 0],
					1.0f - attrib.texcoords[2 * index.texcoord_index + 1]);		// 1.0f - y to flip the texture to the correct orientation

				// Store the 3D model vertices in the tinyobjloader format
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

		// Store 3D model vertices
		_ballVertices.push_back(vertices);

		// Get the material file from the 3D model filepath
		std::string mtlFile = getMtlFromObj(obj_model_filepath.c_str());

		// Store 3D model material
		_materials.push_back(loadMaterial(mtlFile.c_str()));

		// Get the index of the last element of the materials vector
		int lastIndex = _materials.size() - 1;

		// Load the texture file from the material
		std::string textureFile = _materials[lastIndex].map_kd;

		// Store the 3D model texture
		_textures.push_back(loadTexture(textureFile));
	}

	// Function that will once in the process of the ball's creation
	void RenderBalls::Install(void) {

		// Generate the name if the VAO and link it to the OpenGl context
		glGenVertexArrays(1, _ballVAO);
		glBindVertexArray(_ballVAO[0]);

		// Generate the name of the VBOs
		glGenBuffers(_numBalls, _ballVBOs);

		// For each ball
		for (int i = 0; i < _numBalls; i++)
		{
			// Bind the current VBO to the OpenGl context
			glBindBuffer(GL_ARRAY_BUFFER, _ballVBOs[i]);

			// Copy the vertices data to the current VBO
			glBufferData(GL_ARRAY_BUFFER, _ballVertices[i].size() * sizeof(float), _ballVertices[i].data(), GL_STATIC_DRAW);

			// Set the vertex attributes pointers
			// Position
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
			glEnableVertexAttribArray(0);
			// Normal
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
			glEnableVertexAttribArray(1);
			// Texture Coordinates
			glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
			glEnableVertexAttribArray(2);

			// Unbind the current VBO
			glBindVertexArray(0);
		}

		// Generate the name of the textures
		std::vector<GLuint> textNames (_numBalls);
		glGenTextures(_numBalls, textNames.data());

		// For each ball
		for (int i = 0; i < _numBalls; i++)
		{
			// Activate the current texture
			glActiveTexture(GL_TEXTURE0 + i);

			// Bind the current texture to the OpenGl context
			glBindTexture(GL_TEXTURE_2D, textNames[i]);

			// Set the texture parameters
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

			// Copy the texture data to the current texture
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, _textures[i].width, _textures[i].height, 0, _textures[i].nChannels == 4 ? GL_RGBA: GL_RGB, GL_UNSIGNED_BYTE, _textures[i].image);

			// Generate the mipmaps
			glGenerateMipmap(GL_TEXTURE_2D);
		}
	}

	// Function that will run for each ball
	void RenderBalls::Render(glm::vec3 position, glm::vec3 orientation) {

		// Create a translation matrix for the ball
		glm::mat4 translatedModel = glm::translate(_modelMatrix, position);

		// Create a rotation matrix for the ball
		glm::mat4 rotatedModel = glm::rotate(translatedModel, glm::radians(orientation.x), glm::vec3(1.0f, 0.0f, 0.0f));
		rotatedModel = glm::rotate(rotatedModel, glm::radians(orientation.y), glm::vec3(0.0f, 1.0f, 0.0f));
		rotatedModel = glm::rotate(rotatedModel, glm::radians(orientation.z), glm::vec3(0.0f, 0.0f, 1.0f));

		// Create the scaled model matrix for the ball
		glm::mat4 scaledModel = glm::scale(rotatedModel, glm::vec3(_ballRadius));

		// Create the model view matrix for the ball, using the view matrix and the scaled model matrix
		glm::mat4 modelViewMatrix = _viewMatrix * scaledModel;


		// Get the model view matrix uniform location
		GLint modelViewID = glGetUniformLocation(_shaderProgram, "ModelView");

		// Set the model view matrix uniform with the model view matrix value
		glProgramUniformMatrix4fv(_shaderProgram, modelViewID, 1, GL_FALSE, glm::value_ptr(modelViewMatrix));


		// Get the texture sampler uniform location
		GLint texSamplerID = glGetUniformLocation(_shaderProgram, "sampler");

		// Set the texture sampler uniform with the current ball id texture value
		glProgramUniform1i(_shaderProgram, texSamplerID, _id);

		// Get the render matrix uniform location
		GLint renderTex = glGetUniformLocation(_shaderProgram, "renderTex");

		// Set render texture uniform value to shader program with index 1
		glProgramUniform1i(_shaderProgram, renderTex, 1);

		// Load material uniforms for the current ball
		loadMaterialUniforms(getMaterials()[_id], _shaderProgram);

		// Unbind the current VAO
		glBindVertexArray(0);

		// Draw the current ball
		glDrawArrays(GL_TRIANGLES, 0, getBallVertices()[_id].size() / 8);

	}

	#pragma endregion
// -----------------------------------------------------------------------------------------------------
	#pragma region PoolLib Class Secondary Functions

	std::string RenderBalls::getMtlFromObj(const char* obj_filepath) {

		std::string mtlFile;
		std::ifstream objFile(obj_filepath);
		std::string line;

		// Read the obj file line by line
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

	Material RenderBalls::loadMaterial(const char* mtl_filepath) {
		// Original directory of the textures
		std::string directory = "textures/";

		// Open the material file with the given filepath
		std::ifstream mtlFile(directory + mtl_filepath);

		// Check if the file was opened successfully
		if (!mtlFile)
		{
			std::cerr << "Error: Could not open file " << mtl_filepath << "." << std::endl;
			return {};
		}

		std::string line;
		std::string materialName;
		PoolLib::Material material;

		// Read the material file line by line
		while (std::getline(mtlFile, line))
		{
			// Check if the line is empty or a comment
			if (line.empty() || line[0] == '#')
			{
				continue;
			}

			std::istringstream iss(line);
			std::string command;
			iss >> command;

			// Check the command of the line for the material properties: Shininess
			if (command == "Ns")
			{
				float value;
				iss >> value;
				material.ns = value;
			}
			// Check the command of the line for the material properties: Ambient
			else if (command == "Ka")
			{
				float r, g, b;
				iss >> r >> g >> b;
				material.ka = glm::vec3(r, g, b);
			}
			// Check the command of the line for the material properties: Diffuse
			else if (command == "Kd")
			{
				float r, g, b;
				iss >> r >> g >> b;
				material.kd = glm::vec3(r, g, b);
			}
			// Check the command of the line for the material properties: Specular
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

	Texture RenderBalls::loadTexture(std::string texture_filepath) {

		// Create a varaible to store the texture data
		Texture texture {};
		int width, height, nChannels;

		// Original directory of the textures
		std::string directory = "textures/";

		// Load the texture file with the given filepath and store the image data
		std::string fullPath = directory + texture_filepath;
		unsigned char* image = stbi_load(fullPath.c_str(), &width, &height, &nChannels, 0);

		// Check if the image was loaded successfully
		if (!image)
		{
			std::cerr << "Error: Could not load texture " << texture_filepath << "." << std::endl;
			return {};
		}

		// Copy the image data to a new variable and free the original image data
		unsigned char* imageCopy = new unsigned char[width * height * nChannels];
		std::memcpy(imageCopy, image, width * height * nChannels);

		// Store the texture data in the texture variable
		texture.width = width;
		texture.height = height;
		texture.nChannels = nChannels;
		texture.image = imageCopy;

		// Free the original image data
		stbi_image_free(image);

		// Return the texture
		return texture;
	}

	void RenderBalls::loadMaterialUniforms(Material material, GLuint programShader) {
		// Set the material uniforms of each  type of light
		glProgramUniform3fv(programShader, glGetProgramResourceLocation(programShader, GL_UNIFORM, "material.ambient"), 1, glm::value_ptr(material.ka));
		glProgramUniform3fv(programShader, glGetProgramResourceLocation(programShader, GL_UNIFORM, "material.diffuse"), 1, glm::value_ptr(material.kd));
		glProgramUniform3fv(programShader, glGetProgramResourceLocation(programShader, GL_UNIFORM, "material.specular"), 1, glm::value_ptr(material.ks));
		glProgramUniform1f(programShader, glGetProgramResourceLocation(programShader, GL_UNIFORM, "material.shininess"), material.ns);
	}

	void RenderBalls::loadLightingUniforms() {
		// Ambient light source
		glProgramUniform3fv(_shaderProgram, glGetProgramResourceLocation(_shaderProgram, GL_UNIFORM, "ambientLight.ambient"), 1, glm::value_ptr(glm::vec3(7.0f)));

		// Direccional light source
		glProgramUniform3fv(_shaderProgram, glGetProgramResourceLocation(_shaderProgram, GL_UNIFORM, "directionalLight.direction"), 1, glm::value_ptr(glm::vec3(0.5f, -0.5f, 0.0f)));
		glProgramUniform3fv(_shaderProgram, glGetProgramResourceLocation(_shaderProgram, GL_UNIFORM, "directionalLight.ambient"), 1, glm::value_ptr(glm::vec3(4.0f)));
		glProgramUniform3fv(_shaderProgram, glGetProgramResourceLocation(_shaderProgram, GL_UNIFORM, "directionalLight.diffuse"), 1, glm::value_ptr(glm::vec3(2.0f)));
		glProgramUniform3fv(_shaderProgram, glGetProgramResourceLocation(_shaderProgram, GL_UNIFORM, "directionalLight.specular"), 1, glm::value_ptr(glm::vec3(1.0f)));

		// Point light source 
		glProgramUniform3fv(_shaderProgram, glGetProgramResourceLocation(_shaderProgram, GL_UNIFORM, "pointLight.position"), 1, glm::value_ptr(glm::vec3(1.0f, 1.0f, 0.0f)));
		glProgramUniform3fv(_shaderProgram, glGetProgramResourceLocation(_shaderProgram, GL_UNIFORM, "pointLight.ambient"), 1, glm::value_ptr(glm::vec3(0.6f)));
		glProgramUniform3fv(_shaderProgram, glGetProgramResourceLocation(_shaderProgram, GL_UNIFORM, "pointLight.diffuse"), 1, glm::value_ptr(glm::vec3(2.0f)));
		glProgramUniform3fv(_shaderProgram, glGetProgramResourceLocation(_shaderProgram, GL_UNIFORM, "pointLight.specular"), 1, glm::value_ptr(glm::vec3(1.0f)));
		glProgramUniform1f(_shaderProgram, glGetProgramResourceLocation(_shaderProgram, GL_UNIFORM, "pointLight.constant"), 1.0f);
		glProgramUniform1f(_shaderProgram, glGetProgramResourceLocation(_shaderProgram, GL_UNIFORM, "pointLight.linear"), 0.06f);
		glProgramUniform1f(_shaderProgram, glGetProgramResourceLocation(_shaderProgram, GL_UNIFORM, "pointLight.quadratic"), 0.02f);

		// Spot light source
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

		// Standard lightmodel (the one used in the beginning of the program)
		glProgramUniform1i(_shaderProgram, glGetProgramResourceLocation(_shaderProgram, GL_UNIFORM, "lightModel"), 0);
	}

	#pragma endregion
}

#pragma endregion
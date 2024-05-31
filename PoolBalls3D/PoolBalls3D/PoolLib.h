/*
 * Discipline: 3D Programming
 * Subject: Pool Balls 3D Project
 * Authors: João Tavares nº 21871, Diogo Silva nº 22369, Ademar Valente nº 23155, José Lourenço nº23496
 * Date: 17/05/2024
 * File: PoolLib.h
 * Description: Signatures related to the implementation of the PoolLib class.
 */
//-----------------------------------------------------------------------------------------------------
#pragma once
#include <iostream>
#include <glm\glm.hpp>

namespace PoolLib{

#pragma region Global Variables that can be accessed by external classes

	extern GLuint _shaderProgram;
	extern glm::mat4 _modelMatrix;
	extern glm::mat4 _viewMatrix;
	extern glm::mat4 _projectionMatrix;
	extern glm::mat3 _normalMatrix;
	extern float _ballRadius;

#pragma endregion
//-----------------------------------------------------------------------------------------------------
#pragma region Structs

	// Struct that contains the information of the material of the object
	typedef struct {
		float ns;
		glm::vec3 ka;
		glm::vec3 kd;
		glm::vec3 ks;

		std::string map_kd;
	}Material;

	// Struct that contains the information of the texture of the object
	typedef struct {
		int width;
		int height;
		int nChannels;
		unsigned char* image;
	}Texture;

#pragma endregion
//-----------------------------------------------------------------------------------------------------

	class RenderBalls {
	private:

		// Position and orientation of the ball
		glm::vec3 _position;
		glm::vec3 _orientation;

		// Id of the ball
		int _id;

		// Number of balls
		const GLuint _numBalls;

		// Vertices of the ball
		std::vector<std::vector<float>> _ballVertices;

		// VAO and VBOs of the balls
		GLuint* _ballVAO;
		GLuint* _ballVBOs;

		// Materials and textures of the balls
		std::vector<Material> _materials;
		std::vector<Texture> _textures;
		Material* _material;
		Texture* _texture;

		// Shader program
		GLuint _shaderProgram;

	public:

		// Getter functions
		GLuint getNumberOfBalls() const { return _numBalls; };
		const std::vector<std::vector<float>>& getBallVertices() const { return _ballVertices; };
		const std::vector<Material>& getMaterials() const { return _materials; };
		GLuint getShaderProgram() const { return _shaderProgram; };

		// Setter functions
		void setShaderProgram(GLuint shaderProgram) { _shaderProgram = shaderProgram; };
		void setId(int id) { _id = id; };

		// Constructor
		RenderBalls(GLuint numberOfBalls) : _numBalls(numberOfBalls)
		{
			_ballVAO = new GLuint[1];
			_ballVBOs = new GLuint[_numBalls];
			_shaderProgram = -1;
			_id = 0;
		};

		// Destructor
		~RenderBalls()
		{
			delete[] _ballVAO;
			delete[] _ballVBOs;
			_ballVertices.clear();
			_materials.clear();
		};

		// Main functions
		void Load(const std::string obj_model_filepath);
		void Install(void);
		void Render(glm::vec3 position, glm::vec3 orientation);

		// Secondary functions
		std::string getMtlFromObj(const char* obj_filepath);
		Material loadMaterial(const char* mtl_filepath);
		Texture loadTexture(std::string textureFile);
		void loadMaterialUniforms(Material material, GLuint programShader);
		void loadLightingUniforms();

	};

}

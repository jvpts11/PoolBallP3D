#pragma once

#ifndef POOL_BALLS_H
#define POOL_BALLS_H 1

#pragma region importações

#include <iostream>
#include <vector>

#define GLEW_STATIC
#include <GL\glew.h>

#define GLFW_USE_DWM_SWAP_INTERVAL
#include <GLFW\glfw3.h>

#include <glm\glm.hpp>
#include <glm\gtc\type_ptr.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\matrix_inverse.hpp>

#pragma endregion


namespace PoolBalls {

#pragma region declarações da biblioteca

	// estrutura para armazenados dados dos ficheiros .mtl
	typedef struct {
		float ns;				// expoente especular (brilho do objeto)
		glm::vec3 ka;			// coeficiente de reflexão da luz ambiente
		glm::vec3 kd;			// coeficiente de reflexão da luz difusa
		glm::vec3 ks;			// coeficiente de reflexão da luz especular 
		std::string map_kd;		// nome do ficheiro da imagem de textura
	} Material;

	// estrutura para armazenados dados das texturas
	typedef struct {
		int width;				// largura da textura
		int height;				// altura da textura
		int nChannels;			// número de canais de cores
		unsigned char* image;	// imagem da textura
	} Texture;

	// classe base da biblioteca
	class RendererBalls {
	private:
		// atributos
		const GLuint _numberOfBalls;
		std::vector<std::vector<float>> _ballsVertices;
		GLuint* _ballsVAOs;
		GLuint* _ballsVBOs;
		std::vector<Material> _ballsMaterials;
		std::vector<Texture> _ballsTextures;
		GLuint _programShader;
		Material* _material;
		Texture* _texture;

	public:
		// aceder atributos fora da classe - getters
		GLuint getNumberOfBalls() const {
			return _numberOfBalls;
		}

		const std::vector<std::vector<float>>& getBallsVertices() const {	// retorna apontador para ser mais eficiente no draw
			return _ballsVertices;
		}

		const std::vector<Material>& getBallsMaterials() const {
			return _ballsMaterials;
		}

		GLuint getProgramShader() const {
			return _programShader;
		}

		// aceder atributos fora da classe - setters
		void setProgramShader(GLuint programShader) {
			_programShader = programShader;
		}

		// construtor
		RendererBalls(GLuint numberOfBalls) : _numberOfBalls(numberOfBalls) {
			_ballsVAOs = new GLuint[_numberOfBalls];
			_ballsVBOs = new GLuint[_numberOfBalls];
			_programShader = -1;
		}

		// destrutor
		~RendererBalls() {
			// liberta memória
			delete[] _ballsVAOs;
			delete[] _ballsVBOs;
			_ballsVertices.clear();
			_ballsMaterials.clear();
		}

		// principais
		void Read(const std::string obj_model_filepath);
		void Install(void);
		void Render(glm::vec3 position, glm::vec3 orientation);

		// secundárias
		std::vector<float> load3dModel(const char* objFilepath);
		std::string getMtlFromObj(const char* objFilepath);
		Material loadMaterial(const char* mtlFilename);
		Texture loadTexture(std::string imageFilename);
		void loadLightingUniforms();
		void loadMaterialUniforms(Material material, GLuint programShader);
	};

#pragma endregion

}

#endif
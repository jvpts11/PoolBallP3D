/*
@3D Programming

@description File with all signatures related to the Pool library.
@file Pool.h

@authors Jo�o Tavares n�21871, Diogo Silva n�22369, Ademar Valente n�23155, Jos� louren�o n�23496
@date 19/05/2024
*/


#pragma once

#ifndef POOL_BALLS_H
#define POOL_BALLS_H 1

#pragma region importations

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


namespace Pool {

#pragma region library declarations

	// estrutura para armazenados dados dos ficheiros .mtl
	typedef struct {
		float ns;				// expoente especular (brilho do objeto)
		glm::vec3 ka;			// coeficiente de reflex�o da luz ambiente
		glm::vec3 kd;			// coeficiente de reflex�o da luz difusa
		glm::vec3 ks;			// coeficiente de reflex�o da luz especular 
		std::string map_kd;		// nome do ficheiro da imagem de textura
	} Material;

	// estrutura para armazenados dados das texturas
	typedef struct {
		int width;				// largura da textura
		int height;				// altura da textura
		int nChannels;			// n�mero de canais de cores
		unsigned char* image;	// imagem da textura
	} Texture;

	// vari�veis globais
	extern GLuint _programShader;
	extern glm::mat4 _modelMatrix;
	extern glm::mat4 _viewMatrix;
	extern glm::mat4 _projectionMatrix;
	extern glm::mat3 _normalMatrix;

	// fun��es globais da biblioteca
	void bindProgramShader(GLuint* programShader);
	void sendAttributesToProgramShader(GLuint* programShader);
	void sendUniformsToProgramShader(GLuint* programShader, glm::mat4* modelMatrix, glm::mat4* viewMatrix, glm::mat4* modelViewMatrix, glm::mat4* projectionMatrix, glm::mat3* normalMatrix);

	// classe para renderizar bolas
	class RendererBall {
	private:
		// atributos privados
		int _id;	// identificador �nico para depois saber qual a unidade de textura que pertence, entre outros dados, que este seja �til
		glm::vec3 _position;
		glm::vec3 _orientation;

		const char* _objFilepath;
		std::vector<float>* _vertices;
		GLuint* _vao;
		GLuint* _vbo;
		Material* _material;
		Texture* _texture;

	public:
		// getters - definir valores de atributos fora da classe
		const std::vector<float>& getVertices() const;
		const Material& getMaterial() const;
		glm::vec3 getPosition() const;
		glm::vec3 getOrientation() const;

		// setters - obter valores de atributos fora da classe
		void setId(int id);
		void setPosition(glm::vec3 position);
		void setOrientation(glm::vec3 orientation);

		// construtor
		RendererBall();

		// destrutor
		~RendererBall();

		// principais
		void Load(const std::string obj_model_filepath);
		void Install(void);
		void Render(glm::vec3 position, glm::vec3 orientation);

		// secund�rias
		std::vector<float>* load3dModel(const char* objFilepath);
		std::string getMtlFromObj(const char* objFilepath);
		Material* loadMaterial(const char* mtlFilename);
		Texture* loadTexture(std::string imageFilename);
		void loadMaterialLighting(GLuint programShader, Material material);
	};

#pragma endregion

}

#endif
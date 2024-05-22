#pragma region importações

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

#include "LoadShaders.h"
#include "PoolBalls.h"
#include "Source.h"

#pragma endregion


namespace PoolBalls {

#pragma region funções principais da biblioteca PoolBalls

	//Esta função tem como objetivo carregar para a memória do CPU os dados do ficheiro .obj, cujo caminho é passado como argumento.
	//Deverá ainda carregar para a memória do CPU os materiais e texturas associados ao modelo.
	void RendererBalls::Load(const std::string obj_model_filepath) {
		std::vector<float> vertices;

		tinyobj::attrib_t attributes;
		std::vector<tinyobj::shape_t> shapes;
		std::vector<tinyobj::material_t> materials;
		std::string warning, error;

		// se houve erros ao carregar o ficheiro .obj
		if (!tinyobj::LoadObj(&attributes, &shapes, &materials, &warning, &error, obj_model_filepath.c_str())) {
			std::cout << warning << error << '\n';
		}

		// lê atributos do modelo 3D
		for (const auto& shape : shapes) {
			for (const auto& index : shape.mesh.indices) {
				glm::vec4 pos = {
					attributes.vertices[3 * index.vertex_index],
					attributes.vertices[3 * index.vertex_index + 1],
					attributes.vertices[3 * index.vertex_index + 2],
					1
				};

				glm::vec3 normal = {
					attributes.normals[3 * index.normal_index],
					attributes.normals[3 * index.normal_index + 1],
					attributes.normals[3 * index.normal_index + 2]
				};

				glm::vec2 textCoord = {
					attributes.texcoords[2 * index.texcoord_index],
					1.0f - attributes.texcoords[2 * index.texcoord_index + 1]
				};

				vertices.push_back(pos.x);
				vertices.push_back(pos.y);
				vertices.push_back(pos.z);
				vertices.push_back(normal.x);
				vertices.push_back(normal.y);
				vertices.push_back(normal.z);
				vertices.push_back(textCoord.x);
				vertices.push_back(textCoord.y);
			}
		}

		// armazena o modelo 3d
		_ballsVertices.push_back(load3dModel(obj_model_filepath.c_str()));

		// armazena o material
		std::string mtlFilename = getMtlFromObj(obj_model_filepath.c_str());
		_ballsMaterials.push_back(loadMaterial(mtlFilename.c_str()));

		// armazena a textura
		int lastIndex = _ballsMaterials.size() - 1;
		std::string textureFilename = _ballsMaterials[lastIndex].map_kd;
		_ballsTextures.push_back(loadTexture(textureFilename));
	}

	//Gera os VAO e VBO necessários e envia os dados do modelo (vértices, coordenadas de textura e normais) para a memória do GPU.
	void RendererBalls::Install(void) {
		// gera nomes para os VAOs das bolas
		glGenVertexArrays(_numberOfBalls, _ballsVAOs);

		// vincula o VAO de cada bola ao contexto OpenGL atual
		for (int i = 0; i < _numberOfBalls; i++) {
			glBindVertexArray(_ballsVAOs[i]);
		}

		// gera nomes para os VBOs das bolas
		glGenBuffers(_numberOfBalls, _ballsVBOs);

		// cria e configura cada VBO das bolas
		for (int i = 0; i < _ballsVertices.size(); i++) {
			// vincula o VBO ao contexto OpenGL atual
			glBindBuffer(GL_ARRAY_BUFFER, _ballsVBOs[i]);

			// inicializa o vbo atualmente ativo com dados imutáveis
			glBufferStorage(GL_ARRAY_BUFFER, _ballsVertices[i].size() * sizeof(float), _ballsVertices[i].data(), 0);

			// ativa atributos das posições dos vértices
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)0);
			glEnableVertexAttribArray(0);

			// ativa atributos das cores dos vértices
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
			glEnableVertexAttribArray(1);

			// ativa atributos das coordenadas de textura dos vértices
			glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(6 * sizeof(GLfloat)));
			glEnableVertexAttribArray(2);

			// desvincula o VAO atual
			glBindVertexArray(0);
		}

		// gera nomes para as texturas
		std::vector<GLuint> textureNames(_numberOfBalls);
		glGenTextures(_numberOfBalls, textureNames.data());

		// atribui a textura de cada bola
		for (int i = 0; i < _numberOfBalls; i++) {
			// ativa a unidade de textura atual (inicia na unidade 0)
			glActiveTexture(GL_TEXTURE0 + i);

			// vincula um nome de textura ao target GL_TEXTURE_2D da unidade de textura ativa
			glBindTexture(GL_TEXTURE_2D, textureNames[i]);

			// define os parâmetros de filtragem (wrapping e ajuste de tamanho)
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

			// carrega os dados da imagem para o objeto de textura vinculado ao target GL_TEXTURE_2D da unidade de textura ativa
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, _ballsTextures[0].width, _ballsTextures[i].height, 0, _ballsTextures[i].nChannels == 4 ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, _ballsTextures[i].image);

			// gera o mipmap para essa textura
			glGenerateMipmap(GL_TEXTURE_2D);
		}
	}

	//Função para renderizar o modelo na posição definida por position e orientação definida por orientation.
	void RendererBalls::Render(glm::vec3 position, glm::vec3 orientation) {
		Material* material = _material;
		loadMaterialUniforms(*material, _programShader);
	}

#pragma endregion


#pragma region funções secundárias da biblioteca PoolBalls

	std::vector<float> RendererBalls::load3dModel(const char* objFilepath) {
		std::vector<float> vertices;

		tinyobj::attrib_t attributes;
		std::vector<tinyobj::shape_t> shapes;
		std::vector<tinyobj::material_t> materials;
		std::string warning, error;

		// se houve erros ao carregar o ficheiro .obj
		if (!tinyobj::LoadObj(&attributes, &shapes, &materials, &warning, &error, objFilepath)) {
			std::cout << warning << error << '\n';
			return {};
		}

		// lê atributos do modelo 3D
		for (const auto& shape : shapes) {
			for (const auto& index : shape.mesh.indices) {
				glm::vec4 pos = {
					attributes.vertices[3 * index.vertex_index],
					attributes.vertices[3 * index.vertex_index + 1],
					attributes.vertices[3 * index.vertex_index + 2],
					1
				};

				glm::vec3 normal = {
					attributes.normals[3 * index.normal_index],
					attributes.normals[3 * index.normal_index + 1],
					attributes.normals[3 * index.normal_index + 2]
				};

				glm::vec2 textCoord = {
					attributes.texcoords[2 * index.texcoord_index],
					1.0f - attributes.texcoords[2 * index.texcoord_index + 1]
				};

				vertices.push_back(pos.x);
				vertices.push_back(pos.y);
				vertices.push_back(pos.z);
				vertices.push_back(normal.x);
				vertices.push_back(normal.y);
				vertices.push_back(normal.z);
				vertices.push_back(textCoord.x);
				vertices.push_back(textCoord.y);
			}
		}

		return vertices;
	}

	std::string RendererBalls::getMtlFromObj(const char* objFilepath) {
		std::ifstream file(objFilepath);
		std::string line;
		std::string mtlFilename;

		while (std::getline(file, line)) {
			std::istringstream stream(line);
			std::string prefix;
			stream >> prefix;

			if (prefix == "mtllib") {
				stream >> mtlFilename;
				break;
			}
		}

		return mtlFilename;
	}

	Material RendererBalls::loadMaterial(const char* mtlFilename) {
		std::string directory = "textures/";
		std::ifstream mtlFile(directory + mtlFilename);

		// se houve erros ao carregar o ficheiro .mtl
		if (!mtlFile) {
			std::cerr << "Erro ao carregar ficheiro .mtl." << std::endl;
			return {};
		}

		std::string line;
		std::string materialName;
		PoolBalls::Material material;

		// lê cada linha do ficheiro .mtl e guarda os respetivos dados
		while (std::getline(mtlFile, line)) {
			// ignora linhas vazias ou comentadas
			if (line.empty() || line[0] == '#') {
				continue;
			}

			std::istringstream stream(line);
			std::string command;
			stream >> command;

			// brilho
			if (command == "Ns") {
				float value;
				stream >> value;

				material.ns = value;
			}
			// cor ambiente
			else if (command == "Ka") {
				float r, g, b;
				stream >> r >> g >> b;

				material.ka = glm::vec3(r, g, b);
			}
			// cor difusa
			else if (command == "Kd") {
				float r, g, b;
				stream >> r >> g >> b;

				material.kd = glm::vec3(r, g, b);
			}
			// cor especular
			else if (command == "Ks") {
				float r, g, b;
				stream >> r >> g >> b;

				material.ks = glm::vec3(r, g, b);
			}
			// textura
			else if (command == "map_Kd") {
				std::string texture;
				stream >> texture;

				material.map_kd = texture;
			}
		}

		return material;
	}

	Texture RendererBalls::loadTexture(std::string imageFilename) {
		Texture texture{};
		int width, height, nChannels;

		// lê o ficheiro da imagem
		std::string directory = "textures/";
		std::string fullPath = directory + imageFilename;
		unsigned char* image = stbi_load(fullPath.c_str(), &width, &height, &nChannels, 0);

		// se houve erros ao abrir o ficheiro
		if (!image) {
			std::cerr << "Erro ao abrir o ficheiro '" << fullPath.c_str() << "'." << std::endl;
			return {};
		}

		// aloca memória para a imagem
		unsigned char* imageCopy = new unsigned char[width * height * nChannels];

		// copia a imagem para a nova área de memória
		std::memcpy(imageCopy, image, width * height * nChannels);

		texture.width = width;
		texture.height = height;
		texture.nChannels = nChannels;
		texture.image = imageCopy;

		// liberta a imagem da memória do CPU
		stbi_image_free(image);

		return texture;
	}

	void RendererBalls::loadLightingUniforms() {
		// fonte de luz ambiente global
		glProgramUniform3fv(_programShader, glGetProgramResourceLocation(_programShader, GL_UNIFORM, "ambientLight.ambient"), 1, glm::value_ptr(glm::vec3(7.0f)));

		// fonte de luz direcional
		glProgramUniform3fv(_programShader, glGetProgramResourceLocation(_programShader, GL_UNIFORM, "directionalLight.direction"), 1, glm::value_ptr(glm::vec3(1.0f, 0.0f, 0.0f)));
		glProgramUniform3fv(_programShader, glGetProgramResourceLocation(_programShader, GL_UNIFORM, "directionalLight.ambient"), 1, glm::value_ptr(glm::vec3(4.0f)));
		glProgramUniform3fv(_programShader, glGetProgramResourceLocation(_programShader, GL_UNIFORM, "directionalLight.diffuse"), 1, glm::value_ptr(glm::vec3(2.0f)));
		glProgramUniform3fv(_programShader, glGetProgramResourceLocation(_programShader, GL_UNIFORM, "directionalLight.specular"), 1, glm::value_ptr(glm::vec3(1.0f)));

		// fonte de luz pontual 
		glProgramUniform3fv(_programShader, glGetProgramResourceLocation(_programShader, GL_UNIFORM, "pointLight.position"), 1, glm::value_ptr(glm::vec3(1.0f, 0.0f, 0.0f)));
		glProgramUniform3fv(_programShader, glGetProgramResourceLocation(_programShader, GL_UNIFORM, "pointLight.ambient"), 1, glm::value_ptr(glm::vec3(0.6f)));
		glProgramUniform3fv(_programShader, glGetProgramResourceLocation(_programShader, GL_UNIFORM, "pointLight.diffuse"), 1, glm::value_ptr(glm::vec3(2.0f)));
		glProgramUniform3fv(_programShader, glGetProgramResourceLocation(_programShader, GL_UNIFORM, "pointLight.specular"), 1, glm::value_ptr(glm::vec3(1.0f)));
		glProgramUniform1f(_programShader, glGetProgramResourceLocation(_programShader, GL_UNIFORM, "pointLight.constant"), 1.0f);
		glProgramUniform1f(_programShader, glGetProgramResourceLocation(_programShader, GL_UNIFORM, "pointLight.linear"), 0.06f);
		glProgramUniform1f(_programShader, glGetProgramResourceLocation(_programShader, GL_UNIFORM, "pointLight.quadratic"), 0.02f);

		// fonte de luz conica
		glProgramUniform3fv(_programShader, glGetProgramResourceLocation(_programShader, GL_UNIFORM, "spotLight.position"), 1, glm::value_ptr(glm::vec3(0.0f, 2.5f, 0.0f)));
		glProgramUniform3fv(_programShader, glGetProgramResourceLocation(_programShader, GL_UNIFORM, "spotLight.direction"), 1, glm::value_ptr(glm::vec3(0.0f, -0.1f, 0.0f)));
		glProgramUniform3fv(_programShader, glGetProgramResourceLocation(_programShader, GL_UNIFORM, "spotLight.ambient"), 1, glm::value_ptr(glm::vec3(5.0f)));
		glProgramUniform3fv(_programShader, glGetProgramResourceLocation(_programShader, GL_UNIFORM, "spotLight.diffuse"), 1, glm::value_ptr(glm::vec3(1.0f)));
		glProgramUniform3fv(_programShader, glGetProgramResourceLocation(_programShader, GL_UNIFORM, "spotLight.specular"), 1, glm::value_ptr(glm::vec3(1.0f)));
		glProgramUniform1f(_programShader, glGetProgramResourceLocation(_programShader, GL_UNIFORM, "spotLight.constant"), 1.0f);
		glProgramUniform1f(_programShader, glGetProgramResourceLocation(_programShader, GL_UNIFORM, "spotLight.linear"), 0.09f);
		glProgramUniform1f(_programShader, glGetProgramResourceLocation(_programShader, GL_UNIFORM, "spotLight.quadratic"), 0.032f);
		glProgramUniform1f(_programShader, glGetProgramResourceLocation(_programShader, GL_UNIFORM, "spotLight.cutoff"), glm::cos(glm::radians(25.0f)));
		glProgramUniform1f(_programShader, glGetProgramResourceLocation(_programShader, GL_UNIFORM, "spotLight.outerCutOff"), glm::cos(glm::radians(35.0f)));

		glProgramUniform1i(_programShader, glGetProgramResourceLocation(_programShader, GL_UNIFORM, "lightModel"), 1);
	}

	void RendererBalls::loadMaterialUniforms(Material material, GLuint programShader) {
		glProgramUniform3fv(programShader, glGetProgramResourceLocation(programShader, GL_UNIFORM, "material.ambient"), 1, glm::value_ptr(material.ka));
		glProgramUniform3fv(programShader, glGetProgramResourceLocation(programShader, GL_UNIFORM, "material.diffuse"), 1, glm::value_ptr(material.kd));
		glProgramUniform3fv(programShader, glGetProgramResourceLocation(programShader, GL_UNIFORM, "material.specular"), 1, glm::value_ptr(material.ks));
		glProgramUniform1f(programShader, glGetProgramResourceLocation(programShader, GL_UNIFORM, "material.shininess"), material.ns);
	}

#pragma endregion

}

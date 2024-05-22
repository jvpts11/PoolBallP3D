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

#include "Source.h"
#include "LoadShaders.h"
#include "PoolBalls.h"

#pragma endregion


#pragma region variáveis globais

// mesa
const GLuint _numberOfTableVertices = 36;
GLfloat _tableVertices[_numberOfTableVertices * 8];
GLuint _tableVAO;
GLuint _tableVBO;

// bolas
PoolBalls::RendererBalls _rendererBalls(15);

// shaders
GLuint _programShader;

// câmara
glm::mat4 _model, _view, _projection;
glm::mat3 _normalMatrix;
GLfloat _angle = -10.0f;
glm::vec3 _cameraPosition = glm::vec3(0.0f, 1.0f, 5.0f);

// eventos do mouse
float _lastX = 0.0f;
float _lastY = 0.0f;
bool _firstMouse = true;

float _zoomLevel = 1.0f;
float _minZoom = 0.1f;
float _maxZoom = 3.5f;
float _zoomSpeed = 0.1f;
bool _animationStarted = false;

int animatedballIndex = 4;	//bola 5

// posições das bolas
std::vector<glm::vec3> _ballPositions = {
		glm::vec3((static_cast <float> (rand()) / static_cast <float> (RAND_MAX)) * 3.7f - 1.85f, 0.33f, (static_cast <float> (rand()) / static_cast <float> (RAND_MAX)) * 2.2f - 1.1f),		// bola 1
		glm::vec3((static_cast <float> (rand()) / static_cast <float> (RAND_MAX)) * 3.7f - 1.85f, 0.33f, (static_cast <float> (rand()) / static_cast <float> (RAND_MAX)) * 2.2f - 1.1f),		// bola 1
		glm::vec3((static_cast <float> (rand()) / static_cast <float> (RAND_MAX)) * 3.7f - 1.85f, 0.33f, (static_cast <float> (rand()) / static_cast <float> (RAND_MAX)) * 2.2f - 1.1f),		// bola 1
		glm::vec3((static_cast <float> (rand()) / static_cast <float> (RAND_MAX)) * 3.7f - 1.85f, 0.33f, (static_cast <float> (rand()) / static_cast <float> (RAND_MAX)) * 2.2f - 1.1f),		// bola 1
		glm::vec3(0.1f, 0.33f, -0.1f),		// bola 5
		glm::vec3((static_cast <float> (rand()) / static_cast <float> (RAND_MAX)) * 3.7f - 1.85f, 0.33f, (static_cast <float> (rand()) / static_cast <float> (RAND_MAX)) * 2.2f - 1.1f),		// bola 1
		glm::vec3((static_cast <float> (rand()) / static_cast <float> (RAND_MAX)) * 3.7f - 1.85f, 0.33f, (static_cast <float> (rand()) / static_cast <float> (RAND_MAX)) * 2.2f - 1.1f),		// bola 1
		glm::vec3((static_cast <float> (rand()) / static_cast <float> (RAND_MAX)) * 3.7f - 1.85f, 0.33f, (static_cast <float> (rand()) / static_cast <float> (RAND_MAX)) * 2.2f - 1.1f),		// bola 1
		glm::vec3((static_cast <float> (rand()) / static_cast <float> (RAND_MAX)) * 3.7f - 1.85f, 0.33f, (static_cast <float> (rand()) / static_cast <float> (RAND_MAX)) * 2.2f - 1.1f),		// bola 1
		glm::vec3((static_cast <float> (rand()) / static_cast <float> (RAND_MAX)) * 3.7f - 1.85f, 0.33f, (static_cast <float> (rand()) / static_cast <float> (RAND_MAX)) * 2.2f - 1.1f),		// bola 1
		glm::vec3((static_cast <float> (rand()) / static_cast <float> (RAND_MAX)) * 3.7f - 1.85f, 0.33f, (static_cast <float> (rand()) / static_cast <float> (RAND_MAX)) * 2.2f - 1.1f),		// bola 1
		glm::vec3(0.7f, 0.33f, 0.5f),		// bola 12
		glm::vec3((static_cast <float> (rand()) / static_cast <float> (RAND_MAX)) * 3.7f - 1.85f, 0.33f, (static_cast <float> (rand()) / static_cast <float> (RAND_MAX)) * 2.2f - 1.1f),		// bola 1
		glm::vec3((static_cast <float> (rand()) / static_cast <float> (RAND_MAX)) * 3.7f - 1.85f, 0.33f, (static_cast <float> (rand()) / static_cast <float> (RAND_MAX)) * 2.2f - 1.1f),		// bola 1
		glm::vec3((static_cast <float> (rand()) / static_cast <float> (RAND_MAX)) * 3.7f - 1.85f, 0.33f, (static_cast <float> (rand()) / static_cast <float> (RAND_MAX)) * 2.2f - 1.1f),		// bola 1
};

std::vector<glm::vec3> _ballRotations = {
		glm::vec3(0.0f), // bola 1
		glm::vec3(0.0f), // bola 2
		glm::vec3(0.0f), // bola 3
		glm::vec3(0.0f), // bola 4
		glm::vec3(0.0f), // bola 5
		glm::vec3(0.0f), // bola 6
		glm::vec3(0.0f), // bola 7
		glm::vec3(0.0f), // bola 8
		glm::vec3(0.0f), // bola 9
		glm::vec3(0.0f), // bola 10
		glm::vec3(0.0f), // bola 11
		glm::vec3(0.0f), // bola 12
		glm::vec3(0.0f), // bola 13
		glm::vec3(0.0f), // bola 14
		glm::vec3(0.0f), // bola 15
};

#pragma endregion


#pragma region ponto de entrada do programa

int main()
{
	// para quando houver algum erro com a glfw
	glfwSetErrorCallback(printErrorCallback);

	// inicializa a glfw
	if (!glfwInit()) {
		return -1;
	}

	// cria janela
	GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_NAME, NULL, NULL);
	if (window == nullptr) {
		std::cout << "Erro ao inicializar a biblioteca GLFW" << std::endl;
		glfwTerminate();
		return -1;
	}

	// define o contexto atual para a renderização da janela
	glfwMakeContextCurrent(window);

	// inicializa a glew
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK) {
		std::cout << "Erro ao inicializar a biblioteca GLEW" << std::endl;
		glfwTerminate();
		return -1;
	}

	// inicializa a cena pela primeira vez
	init();


	glfwSetKeyCallback(window, keyCallback);

	// quando o utilizador faz scroll com o mouse
	glfwSetScrollCallback(window, scrollCallback);

	// quando o utilizador muda a visão da câmara
	glfwSetCursorPosCallback(window, mouseCallback);

	// quando é pressionada uma tecla (para ativar/desativar os diferentes tipos de luzes)
	glfwSetCharCallback(window, charCallback);

	// mantém a janela aberta e atualizada
	while (!glfwWindowShouldClose(window))
	{
		// renderiza os objetos na cena
		display();

		// troca os buffers de renderização (da frame antiga para a nova)
		glfwSwapBuffers(window);

		// processa todos os eventos ocorridos
		glfwPollEvents();
	}

	// termina todas as instâncias da glfw
	glfwTerminate();

	return 1;
}

#pragma endregion


#pragma region funções do programa


void init(void) {
	// posição da mesa
	float xCoord = 2.0f;
	float yCoord = 0.25f;
	float zCoord = 1.25f;

	// cores da mesa
	std::vector<glm::vec3> colors{
		glm::vec3(1.0f, 0.0f, 0.0f),	   // vermelho
			glm::vec3(1.0f, 1.0f, 0.0f),   // amarelo
			glm::vec3(0.1f, 1.0f, 0.1f),   // verde
			glm::vec3(0.0f, 1.0f, 1.0f),   // ciano
			glm::vec3(0.0f, 0.0f, 1.0f),   // azul
			glm::vec3(1.0f, 0.0f, 1.0f)	   // magenta
	};

	// coordenadas de textura da mesa
	float xTexture = 0.0f;
	float yTexture = 0.0f;

	// cria atributos dos vértices da mesa
	GLfloat _tableVertices[_numberOfTableVertices * 8] = {
		//*************************************************
		//                       X+ (face #0)
		// ************************************************
		// Primeiro triângulo
		// Posições						Cores											Coordenadas de textura
		xCoord, -yCoord,  zCoord,		colors[0].x, colors[0].y, colors[0].z,		xTexture, yTexture,
		xCoord, -yCoord, -zCoord,		colors[0].x, colors[0].y, colors[0].z,      xTexture, yTexture,
		xCoord,  yCoord,  zCoord,		colors[0].x, colors[0].y, colors[0].z,      xTexture, yTexture,
		// Segundo triângulo					 			  			   
		xCoord,  yCoord,  zCoord,		colors[0].x, colors[0].y, colors[0].z,      xTexture, yTexture,
		xCoord, -yCoord, -zCoord,		colors[0].x, colors[0].y, colors[0].z,      xTexture, yTexture,
		xCoord,  yCoord, -zCoord,		colors[0].x, colors[0].y, colors[0].z,      xTexture, yTexture,
		// ************************************************				   
		//                       X- (face #1)							   
		// ************************************************				   
		// Primeiro triângulo											   
		-xCoord, -yCoord, -zCoord,		colors[1].x, colors[1].y, colors[1].z,		xTexture, yTexture,
		-xCoord, -yCoord,  zCoord,		colors[1].x, colors[1].y, colors[1].z,		xTexture, yTexture,
		-xCoord,  yCoord, -zCoord,		colors[1].x, colors[1].y, colors[1].z,		xTexture, yTexture,
		// Segundo triângulo					 			  			   
		-xCoord,  yCoord, -zCoord,		colors[1].x, colors[1].y, colors[1].z,		xTexture, yTexture,
		-xCoord, -yCoord,  zCoord,		colors[1].x, colors[1].y, colors[1].z,		xTexture, yTexture,
		-xCoord,  yCoord,  zCoord,		colors[1].x, colors[1].y, colors[1].z,		xTexture, yTexture,
		// ************************************************				   
		//                       Y+ (face #2)							   
		// ************************************************				   
		// Primeiro triângulo											   
		-xCoord, yCoord, zCoord,		colors[2].x, colors[2].y, colors[2].z,      xTexture, yTexture,
		xCoord, yCoord, zCoord,			colors[2].x, colors[2].y, colors[2].z,      xTexture, yTexture,
		-xCoord, yCoord, -zCoord,		colors[2].x, colors[2].y, colors[2].z,      xTexture, yTexture,
		// Segundo triângulo					 			  			   
		-xCoord, yCoord, -zCoord,		colors[2].x, colors[2].y, colors[2].z,      xTexture, yTexture,
		xCoord, yCoord, zCoord,			colors[2].x, colors[2].y, colors[2].z,      xTexture, yTexture,
		xCoord, yCoord, -zCoord,		colors[2].x, colors[2].y, colors[2].z,      xTexture, yTexture,
		// ************************************************				   
		//                       Y- (face #3)							   
		// ************************************************				   
		// Primeiro triângulo											   
		-xCoord, -yCoord, -zCoord,		colors[3].x, colors[3].y, colors[3].z,		xTexture, yTexture,
		xCoord, -yCoord, -zCoord,		colors[3].x, colors[3].y, colors[3].z,		xTexture, yTexture,
		-xCoord, -yCoord, zCoord,		colors[3].x, colors[3].y, colors[3].z,		xTexture, yTexture,
		// Segundo triângulo					 			  			   
		-xCoord, -yCoord, zCoord,		colors[3].x, colors[3].y, colors[3].z,		xTexture, yTexture,
		xCoord, -yCoord, -zCoord,		colors[3].x, colors[3].y, colors[3].z,		xTexture, yTexture,
		xCoord, -yCoord, zCoord,		colors[3].x, colors[3].y, colors[3].z,		xTexture, yTexture,
		// ************************************************	  			   
		//                       Z+ (face #4)				  			   
		// ************************************************	  			   
		// Primeiro triângulo								  			   
		-xCoord, -yCoord, zCoord,		colors[4].x, colors[4].y, colors[4].z,      xTexture, yTexture,
		xCoord, -yCoord, zCoord,		colors[4].x, colors[4].y, colors[4].z,      xTexture, yTexture,
		-xCoord, yCoord, zCoord,		colors[4].x, colors[4].y, colors[4].z,      xTexture, yTexture,
		// Segundo triângulo					 			  			   
		-xCoord, yCoord, zCoord,		colors[4].x, colors[4].y, colors[4].z,      xTexture, yTexture,
		xCoord, -yCoord, zCoord,		colors[4].x, colors[4].y, colors[4].z,      xTexture, yTexture,
		xCoord,  yCoord, zCoord,		colors[4].x, colors[4].y, colors[4].z,      xTexture, yTexture,
		// ************************************************	  			   
		//                       Z- (face #5)				  			   
		// ************************************************	  			   
		// Primeiro triângulo								  			   
		xCoord, -yCoord, -zCoord,		colors[5].x, colors[5].y, colors[5].z,      xTexture, yTexture,
		-xCoord, -yCoord, -zCoord,		colors[5].x, colors[5].y, colors[5].z,      xTexture, yTexture,
		xCoord, yCoord, -zCoord,		colors[5].x, colors[5].y, colors[5].z,      xTexture, yTexture,
		// Segundo triângulo					 			  			   
		xCoord, yCoord, -zCoord,		colors[5].x, colors[5].y, colors[5].z,      xTexture, yTexture,
		-xCoord, -yCoord, -zCoord,		colors[5].x, colors[5].y, colors[5].z,      xTexture, yTexture,
		-xCoord, yCoord, -zCoord,		colors[5].x, colors[5].y, colors[5].z,      xTexture, yTexture
	};

	// gera o nome para o VAO da mesa
	glGenVertexArrays(1, &_tableVAO);

	// vincula o VAO ao contexto OpenGL atual
	glBindVertexArray(_tableVAO);

	// gera o nome para o VBO da mesa
	glGenBuffers(1, &_tableVBO);

	// vincula o VBO ao contexto OpenGL atual
	glBindBuffer(GL_ARRAY_BUFFER, _tableVBO);

	// inicializa o VBO atualmente ativo com dados imutáveis
	glBufferStorage(GL_ARRAY_BUFFER, sizeof(_tableVertices), _tableVertices, 0);

	// ativar atributos das posições dos vértices
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)0);
	glEnableVertexAttribArray(0);

	// ativar atributos das cores dos vértices
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	// ativar atributos das coordenadas de textura dos vértices
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);

	// desvincula o VAO atual
	glBindVertexArray(_tableVAO);

	// carrega o modelo, material e textura de cada bola
	for (int i = 1; i <= _rendererBalls.getNumberOfBalls(); i++) {
		std::string objFilepath = "textures/Ball" + std::to_string(i) + ".obj";
		_rendererBalls.Load(objFilepath);
	}

	_rendererBalls.Install();

	// cria informações dos shaders
	ShaderInfo shaders[] = {
		{ GL_VERTEX_SHADER,   "shaders/poolballs.vert" },
		{ GL_FRAGMENT_SHADER, "shaders/poolballs.frag" },
		{ GL_NONE, NULL }
	};

	// carrega shaders
	_programShader = loadShaders(shaders);

	// se houve erros ao carregar shaders
	if (!_programShader) {
		std::cout << "Erro ao carregar shaders: " << std::endl;
		exit(EXIT_FAILURE);
	}

	// vincula o programa shader ao contexto OpenGL atual
	glUseProgram(_programShader);

	// obtém as localizações dos atributos no programa shader
	GLint positionId = glGetProgramResourceLocation(_programShader, GL_PROGRAM_INPUT, "vPosition");
	GLint normalId = glGetProgramResourceLocation(_programShader, GL_PROGRAM_INPUT, "vNormal");
	GLint textCoordId = glGetProgramResourceLocation(_programShader, GL_PROGRAM_INPUT, "vTextureCoords");

	// faz a ligação entre os atributos do programa shader ao VAO e VBO ativos 
	glVertexAttribPointer(positionId, 3 /*3 elementos por vértice*/, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glVertexAttribPointer(normalId, 3 /*3 elementos por cor*/, GL_FLOAT, GL_TRUE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glVertexAttribPointer(textCoordId, 2 /*3 elementos por coordenadas da textura*/, GL_FLOAT, GL_TRUE, 8 * sizeof(float), (void*)(6 * sizeof(float)));

	// ativa os atributos do programa shader ao VAO ativo
	glEnableVertexAttribArray(positionId);
	glEnableVertexAttribArray(normalId);
	glEnableVertexAttribArray(textCoordId);

	// matrizes de transformação
	_model = glm::rotate(glm::mat4(1.0f), _angle, glm::vec3(0.0f, 1.0f, 0.0f));
	_view = glm::lookAt(
		_cameraPosition,				// eye (posição da câmara).
		glm::vec3(0.0f, 0.0f, 0.0f),	// center (para onde está a "olhar")
		glm::vec3(0.0f, 1.0f, 0.0f)		// up
	);
	glm::mat4 modelView = _view * _model;
	_projection = glm::perspective(glm::radians(45.0f), 4.0f / 3.0f, 0.1f, 100.0f);
	_normalMatrix = glm::inverseTranspose(glm::mat3(modelView));

	// obtém as localizações dos uniforms no programa shader
	GLint modelId = glGetProgramResourceLocation(_programShader, GL_UNIFORM, "Model");
	GLint viewId = glGetProgramResourceLocation(_programShader, GL_UNIFORM, "View");
	GLint modelViewId = glGetProgramResourceLocation(_programShader, GL_UNIFORM, "ModelView");
	GLint projectionId = glGetProgramResourceLocation(_programShader, GL_UNIFORM, "Projection");
	GLint normalViewId = glGetProgramResourceLocation(_programShader, GL_UNIFORM, "NormalMatrix");

	// atribui o valor aos uniforms do programa shader
	glProgramUniformMatrix4fv(_programShader, modelId, 1, GL_FALSE, glm::value_ptr(_model));
	glProgramUniformMatrix4fv(_programShader, viewId, 1, GL_FALSE, glm::value_ptr(_view));
	glProgramUniformMatrix4fv(_programShader, modelViewId, 1, GL_FALSE, glm::value_ptr(modelView));
	glProgramUniformMatrix4fv(_programShader, projectionId, 1, GL_FALSE, glm::value_ptr(_projection));
	glProgramUniformMatrix3fv(_programShader, normalViewId, 1, GL_FALSE, glm::value_ptr(_normalMatrix));

	_rendererBalls.setProgramShader(_programShader);

	_rendererBalls.loadLightingUniforms();

	// define a janela de renderização
	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

	// ativa o teste de profundidade e o descarte de polígonos não observáveis
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
}

void display(void) {
	// limpa o buffer de cor e de profundidade
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// translação da mesa
	glm::mat4 translatedModel = glm::translate(_model, glm::vec3(0.0f, 0.0f, 0.0f));

	// modelo de visualização do objeto
	glm::mat4 modelView = _view * translatedModel;

	// obtém a localização do uniform
	GLint modelViewId = glGetProgramResourceLocation(_programShader, GL_UNIFORM, "ModelView");

	// atribui o valor ao uniform
	glProgramUniformMatrix4fv(_programShader, modelViewId, 1, GL_FALSE, glm::value_ptr(modelView));

	GLint renderTex = glGetProgramResourceLocation(_programShader, GL_UNIFORM, "renderTex");
	glProgramUniform1i(_programShader, renderTex, 0);

	GLint viewPositionLoc = glGetUniformLocation(_programShader, "viewPosition");
	glUniform3f(viewPositionLoc, _cameraPosition.x, _cameraPosition.y, _cameraPosition.z);

	// desenha a mesa na tela
	glBindVertexArray(_tableVAO);
	glDrawArrays(GL_TRIANGLES, 0, _numberOfTableVertices);

	

	PoolBalls::Material material;
	material.ka = glm::vec3(1.0, 1.0, 1.0);
	material.kd = glm::vec3(1.0, 1.0, 1.0);
	material.ks = glm::vec3(1.0, 1.0, 1.0);
	material.ns = 12.0f;

	_rendererBalls.loadMaterialUniforms(material, _programShader);

	// desenha para cada bola
	for (int i = 0; i < _rendererBalls.getNumberOfBalls(); i++) {
		// translação da bola
		translatedModel = glm::translate(_model, _ballPositions[i]);

		glm::mat4 rotatedModel = glm::rotate(translatedModel, glm::radians(_ballRotations[i].z), glm::vec3(0.0f, 0.0f, 1.0f));	// rotação no eixo z
		rotatedModel = glm::rotate(rotatedModel, glm::radians(_ballRotations[i].y), glm::vec3(0.0f, 1.0f, 0.0f));					// rotação no eixo y
		rotatedModel = glm::rotate(rotatedModel, glm::radians(_ballRotations[i].x), glm::vec3(1.0f, 0.0f, 0.0f));

		// escala de cada bola
		glm::mat4 scaledModel = glm::scale(rotatedModel, glm::vec3(0.08f));

		// modelo de visualização do objeto
		modelView = _view * scaledModel;

		// obtém a localização do uniform
		modelViewId = glGetProgramResourceLocation(_programShader, GL_UNIFORM, "ModelView");

		// atribui o valor ao uniform
		glProgramUniformMatrix4fv(_programShader, modelViewId, 1, GL_FALSE, glm::value_ptr(modelView));

		GLint locationTexSampler1 = glGetProgramResourceLocation(_programShader, GL_UNIFORM, "sampler");
		glProgramUniform1i(_programShader, locationTexSampler1, i /*unidade de textura*/);
		glProgramUniform1i(_programShader, renderTex, 1);

		_rendererBalls.loadMaterialUniforms(_rendererBalls.getBallsMaterials()[i], _programShader);

		// desenha a bola na tela
		glBindVertexArray(0);
		glDrawArrays(GL_TRIANGLES, 0, _rendererBalls.getBallsVertices()[i].size() / 8);
	}

	if (_animationStarted) {
		_ballPositions[animatedballIndex].x += 0.005f;
		_ballPositions[animatedballIndex].z += 0.005f;
		
		_ballRotations[animatedballIndex].x += 2.0f;
		_ballRotations[animatedballIndex].y += 2.0f;
		_ballRotations[animatedballIndex].z -= 2.0f;

		if (collision()) {
			_animationStarted = false;
		}
	}

}


bool collision() {
	float _ballRadius = 0.08f;

	for (int i = 0; i < _ballPositions.size(); i++) {
		if (i != animatedballIndex) {
			float distance = glm::distance(_ballPositions[i], _ballPositions[animatedballIndex]);
			if (distance <= 2 * _ballRadius) {
				return true; // Colisão detectada
			}
		}
	}

	// Verificar colisão com os limites da mesa
	if (_ballPositions[animatedballIndex].x + _ballRadius >= 1.3f || _ballPositions[animatedballIndex].x - _ballRadius <= -1.3f ||
		_ballPositions[animatedballIndex].y + _ballRadius >= 2.05f || _ballPositions[animatedballIndex].y - _ballRadius <= -2.05f) {
		return true; // Colisão detectada
	}

	return false; // Nenhuma colisão
}


#pragma endregion


#pragma region funções de callbacks para a biblioteca glfw

void printErrorCallback(int code, const char* description) {
	std::cout << description << std::endl;
}

void scrollCallback(GLFWwindow* window, double xOffset, double yOffset)
{
	if (yOffset > 0)
		_zoomLevel -= _zoomSpeed;
	else
		_zoomLevel += _zoomSpeed;

	_zoomLevel = glm::clamp(_zoomLevel, _minZoom, _maxZoom);

	_projection = glm::perspective(glm::radians(45.0f) * _zoomLevel, 4.0f / 3.0f, 0.1f, 100.0f);

	GLint projectionId = glGetProgramResourceLocation(_programShader, GL_UNIFORM, "Projection");
	glProgramUniformMatrix4fv(_programShader, projectionId, 1, GL_FALSE, glm::value_ptr(_projection));
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
		_animationStarted = true;
	}
}
void mouseCallback(GLFWwindow* window, double xpos, double ypos)
{
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) != GLFW_PRESS)
	{
		_lastX = xpos;
		_lastY = ypos;
		_firstMouse = false;
		return; // Sai da função se o botão do mouse esquerdo não estiver pressionado
	}

	if (_firstMouse)
	{
		_lastX = xpos;
		_lastY = ypos;
		_firstMouse = false;
	}

	float xOffset = xpos - _lastX;
	float yOffset = ypos - _lastY;

	_lastX = xpos;
	_lastY = ypos;

	float sensitivity = 0.1f;

	// Aplicar rotação horizontal 
	_view = glm::rotate(_view, glm::radians(xOffset * sensitivity), glm::vec3(0.0f, 1.0f, 0.0f));

	// Calcular o vetor direito da câmera
	glm::vec3 right = glm::normalize(glm::cross(glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(_view[2])));

	// Aplicar rotação vertical 
	_view = glm::rotate(_view, glm::radians(yOffset * sensitivity), right);
}

void charCallback(GLFWwindow* window, unsigned int codepoint)
{
	int lightModel;

	// deteta as teclas do teclado
	switch (codepoint)
	{
	case '1':
		lightModel = 1;
		glProgramUniform1i(_programShader, glGetProgramResourceLocation(_programShader, GL_UNIFORM, "lightModel"), lightModel);
		std::cout << "Ambient light activated" << std::endl;
		break;
	case '2':
		lightModel = 2;
		glProgramUniform1i(_programShader, glGetProgramResourceLocation(_programShader, GL_UNIFORM, "lightModel"), lightModel);
		std::cout << "Direction light activated." << std::endl;
		break;
	case '3':
		lightModel = 3;
		glProgramUniform1i(_programShader, glGetProgramResourceLocation(_programShader, GL_UNIFORM, "lightModel"), lightModel);
		std::cout << "Point light activated." << std::endl;
		break;
	case '4':
		lightModel = 4;
		glProgramUniform1i(_programShader, glGetProgramResourceLocation(_programShader, GL_UNIFORM, "lightModel"), lightModel);
		std::cout << "Spot light activated." << std::endl;
		break;
	default:
		break;
	}
}

#pragma endregion
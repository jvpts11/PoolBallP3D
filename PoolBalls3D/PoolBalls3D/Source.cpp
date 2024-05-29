/*
 * Discipline: 3D Programming
 * Subject: Pool Balls 3D Project
 * Authors: João Tavares nº 21871, Diogo Silva nº 22369, Ademar Valente nº 23155, José Lourenço nº23496
 * Date: 17/05/2024
 * File: Source.cpp
 * Description: Main file of the project
 */

#pragma region import

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
#include "PoolLib.h"

#pragma endregion

#pragma region variables

//table variables
const GLuint _nTableVertices = 36;
GLuint _tableVBO;
GLuint _tableVAO;

//camera variables
//glm::mat4 _viewMatrix, _projectionMatrix, _modelMatrix;
//glm::mat3 _normalMatrix;
GLfloat _angle = -10.0f;
glm::vec3 _cameraPosition = glm::vec3(0.0f, 1.0f, 5.0f);

//shader variables
GLuint _shaderProgram;

//mouse variables
float _lastX = 0.0f;
float _lastY = 0.0f;
bool _firstMouse = true;

float _zoomLvl = 1.0f;
float _minZoom = 0.1f;
float _maxZoom = 3.5f;

float _zoomSpeed = 0.1f;

//ball variables
int animBallIndex = 5;
bool animStart = false;

PoolLib::RenderBalls _balls(15);
std::vector<glm::vec3> _ballVertices = {generateRandomBallPositions(15, 3.7, 2.2, 0.08)};
std::vector<glm::vec3> _ballOrientations = {
	glm::vec3(0.0f),
	glm::vec3(0.0f),
	glm::vec3(0.0f),
	glm::vec3(0.0f),
	glm::vec3(0.0f),
	glm::vec3(0.0f),
	glm::vec3(0.0f),
	glm::vec3(0.0f),
	glm::vec3(0.0f),
	glm::vec3(0.0f),
	glm::vec3(0.0f),
	glm::vec3(0.0f),
	glm::vec3(0.0f),
	glm::vec3(0.0f),
	glm::vec3(0.0f)
};

#pragma endregion

int main()
{
	// Initialize GLFW
	if (!glfwInit())
	{
		std::cerr << "Failed to initialize GLFW" << std::endl;
		return -1;
	}

	// Create a windowed mode window and its OpenGL context
	GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_TITLE, NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		std::cerr << "Failed to create window" << std::endl;
		return -1;
	}

	// Make the window's context current
	glfwMakeContextCurrent(window);

	// Initialize GLEW
	if (glewInit() != GLEW_OK)
	{
		std::cerr << "Failed to initialize GLEW" << std::endl;
		glfwTerminate();
		return -1;
	}

	init();

	//set zoom callback
	glfwSetScrollCallback(window, scrollCallback);

	//set mouse callback
	glfwSetCursorPosCallback(window, mouseCallback);

	//set key callback
	glfwSetKeyCallback(window, keyCallback);

	//set character callback
	glfwSetCharCallback(window, characterCallback);

	// Loop until the user closes the window
	while (!glfwWindowShouldClose(window))
	{
		// Render here
		display();

		// Swap front and back buffers
		glfwSwapBuffers(window);

		// Poll for and process events
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}
void init()
{
	//table position
	float xCoord = 2.0f;
	float yCoord = 0.25f;
	float zCoord = 1.25f;

	//table colors
	std::vector<glm::vec3> colors{
		glm::vec3(0.0f, 0.0f, 1.0f), //blue
		glm::vec3(1.0f, 0.0f, 0.0f), //red
		glm::vec3(0.0f, 1.0f, 0.0f), //green
		glm::vec3(0.0f, 1.0f, 1.0f), //cyan
		glm::vec3(1.0f, 1.0f, 0.0f), //yellow
		glm::vec3(1.0f, 1.0f, 1.0f) //white
	};

	//table texture coordinates
	float xTexture = 0.0f;
	float yTexture = 0.0f;

	//table vertices
	GLfloat _tableVertices[_nTableVertices * 8] = {
		//X+ face
		//triangle 1
		xCoord, -yCoord, zCoord,		 colors[0].r, colors[0].g, colors[0].b,		xTexture, yTexture,
		xCoord, -yCoord, -zCoord,	     colors[0].r, colors[0].g, colors[0].b,		xTexture, yTexture,
		xCoord, yCoord, zCoord,		     colors[0].r, colors[0].g, colors[0].b,		xTexture, yTexture,
		//triangle 2
		xCoord, yCoord, zCoord,		     colors[0].r, colors[0].g, colors[0].b,		xTexture, yTexture,
		xCoord, -yCoord, -zCoord,	     colors[0].r, colors[0].g, colors[0].b,		xTexture, yTexture,
		xCoord, yCoord, -zCoord,	     colors[0].r, colors[0].g, colors[0].b,		xTexture, yTexture,

		//X- face
		//triangle 1
		-xCoord, -yCoord, -zCoord,	     colors[1].r, colors[1].g, colors[1].b,		xTexture, yTexture,
		-xCoord, -yCoord, zCoord,	     colors[1].r, colors[1].g, colors[1].b,		xTexture, yTexture,
		-xCoord, yCoord, -zCoord,	     colors[1].r, colors[1].g, colors[1].b,		xTexture, yTexture,
		//triangle 2
		-xCoord, yCoord, -zCoord,	     colors[1].r, colors[1].g, colors[1].b,		xTexture, yTexture,
		-xCoord, -yCoord, zCoord,	     colors[1].r, colors[1].g, colors[1].b,		xTexture, yTexture,
		-xCoord, yCoord, zCoord,	     colors[1].r, colors[1].g, colors[1].b,		xTexture, yTexture,

		//Y+ face
		//triangle 1
		-xCoord, yCoord, zCoord,	     colors[2].r, colors[2].g, colors[2].b,		xTexture, yTexture,
		xCoord, yCoord,  zCoord,	     colors[2].r, colors[2].g, colors[2].b,		xTexture, yTexture,
		-xCoord, yCoord, -zCoord,	     colors[2].r, colors[2].g, colors[2].b,		xTexture, yTexture,
		//triangle 2
		-xCoord, yCoord, -zCoord,	     colors[2].r, colors[2].g, colors[2].b,		xTexture, yTexture,
		xCoord, yCoord,  zCoord,	     colors[2].r, colors[2].g, colors[2].b,		xTexture, yTexture,
		xCoord, yCoord, -zCoord,	     colors[2].r, colors[2].g, colors[2].b,		xTexture, yTexture,

		//Y- face
		//triangle 1
		-xCoord, -yCoord, -zCoord,	     colors[3].r, colors[3].g, colors[3].b,		xTexture, yTexture,
		xCoord, -yCoord, -zCoord,	     colors[3].r, colors[3].g, colors[3].b,		xTexture, yTexture,
		-xCoord, -yCoord, zCoord,	     colors[3].r, colors[3].g, colors[3].b,		xTexture, yTexture,
		//triangle 2
		-xCoord, -yCoord, zCoord,	     colors[3].r, colors[3].g, colors[3].b,		xTexture, yTexture,
		xCoord, -yCoord, -zCoord,	     colors[3].r, colors[3].g, colors[3].b,		xTexture, yTexture,
		xCoord, -yCoord, zCoord,	     colors[3].r, colors[3].g, colors[3].b,		xTexture, yTexture,

		//Z+ face
		//triangle 1
		-xCoord, -yCoord, zCoord,	     colors[4].r, colors[4].g, colors[4].b,		xTexture, yTexture,
		xCoord, -yCoord, zCoord,	     colors[4].r, colors[4].g, colors[4].b,		xTexture, yTexture,
		-xCoord, yCoord, zCoord,	     colors[4].r, colors[4].g, colors[4].b,		xTexture, yTexture,
		//triangle 2
		 -xCoord, yCoord, zCoord,	     colors[4].r, colors[4].g, colors[4].b,		xTexture, yTexture,
		xCoord, -yCoord,  zCoord,	     colors[4].r, colors[4].g, colors[4].b,		xTexture, yTexture,
		xCoord, yCoord, zCoord,			 colors[4].r, colors[4].g, colors[4].b,		xTexture, yTexture,

		//Z- face
		//triangle 1
		xCoord, -yCoord, -zCoord,	     colors[5].r, colors[5].g, colors[5].b,		xTexture, yTexture,
		-xCoord, -yCoord, -zCoord,	     colors[5].r, colors[5].g, colors[5].b,		xTexture, yTexture,
		xCoord, yCoord, -zCoord,	     colors[5].r, colors[5].g, colors[5].b,		xTexture, yTexture,
		//triangle 2
		xCoord, yCoord, -zCoord,	     colors[5].r, colors[5].g, colors[5].b,		xTexture, yTexture,
		-xCoord, -yCoord, -zCoord,	     colors[5].r, colors[5].g, colors[5].b,		xTexture, yTexture,
		-xCoord, yCoord, -zCoord,	     colors[5].r, colors[5].g, colors[5].b,		xTexture, yTexture
	};

	//generate VAO
	glGenVertexArrays(1, &_tableVAO);
	glBindVertexArray(_tableVAO);

	//generate VBO
	glGenBuffers(1, &_tableVBO);
	glBindBuffer(GL_ARRAY_BUFFER, _tableVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(_tableVertices), _tableVertices, GL_STATIC_DRAW);


	//set vertex attribute pointers
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);


	//unbind VAO
	glBindVertexArray(0);

	for (int i = 1; i <= _balls.getNumberOfBalls(); i++)
	{
		std::string texturePath = "textures/Ball" + std::to_string(i) + ".obj";
		_balls.Load(texturePath);
	}
	_balls.Install();

	//shader info
	shaderInfo shaders[] = {
		{ GL_VERTEX_SHADER, "shaders/poolballs.vert" },
		{ GL_FRAGMENT_SHADER, "shaders/poolballs.frag" },
		{ GL_NONE, NULL }
	};
	
	//load shaders
	_shaderProgram = loadShaders(shaders);

	//bind shader program
	glUseProgram(_shaderProgram);

	//store attributes
	GLint vPosition = glGetProgramResourceLocation (_shaderProgram, GL_PROGRAM_INPUT, "vPosition");
	GLint vNormal = glGetProgramResourceLocation(_shaderProgram, GL_PROGRAM_INPUT, "vNormal");
	GLint vTexCoords = glGetProgramResourceLocation(_shaderProgram, GL_PROGRAM_INPUT, "vTextureCoords");

	//link attributes to vao and vbo
	glVertexAttribPointer(vPosition, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(vPosition);
	glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(vNormal);
	glVertexAttribPointer(vTexCoords, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(vTexCoords);

	//transformation matrices
	PoolLib::_viewMatrix = glm::lookAt(_cameraPosition, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	PoolLib::_modelMatrix = glm::rotate(glm::mat4(1.0f), _angle, glm::vec3(0.0f,1.0f,0.0f));
	glm::mat4 _modelViewMatrix = PoolLib::_viewMatrix * PoolLib::_modelMatrix;
	PoolLib::_projectionMatrix = glm::perspective(45.0f, (GLfloat)SCREEN_WIDTH / (GLfloat)SCREEN_HEIGHT, 0.1f, 100.0f);
	PoolLib::_normalMatrix = glm::inverseTranspose(glm::mat3(_modelViewMatrix));

	//store uniform locations
	GLint uModelMatrix = glGetUniformLocation(_shaderProgram, "Model");
	GLint uNormalMatrix = glGetUniformLocation(_shaderProgram, "NormalMatrix");
	GLint uProjectionMatrix = glGetUniformLocation(_shaderProgram, "Projection");
	GLint uViewMatrix = glGetUniformLocation(_shaderProgram, "View");
	GLint uModelViewMatrix = glGetUniformLocation(_shaderProgram, "ModelView");

	//link uniform values to program
	glProgramUniformMatrix4fv(_shaderProgram, uModelMatrix, 1, GL_FALSE, glm::value_ptr(PoolLib::_modelMatrix));
	glProgramUniformMatrix3fv(_shaderProgram, uNormalMatrix, 1, GL_FALSE, glm::value_ptr(PoolLib::_normalMatrix));
	glProgramUniformMatrix4fv(_shaderProgram, uProjectionMatrix, 1, GL_FALSE, glm::value_ptr(PoolLib::_projectionMatrix));
	glProgramUniformMatrix4fv(_shaderProgram, uViewMatrix, 1, GL_FALSE, glm::value_ptr(PoolLib::_viewMatrix));
	glProgramUniformMatrix4fv(_shaderProgram, uModelViewMatrix, 1, GL_FALSE, glm::value_ptr(_modelViewMatrix));

	_balls.setShaderProgram(_shaderProgram);

	_balls.loadLightingUniforms();

	//define render window
	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

	glEnable(GL_DEPTH_TEST);

	glEnable(GL_CULL_FACE);

}
void display()
{
	//clear the color buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

	//translate the table
	glm::mat4 translateModel = glm::translate(PoolLib::_modelMatrix, glm::vec3(0.0f, 0.0f, 0.0f));

	//create view model matrix
	glm::mat4 viewModelMatrix = PoolLib::_viewMatrix * translateModel;

	//get uniform locations
	GLint uModelViewMatrix = glGetUniformLocation(_shaderProgram, "ModelView");

	//link uniform values to program
	glProgramUniformMatrix4fv(_shaderProgram, uModelViewMatrix, 1, GL_FALSE, glm::value_ptr(viewModelMatrix));

	//link render texture to program
	GLint uRenderTexture = glGetUniformLocation(_shaderProgram, "renderTex");
	glProgramUniform1i(_shaderProgram, uRenderTexture, 0);
		
	//draw table
	glBindVertexArray(_tableVAO);
	glDrawArrays(GL_TRIANGLES, 0, _nTableVertices);

	PoolLib::Material material;
	material.ns = 1.0f;
	material.ka = glm::vec3(1.0f);
	material.kd = glm::vec3(1.0f);
	material.ks = glm::vec3(1.0f);

	_balls.loadMaterialUniforms(material, _shaderProgram);

	for (int i = 0; i < _balls.getNumberOfBalls(); i++)
	{
		_balls.setId(i);
		_balls.Render(_ballVertices[i], _ballOrientations[i]);
	}
	if (animStart)
	{
		//check for collision
		if (getCollision())
		{
			animStart = false;
		}
		else
		{
			//update ball position
			_ballVertices[animBallIndex].x += 0.005f;
			_ballVertices[animBallIndex].z += 0.005f;

			//update ball orientation
			_ballOrientations[animBallIndex].x += 0.1f;
			_ballOrientations[animBallIndex].z += 0.1f;

		}
	}

}

void scrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
	//zoom in and out
	if (yoffset > 0)
	{
		_zoomLvl -= _zoomSpeed;
	}
	else
	{
		_zoomLvl += _zoomSpeed;
	}
	
	//clamp zoom level
	_zoomLvl = glm::clamp(_zoomLvl, _minZoom, _maxZoom);

	//update projection matrix
	PoolLib::_projectionMatrix = glm::perspective(glm::radians(45.0f)*_zoomLvl, (GLfloat)SCREEN_WIDTH / (GLfloat)SCREEN_HEIGHT, 0.1f, 100.0f);

	//update uniform value
	GLint uProjectionMatrix = glGetUniformLocation(_shaderProgram, "Projection");
	glProgramUniformMatrix4fv(_shaderProgram, uProjectionMatrix, 1, GL_FALSE, glm::value_ptr(PoolLib::_projectionMatrix));
}
void mouseCallback(GLFWwindow* window, double xpos, double ypos)
{
	if (_firstMouse)
	{
		_lastX = xpos;
		_lastY = ypos;
		_firstMouse = false;
	}
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) != GLFW_PRESS)
	{
		_lastX = xpos;
		_lastY = ypos;
		_firstMouse = false;
		return;
	}

	float xoffset = xpos - _lastX;
	float yoffset = _lastY - ypos;

	_lastX = xpos;
	_lastY = ypos;

	float sensitivity = 0.1f;
	
	//apply horizontal rotation
	PoolLib::_viewMatrix = glm::rotate(PoolLib::_viewMatrix,glm::radians (xoffset * sensitivity), glm::vec3(0.0f, 1.0f, 0.0f));

	//calculate right vector
	glm::vec3 right = glm::normalize(glm::cross(glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)));

	//apply vertical rotation
	PoolLib::_viewMatrix = glm::rotate(PoolLib::_viewMatrix,glm::radians (yoffset * sensitivity), right);

}	

bool getCollision()
{
	float _radius = 0.08f;
	for (int i = 0; i < _balls.getNumberOfBalls(); i++)
	{
		if (i != animBallIndex)
		{
			float distance = glm::distance(_ballVertices[i], _ballVertices[animBallIndex]);
			if (distance <= 2 * _radius)
			{
				std::cout << "Collision detected with balls" << std::endl;
				return true;
			}
		}
		
	}
	if (_ballVertices[animBallIndex].x + _radius >= 2.05f || _ballVertices[animBallIndex].x - _radius <= -2.05f || 
		_ballVertices[animBallIndex].z + _radius >= 1.3f || _ballVertices[animBallIndex].z - _radius <= -1.3f)
	{
		std::cout << "Collision detected with table" << std::endl;
		return true;
	}
	return false;
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
	{
		animStart = true;
		std::cout << "Space pressed" << std::endl;
	}
}

void characterCallback(GLFWwindow* window, unsigned int codepoint)
{
	int lightMode;

	switch (codepoint)
	{
	case '1':
		lightMode = 1;
		glProgramUniform1i(_shaderProgram, glGetUniformLocation(_shaderProgram, "lightModel"), lightMode);
		break;

	case '2':
		lightMode = 2;
		glProgramUniform1i(_shaderProgram, glGetUniformLocation(_shaderProgram, "lightModel"), lightMode);
		break;

	case '3':
		lightMode = 3;
		glProgramUniform1i(_shaderProgram, glGetUniformLocation(_shaderProgram, "lightModel"), lightMode);
		break;

	case '4':
		lightMode = 4;
		glProgramUniform1i(_shaderProgram, glGetUniformLocation(_shaderProgram, "lightModel"), lightMode);
		break;

	default:
		break;
	}
}
std::vector<glm::vec3> generateRandomBallPositions(int numBalls, float tableWidth, float tableHeight, float ballRadius) {
	std::vector<glm::vec3> positions;
	std::srand(static_cast<unsigned int>(std::time(0)));

	auto isTouching = [&](const glm::vec3& pos, const std::vector<glm::vec3>& positions) {
		for (const auto& existingPos : positions) {
			if (glm::distance(pos, existingPos) < 2 * ballRadius) {
				return true;
			}
		}
		return false;
		};

	for (int i = 0; i < numBalls; ++i) {
		glm::vec3 newPos;
		bool validPosition = false;

		while (!validPosition) {
			newPos = glm::vec3(
				(static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX)) * tableWidth - (tableWidth / 2.0f),
				0.33f,
				(static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX)) * tableHeight - (tableHeight / 2.0f)
			);
			validPosition = !isTouching(newPos, positions);
		}

		positions.push_back(newPos);
	}

	return positions;
}
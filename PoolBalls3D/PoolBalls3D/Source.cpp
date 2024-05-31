/*
 * Discipline: 3D Programming
 * Subject: Pool Balls 3D Project
 * Authors: João Tavares nº 21871, Diogo Silva nº 22369, Ademar Valente nº 23155, José Lourenço nº23496
 * Date: 31/05/2024
 * File: Source.cpp
 * Description: Main file of the project
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

#include "Source.h"
#include "LoadShaders.h"
#include "PoolLib.h"

#pragma endregion
//-----------------------------------------------------------------------------------------------------
#pragma region Variables

// Table variables
const GLuint _nTableVertices = 36;
GLuint _tableVBO;
GLuint _tableVAO;
float xCoord = 2.0f;
float yCoord = 0.25f;
float zCoord = 1.25f;

// Camera variables
GLfloat _angle = -10.0f;
glm::vec3 _cameraPosition = glm::vec3(0.0f, 1.0f, 5.0f);

// Shader Program variables
GLuint _shaderProgram;

// Lighting variables
bool _ambient = false;
bool _directional = false;
bool _point = false;
bool _spot = false;

// Mouse variables
float _lastX = 0.0f;
float _lastY = 0.0f;
bool _firstMouse = true;

// Zoom variables
float _zoomLvl = 1.0f;
float _minZoom = 0.1f;
float _maxZoom = 3.5f;
float _zoomSpeed = 0.1f;

// Ball animation variables
int animBallIndex = 5;	// Ball 6
bool animStart = false;

// Ball variables
PoolLib::RenderBalls _balls(15);
std::vector<glm::vec3> _ballVertices = {generateRandomBallPositions(_balls.getNumberOfBalls(), xCoord * 1.75f, zCoord * 1.75f, PoolLib::_ballRadius)};
std::vector<glm::vec3> _ballOrientations = {
	glm::vec3(0.0f),	// Ball 1
	glm::vec3(0.0f),	// Ball 2
	glm::vec3(0.0f),	// Ball 3
	glm::vec3(0.0f),	// Ball 4
	glm::vec3(0.0f),	// Ball 5
	glm::vec3(0.0f),	// Ball 6
	glm::vec3(0.0f),	// Ball 7
	glm::vec3(0.0f),	// Ball 8
	glm::vec3(0.0f),	// Ball 9
	glm::vec3(0.0f),	// Ball 10
	glm::vec3(0.0f),	// Ball 11
	glm::vec3(0.0f),	// Ball 12	
	glm::vec3(0.0f),	// Ball 13
	glm::vec3(0.0f),	// Ball 14
	glm::vec3(0.0f)		// Ball 15
};

#pragma endregion
//-----------------------------------------------------------------------------------------------------
#pragma region Main Function

int main() {

	// In case there is an error with GLFW
	glfwSetErrorCallback(printErrorCallback);

	// Initialize GLFW and check for errors
	if (!glfwInit())
	{
		std::cerr << "Failed to initialize GLFW." << std::endl;
		return -1;
	}

	// Create a window
	GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_TITLE, NULL, NULL);

	// Check if the window was created successfully
	if (!window)
	{
		glfwTerminate();
		std::cerr << "Failed to create window." << std::endl;
		return -1;
	}

	// Make the window's context current
	glfwMakeContextCurrent(window);

	// Initialize GLEW
	if (glewInit() != GLEW_OK)
	{
		std::cerr << "Failed to initialize GLEW." << std::endl;
		glfwTerminate();
		return -1;
	}

	// Initialize the program
	init();

	// Zoom callback for zooming in and out
	glfwSetScrollCallback(window, scrollCallback);

	// Mouse callback for camera rotation
	glfwSetCursorPosCallback(window, mouseCallback);

	// Key callback for starting the ball animation
	glfwSetKeyCallback(window, keyCallback);

	// Character callback for enabling and disabling the different light models
	glfwSetCharCallback(window, characterCallback);

	// Loop until the user closes the window
	while (!glfwWindowShouldClose(window))
	{
		// Render the scene
		display();

		// Swap front and back buffers
		glfwSwapBuffers(window);

		// Poll for and process events
		glfwPollEvents();
	}

	// Terminate GLFW and the program
	glfwTerminate();
	return 0;

}

#pragma endregion
//-----------------------------------------------------------------------------------------------------
#pragma region Secondary Functions

void init() {

	// Table vertices colors
	std::vector<glm::vec3> colors{
		glm::vec3(0.0f, 0.0f, 1.0f), // Blue
		glm::vec3(1.0f, 0.0f, 0.0f), // Red
		glm::vec3(0.0f, 1.0f, 0.0f), // Green
		glm::vec3(0.0f, 1.0f, 1.0f), // Cyan
		glm::vec3(1.0f, 1.0f, 0.0f), // Yellow
		glm::vec3(1.0f, 1.0f, 1.0f)  // White
	};

	// Table texture coordinates
	float xTexture = 0.0f;
	float yTexture = 0.0f;

	// Table vertices data
	GLfloat _tableVertices[_nTableVertices * 8] = {
		// X+ face
		// Triangle 1
		xCoord, -yCoord, zCoord,		 colors[0].r, colors[0].g, colors[0].b,		xTexture, yTexture,
		xCoord, -yCoord, -zCoord,	     colors[0].r, colors[0].g, colors[0].b,		xTexture, yTexture,
		xCoord, yCoord, zCoord,		     colors[0].r, colors[0].g, colors[0].b,		xTexture, yTexture,
		// Triangle 2
		xCoord, yCoord, zCoord,		     colors[0].r, colors[0].g, colors[0].b,		xTexture, yTexture,
		xCoord, -yCoord, -zCoord,	     colors[0].r, colors[0].g, colors[0].b,		xTexture, yTexture,
		xCoord, yCoord, -zCoord,	     colors[0].r, colors[0].g, colors[0].b,		xTexture, yTexture,

		//---------------------------------------------------------------------------------------------
		// X- face
		// Triangle 1
		-xCoord, -yCoord, -zCoord,	     colors[1].r, colors[1].g, colors[1].b,		xTexture, yTexture,
		-xCoord, -yCoord, zCoord,	     colors[1].r, colors[1].g, colors[1].b,		xTexture, yTexture,
		-xCoord, yCoord, -zCoord,	     colors[1].r, colors[1].g, colors[1].b,		xTexture, yTexture,
		// Triangle 2
		-xCoord, yCoord, -zCoord,	     colors[1].r, colors[1].g, colors[1].b,		xTexture, yTexture,
		-xCoord, -yCoord, zCoord,	     colors[1].r, colors[1].g, colors[1].b,		xTexture, yTexture,
		-xCoord, yCoord, zCoord,	     colors[1].r, colors[1].g, colors[1].b,		xTexture, yTexture,

		//---------------------------------------------------------------------------------------------
		// Y+ face
		// Triangle 1
		-xCoord, yCoord, zCoord,	     colors[2].r, colors[2].g, colors[2].b,		xTexture, yTexture,
		xCoord, yCoord,  zCoord,	     colors[2].r, colors[2].g, colors[2].b,		xTexture, yTexture,
		-xCoord, yCoord, -zCoord,	     colors[2].r, colors[2].g, colors[2].b,		xTexture, yTexture,
		// Triangle 2
		-xCoord, yCoord, -zCoord,	     colors[2].r, colors[2].g, colors[2].b,		xTexture, yTexture,
		xCoord, yCoord,  zCoord,	     colors[2].r, colors[2].g, colors[2].b,		xTexture, yTexture,
		xCoord, yCoord, -zCoord,	     colors[2].r, colors[2].g, colors[2].b,		xTexture, yTexture,

		//---------------------------------------------------------------------------------------------
		// Y- face
		// Triangle 1
		-xCoord, -yCoord, -zCoord,	     colors[3].r, colors[3].g, colors[3].b,		xTexture, yTexture,
		xCoord, -yCoord, -zCoord,	     colors[3].r, colors[3].g, colors[3].b,		xTexture, yTexture,
		-xCoord, -yCoord, zCoord,	     colors[3].r, colors[3].g, colors[3].b,		xTexture, yTexture,
		// Triangle 2
		-xCoord, -yCoord, zCoord,	     colors[3].r, colors[3].g, colors[3].b,		xTexture, yTexture,
		xCoord, -yCoord, -zCoord,	     colors[3].r, colors[3].g, colors[3].b,		xTexture, yTexture,
		xCoord, -yCoord, zCoord,	     colors[3].r, colors[3].g, colors[3].b,		xTexture, yTexture,

		//---------------------------------------------------------------------------------------------
		// Z+ face
		// Triangle 1
		-xCoord, -yCoord, zCoord,	     colors[4].r, colors[4].g, colors[4].b,		xTexture, yTexture,
		xCoord, -yCoord, zCoord,	     colors[4].r, colors[4].g, colors[4].b,		xTexture, yTexture,
		-xCoord, yCoord, zCoord,	     colors[4].r, colors[4].g, colors[4].b,		xTexture, yTexture,
		// Triangle 2
		 -xCoord, yCoord, zCoord,	     colors[4].r, colors[4].g, colors[4].b,		xTexture, yTexture,
		xCoord, -yCoord,  zCoord,	     colors[4].r, colors[4].g, colors[4].b,		xTexture, yTexture,
		xCoord, yCoord, zCoord,			 colors[4].r, colors[4].g, colors[4].b,		xTexture, yTexture,

		//---------------------------------------------------------------------------------------------
		// Z- face
		// Triangle 1
		xCoord, -yCoord, -zCoord,	     colors[5].r, colors[5].g, colors[5].b,		xTexture, yTexture,
		-xCoord, -yCoord, -zCoord,	     colors[5].r, colors[5].g, colors[5].b,		xTexture, yTexture,
		xCoord, yCoord, -zCoord,	     colors[5].r, colors[5].g, colors[5].b,		xTexture, yTexture,
		// Triangle 2
		xCoord, yCoord, -zCoord,	     colors[5].r, colors[5].g, colors[5].b,		xTexture, yTexture,
		-xCoord, -yCoord, -zCoord,	     colors[5].r, colors[5].g, colors[5].b,		xTexture, yTexture,
		-xCoord, yCoord, -zCoord,	     colors[5].r, colors[5].g, colors[5].b,		xTexture, yTexture
	};

	// Generate the VAO used for the table
	glGenVertexArrays(1, &_tableVAO);
	glBindVertexArray(_tableVAO);

	// Generate the VBO used for the table
	glGenBuffers(1, &_tableVBO);
	glBindBuffer(GL_ARRAY_BUFFER, _tableVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(_tableVertices), _tableVertices, GL_STATIC_DRAW);

	// Set vertex attribute pointers for the table
	// Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	// Normal attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);
	// Texture attribute
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);

	// Unbind current VAO
	glBindVertexArray(0);

	// Fetch and load each ball texture
	for (int i = 1; i <= _balls.getNumberOfBalls(); i++)
	{
		std::string texturePath = "textures/Ball" + std::to_string(i) + ".obj";
		_balls.Load(texturePath);
	}

	// Generates the VAO for the balls
	// Generates the VBOs for the each ball
	// Sends balls vertice data to the GPU
	_balls.Install();

	// Fetch the vertex and fragment shaders
	shaderInfo shaders[] = {
		{ GL_VERTEX_SHADER, "shaders/poolballs.vert" },
		{ GL_FRAGMENT_SHADER, "shaders/poolballs.frag" },
		{ GL_NONE, NULL }
	};
	
	// Load shaders
	_shaderProgram = loadShaders(shaders);

	// Bind shader program
	glUseProgram(_shaderProgram);

	// Fetch attribute locations
	GLint vPosition = glGetProgramResourceLocation (_shaderProgram, GL_PROGRAM_INPUT, "vPosition");
	GLint vNormal = glGetProgramResourceLocation(_shaderProgram, GL_PROGRAM_INPUT, "vNormal");
	GLint vTexCoords = glGetProgramResourceLocation(_shaderProgram, GL_PROGRAM_INPUT, "vTextureCoords");
	
	// Set vertex attribute pointers for the balls
	// Position attribute
	glVertexAttribPointer(vPosition, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(vPosition);
	// Normal attribute
	glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(vNormal);
	// Texture attribute
	glVertexAttribPointer(vTexCoords, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(vTexCoords);

	// Matrixes declarations
	PoolLib::_viewMatrix = glm::lookAt(_cameraPosition, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	PoolLib::_modelMatrix = glm::rotate(glm::mat4(1.0f), _angle, glm::vec3(0.0f,1.0f,0.0f));
	glm::mat4 _modelViewMatrix = PoolLib::_viewMatrix * PoolLib::_modelMatrix;
	PoolLib::_projectionMatrix = glm::perspective(45.0f, (GLfloat)SCREEN_WIDTH / (GLfloat)SCREEN_HEIGHT, 0.1f, 100.0f);
	PoolLib::_normalMatrix = glm::inverseTranspose(glm::mat3(_modelViewMatrix));

	// Fecth uniform locations
	GLint uModelMatrix = glGetUniformLocation(_shaderProgram, "Model");
	GLint uNormalMatrix = glGetUniformLocation(_shaderProgram, "NormalMatrix");
	GLint uProjectionMatrix = glGetUniformLocation(_shaderProgram, "Projection");
	GLint uViewMatrix = glGetUniformLocation(_shaderProgram, "View");
	GLint uModelViewMatrix = glGetUniformLocation(_shaderProgram, "ModelView");

	// Link uniforms values to program and their respective matrixes
	glProgramUniformMatrix4fv(_shaderProgram, uModelMatrix, 1, GL_FALSE, glm::value_ptr(PoolLib::_modelMatrix));
	glProgramUniformMatrix3fv(_shaderProgram, uNormalMatrix, 1, GL_FALSE, glm::value_ptr(PoolLib::_normalMatrix));
	glProgramUniformMatrix4fv(_shaderProgram, uProjectionMatrix, 1, GL_FALSE, glm::value_ptr(PoolLib::_projectionMatrix));
	glProgramUniformMatrix4fv(_shaderProgram, uViewMatrix, 1, GL_FALSE, glm::value_ptr(PoolLib::_viewMatrix));
	glProgramUniformMatrix4fv(_shaderProgram, uModelViewMatrix, 1, GL_FALSE, glm::value_ptr(_modelViewMatrix));

	// Set the shader program to the balls
	_balls.setShaderProgram(_shaderProgram);

	// Load lighting uniforms
	_balls.loadLightingUniforms();

	// Define render window
	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

	// Enable depth test
	glEnable(GL_DEPTH_TEST);

	// Enable face culling
	glEnable(GL_CULL_FACE);

}

void display() {

	// Clear the color and depth buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

	// Get a translation matrix for the model
	glm::mat4 translateModel = glm::translate(PoolLib::_modelMatrix, glm::vec3(0.0f, 0.0f, 0.0f));

	// Create a view model matrix
	glm::mat4 viewModelMatrix = PoolLib::_viewMatrix * translateModel;

	// Get view model matrix uniform location
	GLint uModelViewMatrix = glGetUniformLocation(_shaderProgram, "ModelView");

	// Link uniform values to program
	glProgramUniformMatrix4fv(_shaderProgram, uModelViewMatrix, 1, GL_FALSE, glm::value_ptr(viewModelMatrix));

	// Fetch uniform location for the render texture
	GLint uRenderTexture = glGetUniformLocation(_shaderProgram, "renderTex");

	// Set render texture uniform value to shader program with index 0
	glProgramUniform1i(_shaderProgram, uRenderTexture, 0);
		
	// Draw the table
	glBindVertexArray(_tableVAO);
	glDrawArrays(GL_TRIANGLES, 0, _nTableVertices);

	// Material properties used for the balls
	PoolLib::Material material;
	material.ns = 1.0f;
	material.ka = glm::vec3(1.0f);
	material.kd = glm::vec3(1.0f);
	material.ks = glm::vec3(1.0f);


	// Load material uniforms for the balls
	_balls.loadMaterialUniforms(material, _shaderProgram);

	// Draw the balls
	for (int i = 0; i < _balls.getNumberOfBalls(); i++)
	{
		_balls.setId(i);
		_balls.Render(_ballVertices[i], _ballOrientations[i]);
	}

	// Ball animation
	if (animStart)
	{
		// Check for collision
		if (getCollision())
		{
			animStart = false;
		}
		else
		{
			// Update ball position
			_ballVertices[animBallIndex].x += 0.005f;
			_ballVertices[animBallIndex].z += 0.005f;

			// Update ball orientation
			_ballOrientations[animBallIndex].x += 2.0f;
			_ballOrientations[animBallIndex].z += 2.0f;
		}
	}

}

// Callback used for printing errors
void printErrorCallback(int code, const char* description) {
	std::cout << description << std::endl;
}

// Callback used for zooming in and out
void scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
	
	// Zoom in and out
	if (yoffset > 0)
	{
		_zoomLvl -= _zoomSpeed;
	}
	else
	{
		_zoomLvl += _zoomSpeed;
	}
	
	// Clamps the zoom level between the minimum and maximum zoom
	_zoomLvl = glm::clamp(_zoomLvl, _minZoom, _maxZoom);

	// Update projection matrix
	PoolLib::_projectionMatrix = glm::perspective(glm::radians(45.0f)*_zoomLvl, (GLfloat)SCREEN_WIDTH / (GLfloat)SCREEN_HEIGHT, 0.1f, 100.0f);

	// Update uniform value of projection
	GLint uProjectionMatrix = glGetUniformLocation(_shaderProgram, "Projection");
	glProgramUniformMatrix4fv(_shaderProgram, uProjectionMatrix, 1, GL_FALSE, glm::value_ptr(PoolLib::_projectionMatrix));
}

// Callback used for camera rotation
void mouseCallback(GLFWwindow* window, double xpos, double ypos) {

	// Gets the beginning of the mouse movement
	if (_firstMouse)
	{
		_lastX = xpos;
		_lastY = ypos;
		_firstMouse = false;
	}

	// Gets current mouse position
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) != GLFW_PRESS)
	{
		_lastX = xpos;
		_lastY = ypos;
		_firstMouse = false;
		return;
	}

	// Calculates the offset of the mouse movement
	float xoffset = xpos - _lastX;
	float yoffset = _lastY - ypos;

	// Updates the current mouse position
	_lastX = xpos;
	_lastY = ypos;

	float sensitivity = 0.1f;
	
	// Apply horizontal rotation
	PoolLib::_viewMatrix = glm::rotate(PoolLib::_viewMatrix,glm::radians (xoffset * sensitivity), glm::vec3(0.0f, 1.0f, 0.0f));

	// Calculate right vector
	glm::vec3 right = glm::normalize(glm::cross(glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)));

	// Apply vertical rotation
	PoolLib::_viewMatrix = glm::rotate(PoolLib::_viewMatrix,glm::radians (yoffset * sensitivity), right);

}	

// Callback used for starting the ball animation
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
	{
		// Sets the animation to start
		animStart = true;
	}
}

// Callback used for enabling and disabling the different light models
void characterCallback(GLFWwindow* window, unsigned int codepoint) {

	int lightMode;

	switch (codepoint)
	{
		// Key 1 - Ambient light
	case '1':
		if (_ambient)
		{
			_ambient = false;
			lightMode = 0;
			// Aplly the light model to the shader program
			glProgramUniform1i(_shaderProgram, glGetUniformLocation(_shaderProgram, "lightModel"), lightMode);
			std::cout << "Lights disabled." << std::endl;
			break;
		}
		else
		{
			_ambient = true;
			lightMode = 1;
			// Aplly the light model to the shader program
			glProgramUniform1i(_shaderProgram, glGetUniformLocation(_shaderProgram, "lightModel"), lightMode);
			std::cout << "Ambient light enabled." << std::endl;
			break;
		}

		// Key 2 - Directional light
	case '2':
		if (_directional)
		{
			_directional = false;
			lightMode = 0;
			// Aplly the light model to the shader program
			glProgramUniform1i(_shaderProgram, glGetUniformLocation(_shaderProgram, "lightModel"), lightMode);
			std::cout << "Lights disabled." << std::endl;
			break;
		}
		else
		{
			_directional = true;
			lightMode = 2;
			// Aplly the light model to the shader program
			glProgramUniform1i(_shaderProgram, glGetUniformLocation(_shaderProgram, "lightModel"), lightMode);
			std::cout << "Directional light enabled." << std::endl;
			break;
		}

		// Key 3 - Point light
	case '3':
		if (_point)
		{
			_point = false;
			lightMode = 0;
			// Aplly the light model to the shader program
			glProgramUniform1i(_shaderProgram, glGetUniformLocation(_shaderProgram, "lightModel"), lightMode);
			std::cout << "Lights disabled." << std::endl;
			break;
		}
		else
		{
			_point = true;
			lightMode = 3;
			// Aplly the light model to the shader program
			glProgramUniform1i(_shaderProgram, glGetUniformLocation(_shaderProgram, "lightModel"), lightMode);
			std::cout << "Point light enabled." << std::endl;
			break;
		}	

		// Key 4 - Spot light
	case '4':
		if (_spot)
		{
			_spot = false;
			lightMode = 0;
			// Aplly the light model to the shader program
			glProgramUniform1i(_shaderProgram, glGetUniformLocation(_shaderProgram, "lightModel"), lightMode);
			std::cout << "Lights disabled." << std::endl;
			break;
		}
		else
		{
			_spot = true;
			lightMode = 4;
			// Aplly the light model to the shader program
			glProgramUniform1i(_shaderProgram, glGetUniformLocation(_shaderProgram, "lightModel"), lightMode);
			std::cout << "Spot light enabled." << std::endl;
			break;
		}		
	default:
		break;
	}
}

bool getCollision() {
	
	float ballDiameter = 2 * PoolLib::_ballRadius;

	// Checks for collisions between the animated ball and the other balls
	for (int i = 0; i < _balls.getNumberOfBalls(); i++)
	{
		if (i != animBallIndex)
		{
			float distance = glm::distance(_ballVertices[i], _ballVertices[animBallIndex]);
			if (distance <= 2 * PoolLib::_ballRadius)
			{
				std::cout << "Collision detected with another ball." << std::endl;
				// Ball collision detected
				return true;
			}
		}

	}

	// Checks for collisions between the animated ball and the table edges
	if (_ballVertices[animBallIndex].x + ballDiameter >= xCoord || _ballVertices[animBallIndex].x - ballDiameter <= -xCoord ||
		_ballVertices[animBallIndex].z + ballDiameter >= zCoord || _ballVertices[animBallIndex].z - ballDiameter <= -zCoord )
	{
		std::cout << "Collision detected with table edges." << std::endl;
		// Table edge collision detected
		return true;
	}

	// No collision detected
	return false;
}

// Function used to generate random ball positions
// The function receives the number of balls, the table width and height, and the ball radius
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

#pragma endregion
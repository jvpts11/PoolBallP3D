/*
 * Discipline: 3D Programming
 * Subject: Pool Balls 3D Project
 * Authors: João Tavares nº 21871, Diogo Silva nº 22369, Ademar Valente nº 23155, José Lourenço nº23496
 * Date: 17/05/2024
 * File: Source.h
 * Description: Signatures related to the implementation of the Source class (Main file of the project).
 */
//-----------------------------------------------------------------------------------------------------
#pragma once

#pragma region Window Settings

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define SCREEN_TITLE "Pool Balls 3D"

#pragma endregion
//-----------------------------------------------------------------------------------------------------
#pragma region Functions

// Initalize the program and it's data
void init();

// Update the program
void display();

// Callback used for printing errors
void printErrorCallback(int code, const char* description);

// Callback used for zooming in and out
void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);

// Callback used for moving the camera
void mouseCallback(GLFWwindow* window, double xpos, double ypos);

// Funtion to check for collisions
bool getCollision(void);

// Callback used for starting the ball animation
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

// Callback used for enabling and disabling the different light models 
void characterCallback(GLFWwindow* window, unsigned int codepoint);

// Function to generate random ball positions
std::vector<glm::vec3> generateRandomBallPositions(int numBalls, float tableWidth, float tableHeight, float ballRadius);

#pragma endregion
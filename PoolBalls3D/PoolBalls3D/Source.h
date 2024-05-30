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

void init();
void display();
void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);
void mouseCallback(GLFWwindow* window, double xpos, double ypos);
bool getCollision(void);
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
void characterCallback(GLFWwindow* window, unsigned int codepoint);
std::vector<glm::vec3> generateRandomBallPositions(int numBalls, float tableWidth, float tableHeight, float ballRadius);

#pragma endregion
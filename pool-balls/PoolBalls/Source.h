/*
@3D Programming

@description File with all signatures related to the application itself.
@file Source.h

@authors João Tavares nº21871, Diogo Silva nº22369, Ademar Valente nº23155, José lourenço nº23496
@date 19/05/2024
*/


#pragma once

#ifndef SOURCE_H
#define SOURCE_H 1

#pragma region importations

#include <GLFW\glfw3.h>

#pragma endregion


#pragma region constants

#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720
#define SCREEN_NAME "Pool Game"

#pragma endregion


#pragma region program functions

	void init(void);
	void display(void);
	void loadSceneLighting(void);
	bool isColliding(void);

#pragma endregion


#pragma region glfw library callback functions

	void printErrorCallback(int code, const char* description);
	void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);
	void mouseCallback(GLFWwindow* window, double xpos, double ypos);
	void charCallback(GLFWwindow* window, unsigned int codepoint);

#pragma endregion

#endif
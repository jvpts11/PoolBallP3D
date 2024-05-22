
#pragma region imports

#define GLEW_STATIC
#include <GL\glew.h>
#include <GLFW\glfw3.h>
#include <iostream>

#pragma comment(lib, "glew32s.lib")
#pragma comment(lib, "glfw3.lib")
#pragma comment(lib, "opengl32.lib")

#include <windows.h>

#pragma endregion


int main(void) {
	GLFWwindow* window;

	glfwSetErrorCallback(errorCallback);

	if (!glfwInit()) return -1;

	window = glfwCreateWindow(800, 600, "My 1st P3D Project", nullptr, nullptr);
	if (!window) {
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);

	while (!glfwWindowShouldClose(window)) {
		glClear(GL_COLOR_BUFFER_BIT);
		glfwSwapBuffers(window);

		glfwPollEvents();
	}

	glfwDestroyWindow(window);

	glfwTerminate();
	return 0;
}

void errorCallback(int code, const char* description) {
	std::cout << description << std::endl;
}
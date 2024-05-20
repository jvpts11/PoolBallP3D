#pragma once

#ifndef LOAD_SHADERS_H
#define LOAD_SHADERS_H 1

#pragma region importações

#include <GL\gl.h>

#pragma endregion


#pragma region estruturas

typedef struct {
	GLenum type;
	const char* filename;
	GLuint shader;
} ShaderInfo;

#pragma endregion


#pragma region funções

//static const GLchar* readShader(const char* filename);
GLuint loadShaders(ShaderInfo*);

#pragma endregion

#endif
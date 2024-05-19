/*
@3D Programming

@description File with all signatures related to shader loading.
@file Shaders.h

@authors Jo�o Tavares n�21871, Diogo Silva n�22369, Ademar Valente n�23155, Jos� louren�o n�23496
@date 19/05/2024
*/


#pragma once

#ifndef LOAD_SHADERS_H
#define LOAD_SHADERS_H 1

#pragma region importations

#include <GL\gl.h>

#pragma endregion


#pragma region structures

typedef struct {
	GLenum type;
	const char* filename;
	GLuint shader;
} ShaderInfo;

#pragma endregion


#pragma region functions

static const GLchar* readShader(const char* filename);
GLuint loadShaders(ShaderInfo* shaders);

#pragma endregion

#endif
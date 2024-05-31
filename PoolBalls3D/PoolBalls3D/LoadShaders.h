/*
 * Discipline: 3D Programming
 * Subject: Pool Balls 3D Project
 * Authors: Jo�o Tavares n� 21871, Diogo Silva n� 22369, Ademar Valente n� 23155, Jos� Louren�o n�23496
 * Date: 31/05/2024
 * File: LoadShaders.h
 * Description: Signatures related to the implementation of the shader loading and reading functions.
 */
//-----------------------------------------------------------------------------------------------------
#pragma once

#include <GL\gl.h>

// Structure that represents the shader information
typedef struct {
	GLenum type;
	const char* filename;
	GLuint shader;
}shaderInfo;

// Function signatures
	// Function that loads the shaders
GLuint loadShaders(shaderInfo* shaders);

	// Function that reads the shader
static const char* readShader(const char* filename);

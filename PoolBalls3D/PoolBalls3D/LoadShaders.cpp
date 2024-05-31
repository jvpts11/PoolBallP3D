/*
 * Discipline: 3D Programming
 * Subject: Pool Balls 3D Project
 * Authors: João Tavares nº 21871, Diogo Silva nº 22369, Ademar Valente nº 23155, José Lourenço nº23496
 * Date: 31/05/2024
 * File: LoadShaders.cpp
 * Description: Implementation of the shader loading and reading functions.
 */
//-----------------------------------------------------------------------------------------------------
#pragma region Imports

#include <iostream>
#include <fstream>

#define GLEW_STATIC
#include <GL\glew.h>

#include "loadShaders.h"

#pragma endregion
//-----------------------------------------------------------------------------------------------------
#pragma region Functions

GLuint loadShaders(shaderInfo* shaders) {

	// Check if the shader is not NULL
	if (shaders == NULL)
	{
		return 0;
	}

	// Create the program
	GLuint program = glCreateProgram();

	for (shaderInfo* entry = shaders; entry->type != GL_NONE; entry++)
	{
		// Create the shader with thes specified type
		GLuint shader = glCreateShader(entry->type);

		// Read the shader code from the file
		const char* shaderCode = readShader(entry->filename);

		// Check for problems reading the shader code
		if (shaderCode == NULL)
		{
			// Delete the shaders
			for (shaderInfo* entry = shaders; entry->type != GL_NONE; entry++)
			{
				glDeleteShader(entry->shader);
			}
			return 0;
		}

		// Set the shader source code
		glShaderSource(shader, 1, &shaderCode, NULL);

		// Free the memory allocated for the shader code
		delete[] shaderCode;

		// Compile the shader
		glCompileShader(shader);

		// Get the compilation status
		GLint compiled;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);

		// Check if the shader compiled successfully
		if (!compiled)
		{
			GLint length;
			glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);

			GLchar* log = new GLchar[length + 1];
			glGetShaderInfoLog(shader, length, &length, log);
			std::cerr << "Shader compilation failed: " << log << "." << std::endl;
			delete[] log;

			// Delete the shaders if the shader compilation failed
			for (shaderInfo* entry = shaders; entry->type != GL_NONE; entry++)
			{
				glDeleteShader(entry->shader);
			}

			return 0;
		}

		// Attach the shader to the program
		glAttachShader(program, shader);
	}

	// Link the program to OpenGL context
	glLinkProgram(program);

	// Get the linking status
	GLint linked;
	glGetProgramiv(program, GL_LINK_STATUS, &linked);

	// Check if the program linked successfully
	if (!linked)
	{
		GLint length;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);

		GLchar* log = new GLchar[length + 1];
		glGetProgramInfoLog(program, length, &length, log);
		std::cerr << "Shader linking failed: " << log << "." << std::endl;
		delete[] log;

		// Delete the shaders if the program linking failed
		for (shaderInfo* entry = shaders; entry->type != GL_NONE; entry++)
		{
			glDeleteShader(entry->shader);
		}

		return 0;
	}

	return program;
}

static const char* readShader(const char* filename) {

	// Open the file in binary mode and at the end
	std::ifstream file(filename, std::ifstream::ate | std::ifstream::binary);

	// Check if the file was opened successfully
	if (!file.is_open())
	{
		std::cerr << "Failed to open file: " << filename << "." << std::endl;
		return NULL;
	}

	// Get the size of the file
	std::streampos size = file.tellg();

	// Reposition the file pointer to the beginning
	file.seekg(0, std::ifstream::beg);

	// Allocate memory for the file content
	char* buffer = new char[int(size) + 1];

	// Read the file content
	file.read(buffer, size);

	// Close the reading string
	buffer[size] = '\0';

	// Close the file
	file.close();

	return buffer;
}

#pragma endregion
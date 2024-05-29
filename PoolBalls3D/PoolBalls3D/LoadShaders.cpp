/*
 * Discipline: 3D Programming
 * Subject: Pool Balls 3D Project
 * Authors: João Tavares nº 21871, Diogo Silva nº 22369, Ademar Valente nº 23155, José Lourenço nº23496
 * Date: 17/05/2024
 * File: LoadShaders.cpp
 * Description: Implementation of the LoadShaders class.
 */

#pragma region Imports

#include <iostream>
#include <fstream>

#define GLEW_STATIC
#include <GL\glew.h>

#include "loadShaders.h"

#pragma endregion

GLuint loadShaders(shaderInfo* shaders)
{
	if (shaders == NULL)
	{
		return 0;
	}

	GLuint program = glCreateProgram();

	for (shaderInfo* entry = shaders; entry->type != GL_NONE; entry++)
	{
		GLuint shader = glCreateShader(entry->type);

		const char* shaderCode = readShader(entry->filename);
		if (shaderCode == NULL)
		{
			for (shaderInfo* entry = shaders; entry->type != GL_NONE; entry++)
			{
				glDeleteShader(entry->shader);
			}
			return 0;
		}

		glShaderSource(shader, 1, &shaderCode, NULL);
		delete[] shaderCode;

		glCompileShader(shader);

		GLint compiled;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
		if (!compiled)
		{
			GLint length;
			glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);

			GLchar* log = new GLchar[length + 1];
			glGetShaderInfoLog(shader, length, &length, log);
			std::cerr << "Shader compilation failed: " << log << std::endl;
			delete[] log;

			for (shaderInfo* entry = shaders; entry->type != GL_NONE; entry++)
			{
				glDeleteShader(entry->shader);
			}

			return 0;
		}

		glAttachShader(program, shader);
		//entry->shader = shader;
	}

	glLinkProgram(program);

	GLint linked;
	glGetProgramiv(program, GL_LINK_STATUS, &linked);
	if (!linked)
	{
		GLint length;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);

		GLchar* log = new GLchar[length + 1];
		glGetProgramInfoLog(program, length, &length, log);
		std::cerr << "Shader linking failed: " << log << std::endl;
		delete[] log;

		for (shaderInfo* entry = shaders; entry->type != GL_NONE; entry++)
		{
			glDeleteShader(entry->shader);
		}

		return 0;
	}

	return program;
}

static const char* readShader(const char* filename)
{
	std::ifstream file(filename, std::ifstream::ate | std::ifstream::binary);
	if (!file.is_open())
	{
		std::cerr << "Failed to open file: " << filename << std::endl;
		return NULL;
	}

	std::streampos size = file.tellg();

	file.seekg(0, std::ifstream::beg);

	char* buffer = new char[int(size) + 1];

	file.read(buffer, size);
	buffer[size] = '\0';

	file.close();

	return buffer;
}


/*
@3D Programming

@description File related to the vertex shader.
@file Pool.vert

@authors João Tavares nº21871, Diogo Silva nº22369, Ademar Valente nº23155, José lourenço nº23496
@date 19/05/2024
*/


#version 440 core

layout(location = 0) in vec3 vPosition;
layout(location = 1) in vec3 vColor;
layout(location = 2) in vec2 vTextureCoord;

layout(location = 0) out vec3 color;
layout(location = 1) out vec2 textureCoord;

layout(location = 2) out vec3 vPositionEyeSpace;
layout(location = 3) out vec3 vNormalEyeSpace;
layout(location = 4) out vec3 textureVector;
layout(location = 5) out vec3 fPosition;

uniform mat4 Model;
uniform mat4 View;
uniform mat4 ModelView;
uniform mat4 Projection;
uniform mat3 NormalMatrix;

void main()
{
    // posição do vértice de entrada
    gl_Position = Projection * ModelView * vec4(vPosition, 1.0);
 
    // cor do vértice de entrada
    color = vColor;

    // coordenadas de textura do vértice
	textureCoord = vTextureCoord;

    // posição do vértice em coordenadas de olho
	vPositionEyeSpace = (ModelView * vec4(vPosition, 1.0)).xyz;

	// normaliza a normal do vértice
	vNormalEyeSpace = normalize(NormalMatrix * vColor);

    // posição do vértice de saída
	fPosition = vec3(Model * vec4(vPosition, 1.0f));
}
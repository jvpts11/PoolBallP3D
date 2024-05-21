#version 440 core

layout(location = 0) in vec3 vPosition;
layout(location = 1) in vec3 vColors;
layout(location = 2) in vec2 vTextureCoords;

layout(location = 0) out vec3 color;
layout(location = 1) out vec2 textureCoord;

layout(location = 2) out vec3 vPositionEyeSpace;
layout(location = 3) out vec3 vNormalEyeSpace;
layout(location = 4) out vec3 textureVector;
layout(location = 5) out vec3 fragPos;

uniform mat4 Model;
uniform mat4 View;
uniform mat4 ModelView;		// View * Model
uniform mat4 Projection;
uniform mat3 NormalMatrix;

void main()
{
    gl_Position = Projection * ModelView * vec4(vPosition, 1.0);
    color = vColors;

    // Posição do vértice em coordenadas do olho.
	vPositionEyeSpace = (ModelView * vec4(vPosition, 1.0)).xyz;

	// Transformar a normal do vértice.
	vNormalEyeSpace = normalize(NormalMatrix * vColors);

	fragPos = vec3(Model * vec4(vPosition, 1.0f));

	textureCoord = vTextureCoords;
}
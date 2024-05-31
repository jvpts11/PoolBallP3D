/*
 * Discipline: 3D Programming
 * Subject: Pool Balls 3D Project
 * Authors: João Tavares nº 21871, Diogo Silva nº 22369, Ademar Valente nº 23155, José Lourenço nº23496
 * Date: 31/05/2024
 * File: poolballs.frag
 * Description: Fragment shader used on the project
 */

#version 440 core

uniform mat4 Model;
uniform mat4 View;
uniform mat4 ModelView;		
uniform mat3 NormalMatrix;
uniform int lightModel;
uniform int renderTex;
uniform sampler2D sampler;
uniform vec3 viewPosition;

// Ambient light structure
struct AmbientLight {
	vec3 ambient;	// Ambient light component
};

uniform AmbientLight ambientLight;	// Ambient light source

// Directional light structure
struct DirectionalLight	{
	vec3 direction;		// Light direction, in world space
	vec3 ambient;		// Ambient light component
	vec3 diffuse;		// Diffuse light component
	vec3 specular;		// Specular light component
};

uniform DirectionalLight directionalLight;	// Directional light source

// Point light structure
struct PointLight {
	vec3 position;		// Light position, in world space
	
	vec3 ambient;		// Ambient light component
	vec3 diffuse;		// Diffuse light component
	vec3 specular;		// Specular light component
	
	float constant;		// Constant attenuation
	float linear;		// Linear attenuation
	float quadratic;	// Quadratic attenuation
};

uniform PointLight pointLight;	// Point light source

// Spot light structure
struct SpotLight {
	vec3 position;		// Light position, in world space
	vec3 direction;		// Light direction, in world space

	vec3 ambient;		// Ambient light component
	vec3 diffuse;		// Diffuse light component
	vec3 specular;		// Specular light component
	
	float constant;		// Constant attenuation
	float linear;		// Linear attenuation
	float quadratic;	// Quadratic attenuation

	float cutoff, outerCutOff;	// Inner and outer cutoff angles, in degrees
};

uniform SpotLight spotLight;	// Spot light source

struct Material {
	vec3 ambient;		// ka
	vec3 diffuse;		// kd
	vec3 specular;		// ks
	float shininess;	// ns
};

uniform Material material;

layout(location = 0) in vec3 color;
layout(location = 1) in vec2 textureCoord;

layout(location = 2) in vec3 vPositionEyeSpace;
layout(location = 3) in vec3 vNormalEyeSpace;
layout(location = 4) in vec3 textureVector;
layout(location = 5) in vec3 fragPos;

layout (location = 0) out vec4 fColor; // Final color

vec4 calcAmbientLight(AmbientLight light);
vec4 calcDirectionalLight(DirectionalLight light);
vec4 calcPointLight(PointLight light);
vec4 calcSpotLight(SpotLight light);
vec4 calcNoLight();

void main() {
	vec4 lightToUse;

	if (lightModel == 1) { 		
		lightToUse = calcAmbientLight(ambientLight);
	} else if (lightModel == 2){
		lightToUse = calcDirectionalLight(directionalLight);
	} else if (lightModel == 3) {
		lightToUse = calcPointLight(pointLight);
	} else if (lightModel == 4) {
		lightToUse = calcSpotLight(spotLight);
	} else {
		lightToUse = calcNoLight();
	}

	if (renderTex == 1) {
		vec4 texColor = texture(sampler, textureCoord);
		fColor = (lightToUse) * texColor;
	} else  {
		fColor = (lightToUse) * vec4(color, 1.0f);
	}
}

vec4 calcAmbientLight(AmbientLight light) {
	// Ambient light contribution calculus for the object color.
	vec4 ambient = vec4(material.ambient * light.ambient, 1.0);
	return ambient;
}

vec4 calcDirectionalLight(DirectionalLight light) {
	// Ambient light reflexion calculus.
	vec4 ambient = vec4(material.ambient * light.ambient, 1.0);

	// Diffuse light reflexion calculus.
	vec3 lightDirectionEyeSpace = (View * vec4(light.direction, 0.0)).xyz;
	vec3 L = normalize(-lightDirectionEyeSpace);	// Direction opposite to the light direction.
	vec3 N = normalize(vNormalEyeSpace);
	float NdotL = max(dot(N, L), 0.0);
	vec4 diffuse = vec4(material.diffuse * light.diffuse, 1.0) * NdotL;
	
	// Specular light reflexion calculus.
	// As the calculations are being performed in eye coordinates, then the camera is at the position (0,0,0).
	// It then results in a vector V between the points (0,0,0) and vPositionEyeSpace:
	//		V = (0,0,0) - vPositionEyeSpace = (0-vPositionEyeSpace.x, 0-vPositionEyeSpace.y, 0-vPositionEyeSpace.z)
	// which can be simplified as:
	//		- vPositionEyeSpace	
	vec3 V = normalize(-vPositionEyeSpace);
	vec3 R = reflect(-L, N);
	float RdotV = max(dot(R, V), 0.0);
	vec4 specular = pow(RdotV, material.shininess) * vec4(light.specular * material.specular, 1.0);

	// Final contribution from direction light source to the final fragment color.
	return (ambient + diffuse + specular);
}

vec4 calcPointLight(PointLight light) {
	// Ambient light reflexion calculus.
	vec4 ambient = vec4(material.ambient * light.ambient, 1.0);

	// Diffuse light reflexion calculus.
	vec3 lightPositionEyeSpace = (View * vec4(light.position, 1.0)).xyz;
	vec3 L = normalize(lightPositionEyeSpace - vPositionEyeSpace);
	vec3 N = normalize(vNormalEyeSpace);
	float NdotL = max(dot(N, L), 0.0);
	vec4 diffuse = vec4(material.diffuse * light.diffuse, 1.0) * NdotL;

	// Specular light reflexion calculus.
	// As the calculations are being performed in eye coordinates, then the camera is at the position (0,0,0).
	// It then results in a vector V between the points (0,0,0) and vPositionEyeSpace:
	//		V = (0,0,0) - vPositionEyeSpace = (0-vPositionEyeSpace.x, 0-vPositionEyeSpace.y, 0-vPositionEyeSpace.z)
	// which can be simplified as:
	//		- vPositionEyeSpace	
	vec3 V = normalize(-vPositionEyeSpace);
	vec3 R = reflect(-L, N);
	float RdotV = max(dot(R, V), 0.0);
	vec4 specular = pow(RdotV, material.shininess) * vec4(light.specular * material.specular, 1.0);
	
	// Attenuation calculation
	float dist = length(mat3(View) * light.position - vPositionEyeSpace);	// Calculus of the distance between the light point and the vertex
	float attenuation = 1.0 / (light.constant + light.linear * dist + light.quadratic * (dist * dist));

	// Calculation of the contribution from the point light source to the final fragment color.
	return (attenuation * (ambient + diffuse + specular));
}

vec4 calcSpotLight(SpotLight light) {

    // Ambient light reflexion calculus.
    vec4 ambient =  vec4(material.ambient * light.ambient, 1.0);

    // Diffuse light reflexion calculus
    vec3 norm = normalize(color);
    vec3 lightPositionEyeSpace = (View * vec4(light.position, 1.0)).xyz;
    vec3 lightDir = normalize(lightPositionEyeSpace - vPositionEyeSpace);
    float diff = max(dot(norm, lightDir), 0.0);
    vec4 diffuse = vec4(light.diffuse * material.diffuse, 1.0) * diff;

	// Specular light reflexion calculus.    
	vec3 viewDir = normalize(viewPosition - fragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec4 specular = vec4(light.specular * spec, 1.0);

    float theta = dot(lightDir, normalize(-light.direction));


    // Spotlight (soft edges)
    float epsilon = (light.cutoff - light.outerCutOff);
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
    diffuse  *= intensity;
    specular *= intensity;

    // Attenuation calculus
    float distance    = length(mat3(View) * light.position - fragPos);
    float attenuation = 1.0f / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

	// Final contribution from the spot light source to the final fragment color.
    return (attenuation * (ambient + diffuse + specular));
}

vec4 calcNoLight() {
	// No light source, all color are black
	return vec4(0.0, 0.0, 0.0, 1.0);
}
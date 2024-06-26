/*
 * Discipline: 3D Programming
 * Subject: Pool Balls 3D Project
 * Authors: João Tavares nº 21871, Diogo Silva nº 22369, Ademar Valente nº 23155, José Lourenço nº23496
 * Date: 17/05/2024
 * File: poolballs.frag
 * Description: Fragment shader for the pool balls.
 */

#version 440 core

uniform mat4 Model;
uniform mat4 View;
uniform mat4 ModelView;		// View * Model
uniform mat3 NormalMatrix;
uniform int lightModel;
uniform sampler2D sampler;
uniform int renderTex;
uniform vec3 viewPosition;

// ambient light structure definition
struct AmbientLight {
	vec3 ambient;	// global ambient light component
};

uniform AmbientLight ambientLight;	// global ambient light source

// directional light structure definition
struct DirectionalLight	{
	vec3 direction;		// direction of the light, in world space
	
	vec3 ambient;		// ambient light component
	vec3 diffuse;		// diffuse light component
	vec3 specular;		// specular light component
};

uniform DirectionalLight directionalLight;	// directional light source

// point light structure definition
struct PointLight {
	vec3 position;		// point light position, in world space
	
	vec3 ambient;		// ambient light component
	vec3 diffuse;		// diffuse light component
	vec3 specular;		// specular light component
	
	float constant;		// constant attenuation coefficient
	float linear;		// linear attenuation coefficient
	float quadratic;	// quadratic attenuation coefficient
};

uniform PointLight pointLight;	// two point lights source

// spot light structure definition
struct SpotLight {
	vec3 position;		// spot light position, in world space
	vec3 direction;		// light direction, in world space

	vec3 ambient;		// ambient light component
	vec3 diffuse;		// diffuse light component
	vec3 specular;		// specular light component
	
	float constant;		// constant atenuation coefficient
	float linear;		// linear atenuation coefficient
	float quadratic;	// quadratic atenuation coefficient

	float cutoff, outerCutOff;
};

uniform SpotLight spotLight;	// spot light source

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

layout (location = 0) out vec4 fColor; // fragment final color

vec4 calcAmbientLight(AmbientLight light);
vec4 calcDirectionalLight(DirectionalLight light);
vec4 calcPointLight(PointLight light);
vec4 calcSpotLight(SpotLight light);

void main()
{

	vec4 lightToUse;

	if (lightModel == 2) {
		lightToUse = calcDirectionalLight(directionalLight);
	} else if (lightModel == 3) {
		lightToUse = calcPointLight(pointLight);
	} else if (lightModel == 4) {
		lightToUse = calcSpotLight(spotLight);
	}  else {
		lightToUse = calcAmbientLight(ambientLight);
	}

	if (renderTex == 1) {
		vec4 texColor = texture(sampler, textureCoord);
		fColor = (lightToUse) * texColor;
	} else  {
		fColor = (lightToUse) * vec4(color, 1.0f);
	}
}

vec4 calcAmbientLight(AmbientLight light) {
	// calculation of the global ambient light contribution to the final color of the fragment.
	vec4 ambient = vec4(material.ambient * light.ambient, 1.0);
	return ambient;
}

vec4 calcDirectionalLight(DirectionalLight light) {
	// calculation of the reflection of the ambient light component.
	vec4 ambient = vec4(material.ambient * light.ambient, 1.0);

	// calculation of the reflection of the diffuse light component.
	vec3 lightDirectionEyeSpace = (View * vec4(light.direction, 0.0)).xyz;
	vec3 L = normalize(-lightDirectionEyeSpace); // inverse of the light direction
	vec3 N = normalize(vNormalEyeSpace);
	float NdotL = max(dot(N, L), 0.0);
	vec4 diffuse = vec4(material.diffuse * light.diffuse, 1.0) * NdotL;
	
// Calculation of the reflection of the specular light component.
	// As the calculations are being performed in eye coordinates, then the camera is at position (0,0,0).
	// It then results in a vector V between the points (0,0,0) and vPositionEyeSpace:
	//		V = (0,0,0) - vPositionEyeSpace = (0-vPositionEyeSpace.x, 0-vPositionEyeSpace.y, 0-vPositionEyeSpace.z)
	// which can be simplified as:
	//		- vPositionEyeSpace

	vec3 V = normalize(-vPositionEyeSpace);

	//vec4 H = normalize(L + V);	// blinn-phong model
		vec3 R = reflect(-L, N);
	float RdotV = max(dot(R, V), 0.0);

	//float NdotH = max(dot(N, H), 0.0);	// blinn-phong model
	vec4 specular = pow(RdotV, material.shininess) * vec4(light.specular * material.specular, 1.0);

	//calculation of the contribution of the directional light source to the final color of the fragment.
	return (ambient + diffuse + specular);
}

vec4 calcPointLight(PointLight light) {
	// calculation of the reflection of the ambient light component.
	vec4 ambient = vec4(material.ambient * light.ambient, 1.0);

	// calculation of the reflection of the diffuse light component.
	//vec3 lightPositionEyeSpace = mat3(View) * light.position;
	vec3 lightPositionEyeSpace = (View * vec4(light.position, 1.0)).xyz;
	vec3 L = normalize(lightPositionEyeSpace - vPositionEyeSpace);
	vec3 N = normalize(vNormalEyeSpace);
	float NdotL = max(dot(N, L), 0.0);
	vec4 diffuse = vec4(material.diffuse * light.diffuse, 1.0) * NdotL;

	// calculation of the reflection of the specular light component.
	// as the calculations are being performed in eye coordinates, then the camera is at position (0,0,0).
	// so, the vector V between the points (0,0,0) and vPositionEyeSpace is:
	//		V = (0,0,0) - vPositionEyeSpace = (0-vPositionEyeSpace.x, 0-vPositionEyeSpace.y, 0-vPositionEyeSpace.z)
	// which can be simplified as:
	//		- vPositionEyeSpace
	vec3 V = normalize(-vPositionEyeSpace);
	//vec4 H = normalize(L + V);	// bliin-phong model
	vec3 R = reflect(-L, N);
	float RdotV = max(dot(R, V), 0.0);
	//float NdotH = max(dot(N, H), 0.0);	// blinn-phong model
	vec4 specular = pow(RdotV, material.shininess) * vec4(light.specular * material.specular, 1.0);
	
	// atenuation calculation
	float dist = length(mat3(View) * light.position - vPositionEyeSpace);	// distance between the light source and the fragment calculation
	float attenuation = 1.0 / (light.constant + light.linear * dist + light.quadratic * (dist * dist));

	// calculation of the contribution of the point light source to the final color of the fragment.
	return (attenuation * (ambient + diffuse + specular));
}

vec4 calcSpotLight(SpotLight light) {

	 // Ambient
    vec4 ambient =  vec4(material.ambient * light.ambient, 1.0);

    // Diffuse
    vec3 norm = normalize(color);
    vec3 lightPositionEyeSpace = (View * vec4(light.position, 1.0)).xyz;
    vec3 lightDir = normalize(lightPositionEyeSpace - vPositionEyeSpace);
    float diff = max(dot(norm, lightDir), 0.0);
    vec4 diffuse = vec4(light.diffuse * material.diffuse, 1.0) * diff;

    // Specular
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

    // Attenuation
    float distance    = length(mat3(View) * light.position - fragPos);
    float attenuation = 1.0f / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

    return (attenuation * (ambient + diffuse + specular));
	
}
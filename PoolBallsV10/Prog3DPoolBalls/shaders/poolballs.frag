#version 440 core

uniform mat4 Model;
uniform mat4 View;
uniform mat4 ModelView;		// View * Model
uniform mat3 NormalMatrix;
uniform int lightModel;
uniform sampler2D sampler;
uniform int renderTex;
uniform vec3 viewPosition;

// estrutura da fonte de luz ambiente global
struct AmbientLight {
	vec3 ambient;	// componente de luz ambiente global
};

uniform AmbientLight ambientLight;	// fonte de luz ambiente global

// estrutura de uma fonte de luz direcional
struct DirectionalLight	{
	vec3 direction;		// direção da luz, espaço do mundo
	
	vec3 ambient;		// componente de luz ambiente
	vec3 diffuse;		// componente de luz difusa
	vec3 specular;		// componente de luz especular
};

uniform DirectionalLight directionalLight;	// fonte de luz direcional

// estrutura de uma fonte de luz pontual
struct PointLight {
	vec3 position;		// posição do ponto de luz, espaço do mundo
	
	vec3 ambient;		// componente de luz ambiente
	vec3 diffuse;		// componente de luz difusa
	vec3 specular;		// componente de luz especular
	
	float constant;		// coeficiente de atenuação constante
	float linear;		// coeficiente de atenuação linear
	float quadratic;	// coeficiente de atenuação quadrática
};

uniform PointLight pointLight;	// duas fontes de luz pontual

// estrutura de uma fonte de luz cónica
struct SpotLight {
	vec3 position;		// posição do foco de luz, espaço do mundo
	vec3 direction;		// direção da luz, espaço do mundo

	vec3 ambient;		// componente de luz ambiente
	vec3 diffuse;		// componente de luz difusa
	vec3 specular;		// componente de luz especular
	
	float constant;		// coeficiente de atenuação constante
	float linear;		// coeficiente de atenuação linear
	float quadratic;	// coeficiente de atenuação quadrática

	float cutoff, outerCutOff;
};

uniform SpotLight spotLight;	// fonte de luz cónica

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

layout (location = 0) out vec4 fColor; // Cor final do fragmento

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
	// cálculo da contribuição da fonte de luz ambiente global, para a cor do objeto.
	vec4 ambient = vec4(material.ambient * light.ambient, 1.0);
	return ambient;
}

vec4 calcDirectionalLight(DirectionalLight light) {
	// cálculo da reflexão da componente da luz ambiente.
	vec4 ambient = vec4(material.ambient * light.ambient, 1.0);

	// cálculo da reflexão da componente da luz difusa.
	vec3 lightDirectionEyeSpace = (View * vec4(light.direction, 0.0)).xyz;
	vec3 L = normalize(-lightDirectionEyeSpace); // direção inversa à da direção luz.
	vec3 N = normalize(vNormalEyeSpace);
	float NdotL = max(dot(N, L), 0.0);
	vec4 diffuse = vec4(material.diffuse * light.diffuse, 1.0) * NdotL;
	
	// cálculo da reflexão da componente da luz especular.
	// como os cálculos estão a ser realizados nas coordenadas do olho, então a câmara está na posição (0,0,0).
	// resulta então um vetor V entre os pontos (0,0,0) e vPositionEyeSpace:
	//		V = (0,0,0) - vPositionEyeSpace = (0-vPositionEyeSpace.x, 0-vPositionEyeSpace.y, 0-vPositionEyeSpace.z)
	// que pode ser simplificado como:
	//		- vPositionEyeSpace
	vec3 V = normalize(-vPositionEyeSpace);
	//vec4 H = normalize(L + V);	// modelo blinn-phong
	vec3 R = reflect(-L, N);
	float RdotV = max(dot(R, V), 0.0);
	//float NdotH = max(dot(N, H), 0.0);	// modelo blinn-phong
	vec4 specular = pow(RdotV, material.shininess) * vec4(light.specular * material.specular, 1.0);

	// cálculo da contribuição da fonte de luz direcional para a cor final do fragmento.
	return (ambient + diffuse + specular);
}

vec4 calcPointLight(PointLight light) {
	// cálculo da reflexão da componente da luz ambiente.
	vec4 ambient = vec4(material.ambient * light.ambient, 1.0);

	// cálculo da reflexão da componente da luz difusa.
	//vec3 lightPositionEyeSpace = mat3(View) * light.position;
	vec3 lightPositionEyeSpace = (View * vec4(light.position, 1.0)).xyz;
	vec3 L = normalize(lightPositionEyeSpace - vPositionEyeSpace);
	vec3 N = normalize(vNormalEyeSpace);
	float NdotL = max(dot(N, L), 0.0);
	vec4 diffuse = vec4(material.diffuse * light.diffuse, 1.0) * NdotL;

	// cálculo da reflexão da componente da luz especular.
	// como os cálculos estão a ser realizados nas coordenadas do olho, então a câmara está na posição (0,0,0).
	// resulta então um vetor V entre os pontos (0,0,0) e vPositionEyeSpace:
	//		V = (0,0,0) - vPositionEyeSpace = (0-vPositionEyeSpace.x, 0-vPositionEyeSpace.y, 0-vPositionEyeSpace.z)
	// que pode ser simplificado como:
	//		- vPositionEyeSpace
	vec3 V = normalize(-vPositionEyeSpace);
	//vec4 H = normalize(L + V);	// Modelo Blinn-Phong
	vec3 R = reflect(-L, N);
	float RdotV = max(dot(R, V), 0.0);
	//float NdotH = max(dot(N, H), 0.0);	// Modelo Blinn-Phong
	vec4 specular = pow(RdotV, material.shininess) * vec4(light.specular * material.specular, 1.0);
	
	// atenuação
	float dist = length(mat3(View) * light.position - vPositionEyeSpace);	// cálculo da distância entre o ponto de luz e o vértice
	float attenuation = 1.0 / (light.constant + light.linear * dist + light.quadratic * (dist * dist));

	// cálculo da contribuição da fonte de luz pontual para a cor final do fragmento.
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
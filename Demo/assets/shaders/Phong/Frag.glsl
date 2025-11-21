#version 460 core

layout(location = 0) out vec4 colour;

in vec4 fragmentPosLightSpace;
in vec3 normal;
in vec3 fragmentPos;
in vec2 texCoord;

struct directionalLight
{
	vec3 colour;
	vec3 direction;
};

struct pointLight
{
	vec3 colour;
	vec3 position;
	vec3 constants;
};

struct spotLight
{
	vec3 colour;
	vec3 position;
	vec3 direction;
	vec3 constants;
	float cutOff;
	float outerCutOff;
};

const int numPointLights = 2;
const int numSpotLights = 2;

layout (std140, binding = 1) uniform b_lights
{
	uniform directionalLight dLight;
	uniform pointLight pLights[numPointLights];
	uniform spotLight sLights[numSpotLights];
};

layout (std140, binding = 0) uniform b_camera
{
	uniform mat4 u_view;
	uniform mat4 u_projection;
	uniform vec3 u_viewPos;
};


uniform vec3 u_albedo;
uniform sampler2D u_albedoMap;
uniform sampler2D u_shadowMap;

float shadowCalculation(vec4 fragPosLightSpace)
{
	// From on Learn OpenGL: https://learnopengl.com/Advanced-Lighting/Shadows/Shadow-Mapping
    // perform perspective divide
    vec3 projCoords = fragmentPosLightSpace.xyz / fragmentPosLightSpace.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(u_shadowMap, projCoords.xy).r; 
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
	// Calculate bias
	float bias = max(0.05 * (1.0 - dot(normal, dLight.direction)), 0.005); 
    // check whether current frag pos is in shadow
    float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;

    return shadow;
}

vec3 getDirectionalLight()
{
	float ambientStrength = 0.4;
	vec3 ambient = ambientStrength * dLight.colour;
	float diff = max(dot(normal, -dLight.direction), 0.0);
	vec3 diffuse = diff * dLight.colour;
	float specularStrength = 0.8;
	vec3 viewDir = normalize(u_viewPos - fragmentPos);
	vec3 reflectDir = reflect(dLight.direction, normal);  
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 64);
	vec3 specular = specularStrength * spec * dLight.colour;      
	return ambient * (diffuse + specular);
}

vec3 getPointLight(int idx)
{
	float ambientStrength = 0.4;
	vec3 ambient = ambientStrength * pLights[idx].colour;
	vec3 norm = normalize(normal);
	float distance = length(pLights[idx].position - fragmentPos);
	float attn = 1.0/(pLights[idx].constants.x + (pLights[idx].constants.y* distance) + (pLights[idx].constants.z * (distance * distance)));
	vec3 lightDir = normalize(pLights[idx].position - fragmentPos);
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = diff * attn * pLights[idx].colour;
	float specularStrength = 0.8;
	vec3 viewDir = normalize(u_viewPos - fragmentPos);
	vec3 reflectDir = reflect(-lightDir, norm);  
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 64);
	vec3 specular = specularStrength * spec * attn * pLights[idx].colour;  
	
	return (ambient + diffuse + specular);
}

vec3 getSpotLight(int idx)
{	
	vec3 lightDir = normalize(sLights[idx].position - fragmentPos);
	float theta = dot(lightDir, normalize(-sLights[idx].direction));
	float ambientStrength = 0.4;
	vec3 ambient = ambientStrength * sLights[idx].colour;
	if(theta > sLights[idx].outerCutOff)
	{
		vec3 norm = normalize(normal);
		float distance = length(sLights[idx].position - fragmentPos);
		float attn = 1.0/(sLights[idx].constants.x + (sLights[idx].constants.y* distance) + (sLights[idx].constants.z * (distance * distance)));
		float diff = max(dot(norm, lightDir), 0.0);
		vec3 diffuse = diff * attn * sLights[idx].colour;
		float specularStrength = 0.8;
		vec3 viewDir = normalize(u_viewPos - fragmentPos);
		vec3 reflectDir = reflect(-lightDir, norm);  
		float spec = pow(max(dot(viewDir, reflectDir), 0.0), 64);
		vec3 specular = specularStrength * spec * attn * sLights[idx].colour;  
	
		float epsilon = sLights[idx].cutOff - sLights[idx].outerCutOff;
		float intensity = clamp((theta - sLights[idx].outerCutOff) / epsilon, 0.0, 1.0); 
	
		diffuse  *= intensity;
		specular *= intensity;
		
		return (ambient + diffuse + specular);
	}
	else
	{
		return ambient;
	}
}


void main()
{
	vec3 result = vec3(0.0, 0.0, 0.0); 
	
	result += getDirectionalLight();
	
	for(int i = 0; i <numPointLights; i++)
	{
		//result += getPointLight(i);
	}
	
	for(int i = 0; i <numSpotLights; i++)
	{
		//result += getSpotLight(i);
	}
	      
	colour = vec4(result * u_albedo, 1.0) * texture(u_albedoMap, texCoord);
}

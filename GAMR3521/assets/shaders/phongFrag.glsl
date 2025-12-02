#version 460 core

layout(location = 0) out vec4 colour;

in vec4 fragmentPosLightSpace;
//in vec3 normal;
in vec3 fragmentPos;
in vec2 texCoord;
in mat3 tangentToWorld;
in vec4 clipSpaceCoords;
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

const int numPointLights = 7;
const int numSpotLights = 1;


uniform sampler2D u_prePassDepthTexture;


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
uniform sampler2D u_specularMap;
uniform sampler2D u_normalMap;

// forward declare
vec3 getDirectionalLight() ;
vec3 getPointLight(int idx) ;
vec3 getSpotLight(int idx) ;
float specularStrength = vec3(texture(u_specularMap,texCoord)).r;
vec3 normalFromMap = texture(u_normalMap, texCoord).rgb;
vec3 normal = normalize(tangentToWorld * (normalFromMap * 2.0 - 1.0));

bool hasPassedDepthTest();

void main()
{

    if(hasPassedDepthTest() == false) return;

	vec3 result = vec3(0.0, 0.0, 0.0); 
	
	result += getDirectionalLight();
	
	for(int i = 0; i <numPointLights; i++)
	{
		result += getPointLight(i);
	}
	
	for(int i = 0; i <numSpotLights; i++)
	{
		//result += getSpotLight(i);
	}
	      

        vec3 albedoColour = texture(u_albedoMap,texCoord).rgb; 
        albedoColour = pow(albedoColour,vec3(2.2)); 
        
	colour = vec4(result * albedoColour, 1.0);
}


vec3 getDirectionalLight()
{
	float ambientStrength = 0.4;
	vec3 ambient = ambientStrength * dLight.colour;
	float diff = max(dot(normal, -dLight.direction), 0.0);
	vec3 diffuse = diff * dLight.colour;
	vec3 viewDir = normalize(u_viewPos - fragmentPos);
	vec3 reflectDir = reflect(dLight.direction, normal);  
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 64);
	vec3 specular = specularStrength * spec * dLight.colour;      
	return ambient + (diffuse + specular);
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



bool hasPassedDepthTest()
{

  // manual perspective divide to normalize the z value we will use 
  // as our depth value
  float fragClipSpaceZ = clipSpaceCoords.z / clipSpaceCoords.w;

  // convert to 0-1 range as depth buffer values are in range 0-1
  float fragClipSpaceNormalisedZ = fragClipSpaceZ * 0.5 + 0.5;
  
  // we then get texture coordinates by extracting the x and y from our clip space 
  // frag coord and performing a manul perspective divide to convert them to ndc  
  // then mapping them to 0-1 range so they can be used as coordinates to sample from our pre pass 
  // depth texture to check if the depth value we have in the clip space frag coord will pass the depth test
  vec2 TCForExtractingDepthValue = (clipSpaceCoords.xy / clipSpaceCoords.w) * 0.5 + 0.5; 

  float extractedDepthValue = texture(u_prePassDepthTexture,TCForExtractingDepthValue).r;
  float bias = 0.001;
  if(fragClipSpaceNormalisedZ >= extractedDepthValue + bias) return false;

  return true;

}
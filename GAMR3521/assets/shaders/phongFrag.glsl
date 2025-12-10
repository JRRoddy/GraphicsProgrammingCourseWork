#version 460 core

layout(location = 0) out vec4 colour;




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

const int numPointLights = 7;
const int numSpotLights = 1;




uniform sampler2D u_prePassDepthTexture;
uniform sampler2D u_shadowMap;

uniform sampler2D u_skyBoxColBuffer;
uniform int u_shadowSampleRadius;
uniform int u_antiAliasingOn;

uniform mat4 u_lightSpaceMatrix;







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


uniform float u_farClip;
uniform float u_nearClip; 



uniform sampler2D u_normalMap;
uniform sampler2D u_diffSpecMap;
uniform sampler2D u_fragmentPositions;
// forward declare
vec3 getDirectionalLight();
vec3 getPointLight(int idx);
vec3 getSpotLight(int idx);
float lineariseDepth(float zDepth);

// values sampled from the deffered rendering pass 
vec3 fragmentPos = texture(u_fragmentPositions,texCoord).rgb;

vec4 fragmentPosLightSpace  = u_lightSpaceMatrix * vec4(fragmentPos,1.0);  

float specularStrength = texture(u_diffSpecMap,texCoord).a;

vec3 normal = texture(u_normalMap, texCoord).rgb;

float shadowContribution();

void main()
{


   float curDepth = texture(u_prePassDepthTexture,texCoord).r;
   
   float linearisedDepth = (lineariseDepth(curDepth) - u_nearClip) / (u_farClip - u_nearClip);

   if(linearisedDepth >= 0.9999) 
   {
      vec3 skyBoxCol = texture(u_skyBoxColBuffer,texCoord).rgb;

	  colour = vec4(skyBoxCol,1.0);
	  return;
   
   }

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
	      

    vec3 albedoColour = texture(u_diffSpecMap,texCoord).rgb; 
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
	
	float shadowAmount = shadowContribution();

	return ambient + (1.0 - shadowAmount)  *  (diffuse + specular);
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





float shadowContribution()
{
  


   vec3 lightSpacePerspectiveDivide = (fragmentPosLightSpace.xyz) / fragmentPosLightSpace.w;

   float lightSpaceDepthRemap = lightSpacePerspectiveDivide.z * 0.5 + 0.5;

   vec2 shadowMapDepthCoords = lightSpacePerspectiveDivide.xy * 0.5 + 0.5; 
   // because we took the vertex coords through the light space transform
   // i.e through the light space projection matrix and view matrix 
   // we can use the ndc xy we get from the perspective divide to directly 
   // calculate the texture coordinates to access the correct depth value 
   // that we need to compare with
   float actualLightSpaceDepth = texture(u_shadowMap,shadowMapDepthCoords).r;

   float shadow = 0.0f;

   // make sure regions outside the frustrum are not in shadow 
   // save performance
   if(lightSpaceDepthRemap > 1.0)
   {
     return 0.0;
   }

   // use a bias here to handle when a particualr edge is on the border of 
   // a pixel in the shadow map helps prevent shadow acene by slightly increasing 
   // the precision required to be in shadow 
   float bias  = 0.015;
   vec2 texelSize = 1.0 / textureSize(u_shadowMap,0);
   float shadowCount = 0.0f;
   float total = 0.0f;
   for(int x = -u_shadowSampleRadius; x <= u_shadowSampleRadius;x++)
   {
        
    for(int y = -u_shadowSampleRadius; y <= u_shadowSampleRadius;y++)
    {
          total += 1.0f;




		  vec2 textureOffsetCoords = shadowMapDepthCoords + vec2(x,y) * texelSize;
          
		  textureOffsetCoords = clamp(textureOffsetCoords,vec2(0.0),vec2(1.0));


		  float shadowSampleDepth = texture(u_shadowMap,textureOffsetCoords).r;

		  if(lightSpaceDepthRemap -  bias > shadowSampleDepth) shadowCount += 1.0f;

    }
   
   
   }




      // compare our calculated  lightspace depth from the main cameras
   // perspective with the depth we got for the fragment when rendering 
   // from the lights perspective and if the fragment was further away 
   // in terms of depth when rendering from the main cameras perspective 
   // than from the lights perspective then their was an object in the way 
   // for the light when trying to reach the fragment within the main cameras 
   // perspective so the depth value for the pixel rendered from the lights perspective 
   // is smaller as it is closer due to their being an object in the way from the lights 
   // perspective 
   if(lightSpaceDepthRemap - bias > actualLightSpaceDepth  ) shadow = 1.0f;
   
   
   float aliasingOn = float(u_antiAliasingOn);
   float shadowContrib = (shadowCount / total) * aliasingOn + shadow * (1.0 - aliasingOn) ;
   return shadowContrib;



}


float lineariseDepth(float zDepth)
{
   
   float depthClip = zDepth * 2.0 -1.0; 



   float linearisedDepth = (2.0 * u_nearClip * u_farClip) / ( u_nearClip + u_farClip - depthClip * (u_farClip - u_nearClip)); 

   return linearisedDepth;
}



  
#version 460 core

layout(location = 0) out vec4 colour;


#define PI 3.1415926538
#define MAXREFLECTIONLOD 4.0
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




uniform  samplerCube u_irradianceMap;
uniform sampler2D u_prePassDepthTexture;
uniform sampler2D u_shadowMap;
uniform sampler2D u_fragmentId;

uniform sampler2D u_skyBoxColBuffer;
uniform float u_ambientFactor;
uniform float u_metallic;
uniform float u_roughness;

int u_shadowSampleRadius  = 1 ;
uniform int u_antiAliasingOn;

uniform mat4 u_lightSpaceMatrix;
uniform int u_usePointLight;
uniform int u_useDirLight;
uniform vec3 u_albedo;



uniform samplerCube u_prefilterMap;
uniform sampler2D u_BDRFLookup;



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






uniform sampler2D u_normalMap;
uniform sampler2D u_diffSpecMap;
uniform sampler2D u_fragmentPositions;

// forward declare
vec3 getDirectionalLight();
vec3 getPointLight(int idx,vec3 f0);
vec3 getSpotLight(int idx);
//float lineariseDepth(float zDepth);
vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness);
// values sampled from the deffered rendering pass 
vec4 fragmentCol = texture(u_fragmentPositions,texCoord);

vec3 fragmentPos = fragmentCol.rgb;
float metallic = fragmentCol.a ;
uniform int u_PBRDebugEnabled;

vec4 fragmentPosLightSpace  = u_lightSpaceMatrix * vec4(fragmentPos,1.0);  

float baseReflectivity = texture(u_diffSpecMap,texCoord).a;

vec4 normalCol = texture(u_normalMap, texCoord);
vec3 normal = normalCol.rgb;
float roughness = normalCol.a;


vec3 albedoColour = texture(u_diffSpecMap,texCoord).rgb; 


vec3 irradiance = texture(u_irradianceMap,normal).rgb;
vec3 aAlbedo = irradiance * albedoColour;
float GGX(float NdotH);
float shadowContribution();

vec3 viewDir = normalize(u_viewPos - fragmentPos);
vec3 calculateFresnelSheen(float cosTheta, vec3 f0);
vec3 getDirectionalLight(vec3 f0);
float GeometrySmith(float NDotL,float NDotV);
float GGXSchlickBeckman(float NDot);
void main()
{
   
   float curDepth = texture(u_prePassDepthTexture,texCoord).r;


   if(curDepth >= 0.9999) 
   {
      vec3 skyBoxCol = texture(u_skyBoxColBuffer,texCoord).rgb;

	  colour = vec4(skyBoxCol,1.0);
	  return;
   
   }
   
   if(u_PBRDebugEnabled > 0)
   {
      metallic = u_metallic;
	  roughness = u_roughness;
	  albedoColour = u_albedo;
   
   }

   
	vec3 result = vec3(0.0, 0.0, 0.0); 
	
	// f0 is a property that is used to help calculate fresnel 
	// determining how much of the objects diffuse is used byb interpolating between a base reflectivty value 
	// and the objects diffuse colour based on metalness as if an object has no metalness at all 
	// then the reflective colour of the object will be very shallow compared to an object 
	// of high metalness i.e based on metalness of object does it have a perfect relfection or does it have a really
	// dull relfection when it comes to fresnel(sheen)
	vec3 f0 = mix(vec3(baseReflectivity),albedoColour,metallic);

	result += getDirectionalLight(f0) * float(u_useDirLight);
	
	for(int i = 0; i < numPointLights; i++)
	{
		//result += getPointLight(i,f0) * float(u_usePointLight);
	}
	
	for(int i = 0; i <numSpotLights; i++)
	{
		//result += getSpotLight(i);
	}
	      
    
        
    colour = vec4(result, 1.0); 
	
	


}





vec3 getDirectionalLight(vec3 f0)
{
	
    vec3 reflection = reflect(-viewDir,normal);
    vec3 prefilterColour = textureLod(u_prefilterMap, reflection, 1.0).rgb;

    vec3 aks = fresnelSchlickRoughness(max(dot(normal, viewDir), 0.0), f0, roughness);  
    vec2 envBDRF = texture(u_BDRFLookup,vec2(max(dot(normal,viewDir),0.0),roughness)).xy;   
	//vec2 envBDRF = texture(u_BDRFLookup,vec2(0.0,0.0)).xy; 
	vec3 aSpecular = prefilterColour * (aks * envBDRF.x + envBDRF.y);



	vec3 negateDir = normalize(-dLight.direction);

	vec3 halfwayVector = normalize(viewDir + negateDir);

	// calculate dot product between normal and light dir 
	// usually used for diffuse scalar
	float normalDotL= max(dot(normal,negateDir),0.0001);
	// calcuate dot product between normal and halfway vector 
	// will be used to determine specular highlights via fresnel 
	float normalDotH = max(dot(normal,halfwayVector),0.0001);
    float NdotV = max(dot(normal,viewDir),0.0001);

	vec3 fresnelEnhancedReflecivity = calculateFresnelSheen(normalDotH,f0);


	vec3 specualrWeight = fresnelEnhancedReflecivity;

	// for the purpose of energy conservation we dont allow the diffuse or 
	// specualr term to go over 1.0 enerusing that the properties of the material are balanced 
	// reducing things such as unatural glowing/brightness as diffuse is reduced as specualr increases
	vec3 diffuseWeight = vec3(1.0) - specualrWeight;

	//multiply the diffuse term by 1.0 - metallic as metals do not have a diffuse component
	diffuseWeight *= (1.0 - metallic);
	// calculate final diffuse colour contirbution multiplying it by the dot product between the normal and the light direction
	// division by pi ensures that energy conservation is adhered to as light is diffused equally in all directions 
	// for every unit of light hiting the surfa ethe same amount is reflected back over the heimsphere 
	// ensuring that the amount of light reflected back is equal in terms of diffuse making the colour more realistic

	vec3 diffuseContrib = (diffuseWeight * albedoColour / PI) * normalDotL;

	// normal distruction(D term) essentially descirbes the number of imcro facets that are aligned with the 
	// halfway vector calculated for the surface(commonly used to dtermine the strenghtr of specualr highlights)
	// which will determine how the specualr relfection appears harsh and concentrated(lots of alignement) or
	// less concentrated and weaker(small amount of alignement) GGX is the function we will use
	float alignmentWithMicroFacets = GGX(normalDotH);


	// G term is what is used to calcuate ovbershadowing laogn the surface based onm how many  mciro facets(small imperfections) 
	// lbock the light from reaching a certain part of the surface itself this accounts for light masked from reaching the viewer 
	// or where light would be blocked from reaching a particualr point on the surface 

	float G = GeometrySmith(normalDotL,NdotV);
	
	

	// we calculate the specualr of the equation through the use of cookTorrance which 
	// makes used ot the descirbed normal distribution and the fresnel effect 

	vec3 numerator = alignmentWithMicroFacets * G * fresnelEnhancedReflecivity;
	// n dot v takes into account directio of fragement to camera 
	float denom = 4.0 * NdotV * normalDotL + 0.0001;

	vec3 specularContrib = numerator / denom;



	// ambient lighiting with ibl 


	vec3 akd = vec3(1.0) - aks;

	akd *= (1.0 - metallic);


	
	 
	vec3 ambient = aAlbedo * akd + aSpecular ;


	float isInShadow = shadowContribution();
	return (ambient + ((1.0 - isInShadow) * (diffuseContrib + specularContrib))) * dLight.colour;





}

float GGX(float NdotH)
{
    float a  = roughness * roughness;
	float a2 = a * a;
    float sqrDot = NdotH * NdotH;

	float denom = (sqrDot * (a2 - 1.0) + 1.0);
	denom = PI * denom * denom;

    return a2 / denom;



}


vec3 calculateFresnelSheen(float cosTheta, vec3 f0)
{
    // adding to strenght of the releftivness colour based on the angle between the halfway vector and the normal
    return f0 + (1.0 - f0) * pow(clamp(1.0 - cosTheta,0.0,1.0),5.0);

}

float GeometrySmith(float NDotL,float NDotV)
{
   

   float ggx1 = GGXSchlickBeckman(NDotV);
   float ggx2 = GGXSchlickBeckman(NDotL);


   return ggx1 * ggx2;



}

// as indirect ambient lighting is coming from all directions across the hemishpehere orinated around the normal 
// there is no single halfway vector that can be used to simulate the fresnel response therefore we use the 
// view direction dot with the normal as cosTheta instead while also taking into account roughness to dampen the 
// reflective ratio of the surface ensuring that relfections from  indirect light follow the same rules as directional light 
vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness)
{
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

float GGXSchlickBeckman(float NDot)
{
    float roughnessSqr = (roughness * roughness + 1.0);

	float k = (roughnessSqr * roughnessSqr) / 8.0;

	float denom = NDot * (1.0 - k) + k;

	return NDot / denom;

}


vec3 getPointLight(int idx,vec3 f0)
{

    vec3 dir = normalize((pLights[idx].position - fragmentPos));
	
	float distance = distance(pLights[idx].position,fragmentPos);
    float attn = 1.0/(pLights[idx].constants.x + (pLights[idx].constants.y* distance) + (pLights[idx].constants.z * (distance * distance)));

	vec3 halfwayVector = normalize(viewDir + dir);

	// calculate dot product between normal and light dir 
	// usually used for diffuse scalar
	float normalDotL= max(dot(normal,dir),0.0001);
	// calcuate dot product between normal and halfway vector 
	// will be used to determine specular highlights via fresnel 
	float normalDotH = max(dot(normal,halfwayVector),0.0001);
    float NdotV = max(dot(normal,viewDir),0.0001);

	vec3 fresnelEnhancedReflecivity = calculateFresnelSheen(normalDotH,f0);


	vec3 specualrWeight = fresnelEnhancedReflecivity;

	// for the purpose of energy conservation we dont allow the diffuse or 
	// specualr term to go over 1.0 enerusing that the properties of the material are balanced 
	// reducing things such as unatural glowing/brightness as diffuse is reduced as specualr increases
	vec3 diffuseWeight = vec3(1.0) - specualrWeight;

	//multiply the diffuse term by 1.0 - metallic as metals do not have a diffuse component
	diffuseWeight *= (1.0 - metallic);
	// calculate final diffuse colour contirbution multiplying it by the dot product between the normal and the light direction
	// division by pi ensures that energy conservation is adhered to as light is diffused equally in all directions 
	// for every unit of light hiting the surfa ethe same amount is reflected back over the heimsphere 
	// ensuring that the amount of light reflected back is equal in terms of diffuse making the colour more realistic
	vec3 diffuseContrib = ((diffuseWeight * albedoColour / PI) ) * normalDotL;

	// normal distruction(D term) essentially descirbes the number of imcro facets that are aligned with the 
	// halfway vector calculated for the surface(commonly used to dtermine the strenghtr of specualr highlights)
	// which will determine how the specualr relfection appears harsh and concentrated(lots of alignement) or
	// less concentrated and weaker(small amount of alignement) GGX is the function we will use
	float alignmentWithMicroFacets = GGX(normalDotH);


	// G term is what is used to calcuate ovbershadowing laogn the surface based onm how many  mciro facets(small imperfections) 
	// lbock the light from reaching a certain part of the surface itself this accounts for light masked from reaching the viewer 
	// or where light would be blocked from reaching a particualr point on the surface 

	float G = GeometrySmith(normalDotL,NdotV);
	
	

	// we calculate the specualr of the equation through the use of cookTorrance which 
	// makes used ot the descirbed normal distribution and the fresnel effect 

	vec3 numerator = alignmentWithMicroFacets * G * fresnelEnhancedReflecivity;
	
	float denom = 4.0 * NdotV * normalDotL + 0.0001;

	vec3 specularContrib = (numerator / denom);


	vec3 ambient = vec3(1.0) * u_ambientFactor * albedoColour;
	
	
	return (ambient + ((diffuseContrib + specularContrib) * attn)) * pLights[idx].colour;


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


//float lineariseDepth(float zDepth)
//{
   
 //  float depthClip = zDepth * 2.0 -1.0; 



  // float linearisedDepth = (2.0 * u_nearClip * u_farClip) / ( u_nearClip + u_farClip - depthClip * (u_farClip - u_nearClip)); 

  // return linearisedDepth;
//}




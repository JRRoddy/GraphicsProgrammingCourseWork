#version 460 core 

out vec4 colour;
layout(location = 0) out vec4 g_position;
layout(location = 1) out vec4 g_normal;
layout(location = 2) out vec4 g_diffSpec;

uniform sampler2D u_particleTexture;


in vec2 texCoord;
in vec3 fragPos;
in vec3 normal;


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



float maxDist = 1.1;
in float particleAge;
in vec3 particleOrigin;
in vec3 particlePos;
vec4 particleCol = texture(u_particleTexture,texCoord);

vec3 dirLight();

void main()
{
  
  
 
  

 if(particleCol.a <= 0.8)
 {
     discard;
   
 } 
 float dist =  distance(particlePos,particleOrigin);
 float alpha = smoothstep(0.0,maxDist, dist);
 vec3 end  = vec3(0.8,0.0,0.0);
 vec3 start  = vec3(1.0,1.0,0.0);
 vec3 particleColour = mix(start,end,alpha);

 //particleColour *= alpha;
  
 
  

 g_position = vec4(fragPos,1.0);
 g_normal = vec4(normal,1.0);
 g_diffSpec = vec4(particleColour,alpha);  

// vec3 lightColour = particleColour.rgb * dirLight();

// colour = vec4(lightColour, alpha);

 
 


}




vec3 dirLight()
{ 
    
	 vec3 dirLightInv = normalize(-dLight.direction);

	 float ambientScalar = 0.4;
	 vec3 ambient = dLight.colour * ambientScalar;
	 
	 float diffuseScalar  = max(dot(normal,dirLightInv),0.0);
	 vec3 diffuse = dLight.colour * diffuseScalar; 

	 float specularStrenght  = 1.0; 
	 
	 vec3 toCam = normalize(u_viewPos - fragPos);

	 vec3 refelct = reflect(dLight.direction,normal);

	 float specualrScalar = pow(max(dot(toCam,refelct),0.0),64.0);
	 vec3 specualr = dLight.colour * specualrScalar * specularStrenght;

	 return ambient + diffuse + specualr;

   
   

    
 
 


}
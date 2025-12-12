
# version 460 core 

layout (location = 0) out vec4 g_position; 
layout (location = 1) out vec4 g_normal;
layout (location = 2) out vec4 g_diffSpec;


in vec3 normal;
in vec3 fragmentPos;
in vec2 texCoord;



uniform sampler2D u_albedoMap;
uniform vec3 u_albedo;


void main()
{
   
  float specular = 0.8; 
  
  g_position = vec4(fragmentPos,1.0);
  g_normal = vec4(normal,1.0);
  vec3 diffuse = texture(u_albedoMap,texCoord).rgb;
  g_diffSpec = vec4(u_albedo*diffuse,specular);
  
  
     


}
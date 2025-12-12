# version 460 core 

layout(location = 0) out vec4 g_position;
layout(location = 1) out vec4 g_normal;
layout(location = 2) out vec4 g_difSpec;



in vec3 fragmentPos;
in vec3 normal;



void main()
{

  g_position = vec4(fragmentPos,1.0);
  g_normal = vec4(normal,1.0);
  g_difSpec = vec4(0.0,1.0,0.0,1.0);
  
}

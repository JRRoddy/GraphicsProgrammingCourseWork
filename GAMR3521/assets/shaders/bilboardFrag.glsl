#version 460

layout (location = 0) out vec4 g_position; 
layout (location = 1) out vec4 g_normal;
layout (location = 2) out vec4 g_diffSpec; 




in vec2 texCoord;
in vec3 fragmentPosition;
in vec3 normal;
uniform sampler2D u_bilBoardTexture;




void main()
{
  vec4 diff = texture(u_bilBoardTexture,texCoord);

  if(diff.a <= 0.0)
  {
    discard;
  }
  
  g_position = vec4(fragmentPosition,0.0);
  g_normal = vec4(normal,0.0);


  
  

  g_diffSpec = vec4(diff.rgb,0.0);

}





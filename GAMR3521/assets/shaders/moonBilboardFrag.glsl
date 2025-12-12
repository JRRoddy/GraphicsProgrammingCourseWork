#version 460 core 


layout(location = 0) out vec4 g_position;
layout(location = 1) out vec4 g_normal;
layout(location = 2) out vec4 g_difSpec;
layout(location = 3) out vec4 g_id;


in vec2 texCoord;
in vec3 fragmentPosition;
in vec3 normal;
uniform sampler2D u_bilBoardMoonTexture;





void main()
{
     
     g_id = vec4(1.0,0.0,0.0,0.0);
     vec4 diff  = texture(u_bilBoardMoonTexture,texCoord);
   
     if(diff.a <= 0.0) discard;
   
     g_position = vec4(fragmentPosition,1.0);
     g_normal = vec4(normal,1.0);

        
      
     g_difSpec = vec4(diff.rgb,0.0);
    

     
     



  
   
   

}
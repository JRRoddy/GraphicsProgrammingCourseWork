#version 460 core


out vec4 colour;




in vec2 texCoord;

uniform sampler2D u_deferredDepth;
uniform sampler2D u_deferredCol;
uniform sampler2D u_forwardDepth;
uniform sampler2D u_forwardCol;

void main()
{

   float deferredDepth = texture(u_deferredDepth,texCoord).r; 
   float forwardDepth = texture(u_forwardDepth,texCoord).r; 
   vec4 forwardCol = texture(u_forwardCol,texCoord);
   vec4 deferredCol = texture(u_deferredCol,texCoord);

   //if(forwardDepth < deferredDepth)
   //{
     //   colour = texture(u_forwardCol,texCoord);  
    //    return;
   //}

    colour = texture(u_deferredCol,texCoord);  

  




}



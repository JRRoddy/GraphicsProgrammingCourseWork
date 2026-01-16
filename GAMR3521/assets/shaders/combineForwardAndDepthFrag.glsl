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
   float deferredCol = texture(u_deferredCol,texCoord).r; 
   float forwardCol = texture(u_forwardCol,texCoord).r; 
  // if(forwardDepth < deferredDepth)
  // {
  //      colour = texture(u_forwardCol,texCoord);  
      //  return;
  // }

    colour = vec4(1.0,0.0,0.0,1.0);  

  




}



#version 460 core 

layout(location = 0) out vec4 linDepthCol;

uniform sampler2D u_depthBufferTexture;

in vec2 screenTexCoords;


uniform float u_nearClip;
uniform float u_farClip; 



float lineariseDepth(float depth);

void main()
{

   float depth =  texture(u_depthBufferTexture,screenTexCoords).r;
   float linearisedDepth = lineariseDepth(depth) / u_farClip;
  
    
   vec3 linDepthColour = vec3(linearisedDepth);    
    
  
   linDepthCol = vec4(linDepthColour,1.0);

  

}

float lineariseDepth(float depth)
{
   float zDepthNdc =  depth * 2.0f - 1.0;
    
   float lineariseDepthValue = (2.0 * u_nearClip * u_farClip) / (u_nearClip + u_farClip  - zDepthNdc * (u_farClip  - u_nearClip));
 
   return lineariseDepthValue;
}
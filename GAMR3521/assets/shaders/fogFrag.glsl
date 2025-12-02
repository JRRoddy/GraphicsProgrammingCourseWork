# version 460

layout(location = 0) out vec4 fogColour; 



uniform int u_active; 
uniform sampler2D u_colourBufferTexture;
uniform sampler2D u_depthTexture;
uniform float u_farClip; 
uniform float u_nearClip;
uniform vec3 u_fogColour;
in vec2 screenTexCoords;


float lineariseDepth(float depth);

void main()
{
  

    float isActive = float(u_active); 
     
    vec3 colourBufferTextureSample = texture(u_colourBufferTexture,screenTexCoords).rgb;
     
    float zDepth = texture(u_depthTexture , screenTexCoords).r;   
    
    float linDepth = lineariseDepth(zDepth); 
    
    float mappedDepth = (linDepth - u_nearClip) / (u_farClip - u_nearClip); 

    vec3 finalFogColour = mix(colourBufferTextureSample,u_fogColour,mappedDepth);
    
    fogColour =  vec4(finalFogColour,1.0) * isActive + vec4(colourBufferTextureSample,1.0) * (1.0 - isActive);

   


}



float lineariseDepth(float depth)
{

  
   float zDepthNDC =  depth * 2.0 - 1.0;
                           
   return  ( 2.0 * u_nearClip * u_farClip) / ( u_farClip + u_nearClip - zDepthNDC * (u_farClip - u_nearClip));

}
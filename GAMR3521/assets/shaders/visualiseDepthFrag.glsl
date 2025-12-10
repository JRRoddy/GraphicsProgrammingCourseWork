#version 460

layout(location = 0 ) out vec4 linearDepthColour; 

in vec2 screenTexCoords;


uniform sampler2D u_depthBufferTexture;

uniform float u_nearClip; 
uniform float u_farClip;




float lineariseDepth(float depth);

void main()
{
  
    
  
    float depthSample = texture(u_depthBufferTexture, screenTexCoords).r; 
    // linearise the depth value(z value) we get from performing projection and perspective divide 
    float depthLin = lineariseDepth(depthSample);
    // once we have linearised the depth it will be in the range near - far with far being the max so to convert it bakc into a percentatge we can use for the  colour we divide the result of linearisation by far
    float depthNormalised = depthLin / u_farClip;
    vec3 finalCol = vec3(depthNormalised);
    
    vec3 reinhart = finalCol / (finalCol + vec3(1.0));
    
    vec3 gammaCorrect = pow(finalCol, vec3(1.0/2.2)); 
   
    gammaCorrect = clamp(gammaCorrect,0.0,1.0); 
     
    linearDepthColour = vec4(finalCol,1.0);
     
 
    
 



}   



float lineariseDepth(float depth)
{
   
   float depthInClipSpaceZ = depth * 2.0 - 1.0;
   
    
   float lineariseDepthCalc = ( 2.0 * u_nearClip * u_farClip) / ( u_farClip + u_nearClip - depthInClipSpaceZ * (u_farClip - u_nearClip));
  
   return lineariseDepthCalc;

}







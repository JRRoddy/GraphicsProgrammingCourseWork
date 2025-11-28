# version 460 

layout(location = 0) out vec4 blurColour;


in vec2 screenTexCoords; 

uniform sampler2D u_colourBufferTexture;

uniform int u_active;

uniform int u_blurRadius;

uniform vec2 u_imageSize;
void main()
{  
   vec3 colourBufferTextureSample  = texture(u_colourBufferTexture,screenTexCoords).rgb;
   float sampleCount = 0.0;
   vec4 colourSum = vec4(0.0);
   
   for(int x = -u_blurRadius; x <= u_blurRadius; ++x)
   {
      for(int y  =  -u_blurRadius; y <= u_blurRadius; ++y)
      {
      
         vec2 uv = screenTexCoords + vec2(x,y) / u_imageSize; 

         uv = clamp(uv,vec2(0.0),vec2(1.0));

         colourSum += texture(u_colourBufferTexture,uv);
         sampleCount += 1.0;

      }
    }



   float isActive  = float(u_active);
  
   blurColour = (colourSum / sampleCount) * isActive +  vec4(colourBufferTextureSample,1.0) * (1.0 - isActive);

}
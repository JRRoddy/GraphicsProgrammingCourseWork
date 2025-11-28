# version 460
layout(location = 0) out vec4 ppColour;


in vec2 screenTexCoords;

uniform sampler2D u_colourBufferTexture;

void main()
{

   vec3 ColourBufferTextureSample = texture(u_colourBufferTexture,screenTexCoords).rgb;
   ppColour = vec4(ColourBufferTextureSample,1.0);
  

}
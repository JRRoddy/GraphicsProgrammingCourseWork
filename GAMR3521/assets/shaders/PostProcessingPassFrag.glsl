layout(location = 0) out vec4 ppColour;


in vec2 screenTexCoords;

uniform sampler2D colourBufferTexture;

void main()
{

  
   vec3 ColourBufferTextureSample = texture(colourBufferTexture,screenTextCoords).rgb;
   ppColour = vec4(ColourBufferTextureSample,1.0);
  

}
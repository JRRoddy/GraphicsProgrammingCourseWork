# version 460
layout(location = 0) out vec4 invertedColour;



in vec2 screenTexCoords;


uniform sampler2D u_colourBufferTexture; 

uniform int u_active;

void main()
{

   vec3 colourBufferTextureSample = texture(u_colourBufferTexture,screenTexCoords).rgb;
   float isActive = float(u_active);

   invertedColour =  vec4(1.0 - colourBufferTextureSample,1.0) * isActive + vec4(colourBufferTextureSample,1.0) * (1.0 - isActive);


}
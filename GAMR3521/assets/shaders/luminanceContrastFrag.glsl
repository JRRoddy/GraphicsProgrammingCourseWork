# version 460

layout (location = 0) out vec4 contrastColour;



in vec2 screenTexCoords;

uniform sampler2D u_colourBufferTexture;

uniform float u_contrast;

uniform int u_active;


void main()
{

  vec3 colourBufferTextureSample = texture(u_colourBufferTexture,screenTexCoords).rgb;
  vec3 relLumScalars = vec3(0.299,0.587,0.114);
  float relLum = dot(colourBufferTextureSample,relLumScalars);
  // we use a base grey to get the distance from grey in terms of our brightness and cauclate the midpoint between the two 
  vec3 grey = vec3(0.5,0.5,0.5);
  vec3 vecLum = vec3(relLum,relLum,relLum);
  vec3 midPoint = (grey + vecLum) / 2.0;
  // calculate the distance from grey interms of brightness using how large the midpoint was(higher range interms of midpoint means more impact )
  vec3 contrastCol = clamp((colourBufferTextureSample - midPoint) * u_contrast + midPoint, 0.0, 1.0);
  
  float isActive = float(u_active);
  contrastColour = vec4(contrastCol,1.0) * isActive + vec4(colourBufferTextureSample,1.0) * (1.0 - isActive); 
  
}

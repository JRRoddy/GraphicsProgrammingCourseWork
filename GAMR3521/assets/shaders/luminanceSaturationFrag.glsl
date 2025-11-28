# version 460
layout(location = 0) out vec4 lumContrastColour;




in vec2 screenTexCoords;

uniform float u_saturation; 

uniform sampler2D u_colourBufferTexture;

uniform int u_active;

void main()
{

 vec3 relLumScalars = vec3(0.299,0.587,0.114);
 vec3 colourBufferTextureSample = texture(u_colourBufferTexture, screenTexCoords).rgb ;
 float relLum = dot(colourBufferTextureSample,relLumScalars); 
   
 
 float isActive = float(u_active);

 // uses the brightness obtained from relativel luminance(an average of how bright the colour is based on how the human eye would percieve it )
 // as a vec3 which gives us a grey scale colour to be assigned to this pixel and mixed with the orginal colour depending on the saturation value
 // giving us a grey scale image based on brightness at a saturation level of 0 and the default colour at a saturation of 1.0
 lumContrastColour =  vec4(mix(vec3(relLum,relLum,relLum),colourBufferTextureSample,u_saturation),1.0) * isActive
                 + vec4(colourBufferTextureSample,1.0) * (1.0 - isActive);
   

}
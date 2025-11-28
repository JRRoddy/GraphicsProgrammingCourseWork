# version 460
layout(location = 0) out vec4 gColour;


in vec2 screenTexCoords;

uniform sampler2D u_colourBufferTexture;

void main()
{

    vec3 colourBufferTextureSample = texture(u_colourBufferTexture,screenTexCoords).rgb;
    
    colourBufferTextureSample = colourBufferTextureSample/(colourBufferTextureSample + vec3(1.0));
    
    colourBufferTextureSample = pow(colourBufferTextureSample , vec3(1.0/2.2));   
    colourBufferTextureSample = clamp(colourBufferTextureSample,0.0,1.0);
   
    gColour = vec4(colourBufferTextureSample,1.0);

}






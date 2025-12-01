# version 460

layout(location = 0) out vec4 relLColour;


in vec2 screenTexCoords;

uniform sampler2D u_colourBufferTexture;
uniform int u_active;
uniform vec3 u_tint;


void main()
{
     vec3 colourBufferTextureSample = texture(u_colourBufferTexture, screenTexCoords).rgb ;
     vec3 relLumScalars = vec3(0.299,0.587,0.114);
     vec3 tinted =  u_tint;
     float relativeLuminance = clamp(dot(colourBufferTextureSample, relLumScalars),0.0,1.0);
     float isActive = float(u_active);
    // based on the brightness of the fragment colour we lerp from the orginal colour and the tint colour 
    // brightness ensures that the tint doesnt completly wash out the original image/ isnt too harsh 
     relLColour = vec4(mix(colourBufferTextureSample,tinted,relativeLuminance),1.0) * isActive 
                 + vec4(colourBufferTextureSample,1.0) * (1.0 - isActive);
      
    

}


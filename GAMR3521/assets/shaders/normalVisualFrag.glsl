# version 460 core 

layout(location = 0) out vec4 normalColour;



uniform sampler2D u_lightPassDepth;

in vec4 fragPosClip;


in vec3 normal;


in vec3 fragmentPos[];

void main()
{

  vec3 perspectiveDivide = fragPosClip.xyz / fragPosClip.w;

  vec3 remap = perspectiveDivide * 0.5 + 0.5; 
  float currentDepth = texture(u_lightPassDepth,remap.xy).r;
  if(remap.z < currentDepth)
  {
    normalColour = vec4(normal,1.0);
    return;
  }


  normalColour = vec4(0.0);


}
# version 460 core 

layout(location = 0) out vec4 normalOverlayCol;

uniform sampler2D u_normalSceneCol;

uniform sampler2D u_sceneCol;

in vec2 texCoord;


uniform int u_active;
void main()
{

  float isActive = float(u_active);
  
  vec4 sceneCol = texture(u_sceneCol,texCoord);
  vec4 sampleNormalCol = texture(u_normalSceneCol,texCoord);
  vec3 gammaNegate = pow(sampleNormalCol.rgb,vec3(2.2));
  vec4 normalCol =  vec4(gammaNegate,sampleNormalCol.a);
  vec4 difference = sceneCol - normalCol;
  float hasNormalCol =  float((difference == sceneCol));

  normalCol += (sceneCol * hasNormalCol);
  normalOverlayCol = normalCol * isActive + sceneCol * (1.0 - isActive);
  

}
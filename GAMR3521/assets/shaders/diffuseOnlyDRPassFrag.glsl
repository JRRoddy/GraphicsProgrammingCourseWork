
# version 460 core 

layout (location = 0) out vec4 g_position; 
layout (location = 1) out vec4 g_normal;
layout (location = 2) out vec4 g_diffSpec;


in vec3 normal;
in vec3 fragmentPos;
in vec2 texCoord;
in mat3 TBN;

uniform float u_heightScalar;
uniform float u_terrainHeightOffset;

uniform sampler2D u_albedoMap;
uniform sampler2D u_secondaryAlbedoMap;
uniform sampler2D u_secondNormalMap;
uniform vec3 u_albedo;
uniform int u_heightColActive;
uniform sampler2D u_normalMap; 

void main()
{
   


  vec3 normalNdc = texture(u_normalMap,texCoord).rgb * 2.0 - 1.0;


  vec3 normalNDC1 = texture(u_secondNormalMap,texCoord).rgb * 2.0 - 1.0;

  vec3 blendNormal = normalize(vec3(normalNdc.xy + normalNDC1.xy, normalNdc.z));



  vec3 fragNormal =  normal;

  float specular = 0.5; 

  float scale = 10.0f;
  // get the percent cotribution of each axis direction along the surface 
  // by taking the absolute of the normal and normalising 
  vec3 blendPercent = normalize(abs(fragNormal));
  // get sum of contribution
  float b = blendPercent.x + blendPercent.y + blendPercent.z;
  // get avrg contribution for each axis 
  vec3 avrgContrib = blendPercent / vec3(b);
  // sample the colour along the texture within each plane 
  // x,y, and z giving us the colour contribution for each axis direction
  vec4 xPlaneColour = texture2D(u_albedoMap,fragmentPos.yz * 0.1 );
  vec4 yPlaneColour = texture2D(u_albedoMap,fragmentPos.xz * 0.1 );
  vec4 zPlaneColour = texture2D(u_albedoMap,fragmentPos.xy * 0.1);
  // blend the colours using the avrg contribution along each axis 
  vec4 triBlendTexColour = xPlaneColour * avrgContrib.x  + yPlaneColour * avrgContrib.y + zPlaneColour * avrgContrib.z;
  vec4 xPlaneColour1 = texture2D(u_secondaryAlbedoMap,fragmentPos.yz * 0.1 );
  vec4 yPlaneColour1 = texture2D(u_secondaryAlbedoMap,fragmentPos.xz * 0.1);
  vec4 zPlaneColour1 = texture2D(u_secondaryAlbedoMap,fragmentPos.xy * 0.1 );
  // blend the colours using the avrg contribution along each axis 
  vec4 triBlendTexColour1 = xPlaneColour1 * avrgContrib.x + yPlaneColour1 * avrgContrib.y + zPlaneColour1 * avrgContrib.z;


  

  g_position = vec4(fragmentPos,1.0);
  g_normal = vec4(blendNormal,1.0);
  vec3 diffuse = texture(u_albedoMap,texCoord).rgb;

  // calcuate surface colour based on height
  float mid = u_heightScalar * 0.5;

  
  float offsetNegate = abs(u_terrainHeightOffset) *  ( (float((u_terrainHeightOffset < 0.0))) * 2.0 - 1.0);
  vec3 heightColour = mix(vec3(triBlendTexColour),vec3(triBlendTexColour1),smoothstep(0.0,mid + mid * 0.5, fragmentPos.y  + offsetNegate));

  vec3 finalColour =  heightColour * u_heightColActive + u_albedo *( 1.0 - u_heightColActive);
  g_diffSpec = vec4(heightColour ,specular);
  
  
     


}
#version 460 core
layout(binding = 0, rgba16f) uniform image2D outputImage;


uniform vec2 u_heightMapSize;
uniform sampler2D u_heightMap;
uniform float u_heightScalar;



void main()
{

   ivec2 pixelCoords = ivec2(gl_GlobalInvocationID.xy);
   vec2 uv = pixelCoords / u_heightMapSize;


   float right = (textureOffset(u_heightMap,uv,ivec2(-1,0)).r) * u_heightScalar;
   float left = (textureOffset(u_heightMap,uv,ivec2(1,0)).r) * u_heightScalar;
   float up = (textureOffset(u_heightMap,uv,ivec2(0,-1)).r) * u_heightScalar;
   float down = (textureOffset(u_heightMap,uv,ivec2(0,1)).r) * u_heightScalar;

   float lr = left - right;
   float ud = up - down;
   vec3 normal = normalize(vec3(lr,2.0,ud));
   vec4 normalCol = vec4(normal,1.0);

   vec4 pixelColour = vec4(1.0,0.0,0.0,1.0);
   


  
   
  
}
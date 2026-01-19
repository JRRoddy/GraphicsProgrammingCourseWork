#version 460 core 
layout(local_size_x = 16, local_size_y = 16) in;
layout(binding = 0, rgba16f) uniform image2D outputImg;


uniform sampler2D u_heightMap;

uniform float u_heightScalar;



void main()
{
   ivec2 pixelCoords = ivec2(gl_GlobalInvocationID.xy);  
   vec2 heightMapSize = textureSize(u_heightMap,0);

   vec2 uv = vec2(pixelCoords) / heightMapSize;
   float height = texture(u_heightMap,uv).r;

   float right = (textureOffset(u_heightMap, uv, ivec2(1,0)).r);
   float left = (textureOffset(u_heightMap, uv, ivec2(-1,0)).r);
   float up = (textureOffset(u_heightMap, uv, ivec2(0,-1)).r);  
   float down = (textureOffset(u_heightMap, uv, ivec2(0,1)).r); 

   
   right *= u_heightScalar;
   left  *= u_heightScalar;
   up *= u_heightScalar;
   down *= u_heightScalar;
  
   float lr  = left - right;
   float ud = up - down; 
   

   vec3 normal = normalize(vec3(lr,2.0,ud));
   vec4 pixelColour = vec4(normal,height) ;
   
   imageStore(outputImg,pixelCoords,pixelColour);


}


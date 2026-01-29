#version 460 core 



layout(local_size_x = 32, local_size_y = 32, local_size_z = 1) in;


layout(binding = 0,rgba16f) uniform imageCube outputCubeMap;


#define TwoPI 6.28318530718
#define PI 3.1415926538 
vec3 getSampleCoord();

uniform sampler2D u_envMap;

vec2 imageSize = vec2(1024,1024);


void main()
{
  
    
     vec3 sampleCoord = getSampleCoord();
      
     float phi = atan(sampleCoord.z,sampleCoord.x);
     float theta = acos(sampleCoord.y);

     vec4 colour = texture(u_envMap,vec2(phi/TwoPI,-theta/PI));
     
     imageStore(outputCubeMap,ivec3(gl_GlobalInvocationID),colour);

}


vec3 getSampleCoord()
{
   
    vec2 st = vec2(gl_GlobalInvocationID.xy) / imageSize;

    vec2 uv  = 2.0 * vec2(st.x, 1.0 - st.y) - vec2(1.0);

    vec3 sv = vec3(0.0);


    if(gl_GlobalInvocationID.z == 0)  sv = vec3(1.0,uv.y,-uv.x);

    else if(gl_GlobalInvocationID.z == 1)  sv = vec3(-1.0,uv.y,uv.x);

    else if(gl_GlobalInvocationID.z == 2)  sv = vec3(uv.x,1.0,-uv.y);

    else if(gl_GlobalInvocationID.z == 3)  sv = vec3(uv.x,-1.0,uv.y);
    
    else if(gl_GlobalInvocationID.z == 4)  sv = vec3(uv.x,uv.y,1.0);
   
    else if(gl_GlobalInvocationID.z == 5)  sv = vec3(-uv.x,uv.y,-1.0);


   return normalize(sv);

}

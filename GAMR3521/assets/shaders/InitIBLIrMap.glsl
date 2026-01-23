# version 460 core 



#define TwoPI 6.28318530718
#define PIHALF 3.1415926538/2.0 
#define PI 3.1415926538 
layout(local_size_x = 32, local_size_y = 32, local_size_z = 1) in;
layout(binding = 0,rgba16f) uniform imageCube outputIrCubeMap;




uniform samplerCube u_envCubeMap;
vec2 imageSize = vec2(1024,1024);
vec3 getSampleCoord();
void main()
{
    
   float sampleInc = 0.025;
   float totalSamples = 0.0;
   vec4 irradiance = vec4(0.0);
   vec3 up = vec3(0.0,1.0,0.0);
   vec3 normal = normalize(getSampleCoord() * vec3(gl_GlobalInvocationID));

   vec3 right =  normalize(cross(up,normal));
   up = normalize(cross(normal, right));


   for(float phi  = 0.0f; phi < TwoPI; phi += sampleInc)
   {
      for(float theta  = 0.0f; theta < PIHALF; theta += sampleInc)
      {
               
           vec3 tangentSample = vec3(sin(theta) * cos(phi), sin(theta) * sin(phi),cos(theta));
           vec3 worldSample = tangentSample.x * right + tangentSample.y * up + tangentSample.z * normal;

           irradiance += texture(u_envCubeMap,) * sin(phi) * cos(phi);
            

           totalSamples += 1.0;
    

      } 


   
   }
    
   
   irradiance = PI * irradiance * (1.0/totalSamples); 
    
 
   imageStore(outputIrCubeMap,ivec3(gl_GlobalInvocationID),irradiance);

  
   
    
  



}




vec3 getSampleCoord()
{
   
    vec2 st = vec2(gl_GlobalInvocationID.xy) / imageSize;

    vec2 uv  = 2.0 * vec2(st.x, st.y) - vec2(1.0);

    
   
    vec3 sv = vec3(0.0);



    if(gl_GlobalInvocationID.z == 0)  sv = vec3(1.0,uv.y,-uv.x);

    else if(gl_GlobalInvocationID.z == 1)  sv = vec3(-1.0,uv.y,uv.x);

    else if(gl_GlobalInvocationID.z == 2)  sv = vec3(uv.x,1.0,-uv.y);

    else if(gl_GlobalInvocationID.z == 3)  sv = vec3(uv.x,-1.0,uv.y);
    
    else if(gl_GlobalInvocationID.z == 4)  sv = vec3(uv.x,uv.y,1.0);
   
    else if(gl_GlobalInvocationID.z == 5)  sv = vec3(-uv.x,uv.y,-1.0);





   return normalize(sv);

}


# version 460 core 



#define TwoPI 6.28318530718
#define PIHALF 3.1415926538/2.0 
#define PI 3.1415926538 
layout(local_size_x = 32, local_size_y = 32, local_size_z = 1) in;

layout(binding = 0,rgba16f) uniform imageCube outputIrCubeMap;

vec2 imageSize = vec2(1024.0,1024.0);

vec3 getSampleCoord();

uniform samplerCube u_envCubeMap;



vec3 normal = getSampleCoord();
vec3 up = vec3(0.0,1.0,0.0);

vec3 right = normalize(cross(up,normal));






void main()
{
    up = normalize(cross(right, normal));

    float sampleCount  = 0.0;
    float sampleInc = 0.05;
    vec3 irradiance  = vec3(0.0);
   // for(float phi = 0.0; phi < TwoPI; phi += sampleInc)
   // {
   //    for(float theta = 0.0; theta < PI * 0.5; theta += sampleInc)
   //    {
   //       vec3 tangent = vec3(sin(theta) * cos(phi), sin(theta) * sin(phi), cos(theta));
   //
   //       vec3 worldSample = right * tangent.x + up * tangent.y + normal * tangent.z; 
   //
   //       irradiance += texture(u_envMap,worldSample).rgb * cos(theta) * sin(theta);
   //       sampleInc++;
   //    
   //    }
   // 
   // }


   irradiance = (irradiance * PI) * (1.0/float(sampleInc));

   vec4 irrCol = vec4(irradiance,1.0);
    
   vec4 cubeMapMipped = textureLod(u_envCubeMap,getSampleCoord(),10.0);
   
   imageStore(outputIrCubeMap,ivec3(gl_GlobalInvocationID),cubeMapMipped);
   

}




vec3 getSampleCoord()
{
   
    vec2 st = vec2(gl_GlobalInvocationID.xy) / imageSize;

    vec2 uv  = 2.0 * vec2(st.x, 1.0 - st.y) - vec2(1.0);

    
   
    vec3 sv = vec3(0.0);




    if(gl_GlobalInvocationID.z == 0)
    {
        sv = vec3(1.0,uv.y,-uv.x);
       // faceCentre =  vec3(1.0,0.0,0.0);


    }  
    

    else if(gl_GlobalInvocationID.z == 1) 
    { 
        sv = vec3(-1.0,uv.y,uv.x);
       // faceCentre =  vec3(-1.0,0.0,0.0);


    }

    else if(gl_GlobalInvocationID.z == 2)
    {
        sv = vec3(uv.x,1.0,-uv.y);
       // faceCentre =  vec3(0.0,1.0,0.0);
       // up = vec3(0.0,0.0,1.0);

    }

    else if(gl_GlobalInvocationID.z == 3)  
    {
        sv = vec3(uv.x,-1.0,uv.y);
       // faceCentre =  vec3(0.0,-1.0,0.0);
       // up = vec3(0.0,0.0,1.0);

    }
    
    else if(gl_GlobalInvocationID.z == 4) 
    { 
        sv = vec3(uv.x,uv.y,1.0);
       // faceCentre =  vec3(0.0,0.0,1.0);


    }
   
    else if(gl_GlobalInvocationID.z == 5)  
    { 
        
        sv = vec3(-uv.x,uv.y,-1.0);
        //faceCentre =  vec3(0.0,0.0,-1.0);


    }





   return normalize(sv);

}


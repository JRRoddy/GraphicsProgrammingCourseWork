#version 460 core
layout(local_size_x = 4, local_size_y = 4, local_size_z = 1) in;

layout(binding  =  0, rgba16f) uniform imageCube outputPrefilterMap;

/// this shader pre computes the specularity porition of the indirect lighting using various functions 
// to approximate it such as monte carlo integration and importance sampling

uniform float u_roughness;
uniform samplerCube u_envMap;


#define PI 3.14159265359
 
float DistributionGGX(vec3 N, vec3 H, float roughness);
vec3 getSampleCoord();
vec3 ImportanceSampleGGX(vec2 Xi, vec3 N, float roughness);
float RadicalInverse_VdC(uint bits);
vec2 Hammersley(uint i, uint N);

vec2 mipSize = vec2(imageSize(outputPrefilterMap));

void main()
{
    vec2 pixelCoords = vec2(gl_GlobalInvocationID.xy);
    
   
    
    
    
     vec3 N = getSampleCoord();
     vec3 R = N;
     vec3 V  = R;
       
     const uint SAMPLE_COUNT = 1024u;
     float totalWeight = 0.0;
     vec3 prefilteredColour = vec3(0.0);
     for(uint i = 0u; i < SAMPLE_COUNT; ++i)
     {
        vec2 xi =  Hammersley(i, SAMPLE_COUNT);
        vec3 H = ImportanceSampleGGX(xi, N, u_roughness);
        vec3 L = normalize(2.0 * dot(V,H) * H - V);       
        
        float NdotL = max(dot(N,L),0.0);
        
        if(NdotL > 0.0)
        {
  
  
 
          // this is done to help avoid any large bright spots that come from high frequency detail
          // and varying intensities in specualr reflections 
          float D = DistributionGGX(N, H, u_roughness);
          float NdotH = max(dot(N,H),0.0);
          float HdotV = max(dot(H,V),0.0);

          float pdf = D * NdotH / ((4.0 * HdotV) + 0.0001);

          float resolution  = 1024.0;
          float saTexel  = 4.0 * PI / (6.0 * resolution * resolution);
          float saSample = 1.0 / (float(SAMPLE_COUNT) * pdf + 0.0001);
          float mipLevel = u_roughness == 0.0 ? 0.0 : 0.5 * log2(saSample / saTexel);

          prefilteredColour += textureLod(u_envMap, L, mipLevel).rgb * NdotL;
          totalWeight += NdotL; 
         
        }
  
     }     
     
     prefilteredColour  = prefilteredColour / (totalWeight + 0.0001);
     
     
  

     
     imageStore(outputPrefilterMap,ivec3(gl_GlobalInvocationID),vec4(prefilteredColour,1.0));
    

}







vec3 getSampleCoord()
{
   
    vec2 st = vec2(gl_GlobalInvocationID.xy) / mipSize;

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









float RadicalInverse_VdC(uint bits) 
{
    bits = (bits << 16u) | (bits >> 16u);
    bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
    bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
    bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
    bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
    return float(bits) * 2.3283064365386963e-10; // / 0x100000000
}

vec2 Hammersley(uint i, uint N)
{
    return vec2(float(i)/float(N), RadicalInverse_VdC(i));
}  


vec3 ImportanceSampleGGX(vec2 Xi, vec3 N, float roughness)
{
    float a = roughness*roughness;
	
    float phi = 2.0 * PI * Xi.x;
    float cosTheta = sqrt((1.0 - Xi.y) / (1.0 + (a*a - 1.0) * Xi.y));
    float sinTheta = sqrt(1.0 - cosTheta*cosTheta);
	
    // from spherical coordinates to cartesian coordinates
    vec3 H;
    H.x = cos(phi) * sinTheta;
    H.y = sin(phi) * sinTheta;
    H.z = cosTheta;
	
    // from tangent-space vector to world-space sample vector
    vec3 up        = abs(N.z) < 0.999 ? vec3(0.0, 0.0, 1.0) : vec3(1.0, 0.0, 0.0);
    vec3 tangent   = normalize(cross(up, N));
    vec3 bitangent = cross(N, tangent);
	
    vec3 sampleVec = tangent * H.x + bitangent * H.y + N * H.z;
    return normalize(sampleVec);
} 

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness*roughness;
    float a2 = a*a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;

    float nom   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom / denom;
}

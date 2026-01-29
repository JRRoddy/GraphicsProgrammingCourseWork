# version 460 core 

layout(local_size_x = 16, local_size_y = 16) in;

#define PI 3.1415926538




uniform vec3 u_particleOrigin;
float maxFlameWidth = 0.5;
float minFlameWidth = -0.5;
float spawnLocationWidth = 5.0;
float spawnLocationLength = 5.0;

float maxAge = 5.0;
float minAge = 1.0;
float maxAccel = 0.8;
float minAccel = 0.5;
struct particle
{ 
   vec4 origin;
   vec4 position;
   vec4 velocity;
  
};

layout(std430, binding = 0) buffer particlesBuffer
{

    particle particles[];

};


float calcAge();
float calcAccel();
vec4 calcVelocity();
vec3 randomDirection(vec2 seed);
vec3 randomDirectionXY(vec2 seed);

vec3 randomSpawnLocation(vec2 seed);

float rand(vec2 seed);
vec2 randSeed = vec2(gl_GlobalInvocationID.xy);
void main()
{
  uint gridWidth = 32;
  vec2 res = vec2(gridWidth*16);

  ivec2 gridCoords = ivec2(gl_GlobalInvocationID.xy);
  uint gridId = uint(gridCoords.x) * gridWidth + uint(gridCoords.y);

  float angle =  rand(randSeed) * (2*PI);
  particles[gridId].position.w = maxAge;
  vec3 vel = randomDirectionXY(randSeed);
   
  particles[gridId].origin.xyz = randomSpawnLocation(randSeed);
  particles[gridId].position.xyz = randomSpawnLocation(randSeed);
  particles[gridId].origin.w = particles[gridId].position.w;
  particles[gridId].velocity = vec4(vel,calcAccel());
  

}


vec4 calcVelocity()
{ 

   return vec4(randomDirection(randSeed),1.0);
  
}


float calcAge()
{
  
   return minAge + (maxAge - minAge) * rand(randSeed*2.0);

}


float calcAccel()
{
  
   return minAccel + (maxAccel - minAccel) * rand(randSeed*3.0);

}

float rand(vec2 seed)
{
    
    return fract(sin(dot(seed.xy,
                         vec2(10.9898,90.233)))*
        43758.5453123);
}


vec3 randomDirection(vec2 seed) {

    float theta = rand(seed) * 2.0 * 3.14159;
    float phi = acos(2.0 * rand(seed * 2.0) - 1.0);
    float x = sin(phi) * cos(theta);
    float y = sin(phi) * sin(theta);
    float z = cos(phi);
    return vec3(x, y, z);
}

vec3 randomDirectionXY(vec2 seed) {
    
   
    float x = rand(seed);
    float y = -1.0 ;
    float z = rand(seed);
    return vec3(x, y, z);
}


vec3 randomSpawnLocation(vec2 seed)
{
   vec3 spawnLocation = vec3(0.0);
    
   spawnLocation.x  = u_particleOrigin.x + spawnLocationWidth * (rand(seed * 8.0) * 2.0 -1.0);  
   spawnLocation.y  = u_particleOrigin.y;
   spawnLocation.z =  u_particleOrigin.z + spawnLocationLength * (rand(seed * 2.0) * 2.0 -1.0);  
   return spawnLocation;
}

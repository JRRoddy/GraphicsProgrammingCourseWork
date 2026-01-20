#version 460 core 



layout(local_size_x = 16, local_size_y = 16) in;


struct particle
{
  vec4 origin;
  vec4 position;
  vec4 velocity;


};
float maxRadius  = 3.0f;
uniform float dt;
uniform float u_particleAcceleration;
float maxAge = 2.0;
layout(std430,binding = 0) buffer particlesBuffer
{

    particle particles[];

};


void updateParticle();
void main()
{
 

  updateParticle();

   
}


void updateParticle()
{
   ivec2 gridCoords = ivec2(gl_GlobalInvocationID.xy);
   uint gridWidth = 32;
   uint id = uint(gridCoords.x) * gridWidth + uint(gridCoords.y);
   float accel = particles[id].velocity.w;
   particles[id].position.xyz += particles[id].velocity.xyz *(accel *dt);

   particles[id].position.w -= (dt*0.8) ;
   if(particles[id].position.w <= 0.0)
   {
       
       particles[id].position.w = particles[id].origin.w;

       particles[id].position.xyz = particles[id].origin.xyz;
   }
}


#version 460 core 



layout(local_size_x = 16, local_size_y = 16) in;


struct particle
{
  vec4 origin;
  vec4 position;
  vec4 velocity;


};

uniform float dt;
uniform float u_particleAcceleration;

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
   particles[id].position.xyz += particles[id].velocity.xyz *  (u_particleAcceleration* dt) ;
   //+ 0.5 * u_particleAcceleration * dt * dt; 
   //particles[id].velocity += u_particleAcceleration * dt;
   
   particles[id].position.w -= dt ;
   if(particles[id].position.w <= 0.0)
   {
       
       particles[id].position.xyz = particles[id].origin.xyz;
       particles[id].position.w = 5.0;
      
   }
}


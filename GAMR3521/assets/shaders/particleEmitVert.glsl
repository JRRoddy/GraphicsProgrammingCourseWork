#version 460 core 

struct particle
{
  vec4 origin;
  vec4 position;
  vec4 velocity;

};

layout(std430,binding = 0) buffer particlesBuffer
{
   
  particle particles[];
  

};

out float vs_particleAge;
out vec3 vs_particleOrigin;
void main()
{
  vs_particleAge = particles[gl_VertexID].position.w;
  vs_particleOrigin = particles[gl_VertexID].origin.xyz;
  gl_Position = vec4(particles[gl_VertexID].position.xyz,1.0); 




}

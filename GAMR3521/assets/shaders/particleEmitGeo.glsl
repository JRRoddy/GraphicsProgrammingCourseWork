# version 460 core 


layout(points) in;

layout(triangle_strip, max_vertices  = 4) out;



layout (std140, binding = 0) uniform b_camera
{
	uniform mat4 u_view;
	uniform mat4 u_projection;
	uniform vec3 u_viewPos;
};


uniform float u_particleScale;

out vec3 fragPos;
out vec2 texCoord;
out vec3 normal;
out float particleAge;

in float vs_particleAge[];

void main()
{

   particleAge = vs_particleAge[0];
   vec3 up  = vec3(0,1,0);
   vec3 particlePos = gl_in[0].gl_Position.xyz;
   vec3 forward = normalize(u_viewPos - particlePos); 
   vec3 right = normalize(cross(forward,up)) * u_particleScale;
   up *= u_particleScale;
    
   normal  = forward;
   fragPos = particlePos - right - up;
   texCoord = vec2(0.0,0.0);
   gl_Position = u_projection * u_view * vec4(fragPos,1.0);
   EmitVertex(); 
  
   fragPos = particlePos + right - up;
   texCoord = vec2(1.0,0.0);
   gl_Position = u_projection * u_view * vec4(fragPos,1.0);
   EmitVertex();
   

   fragPos = particlePos - right  + up;
   texCoord = vec2(0.0,1.0);
   gl_Position = u_projection * u_view * vec4(fragPos,1.0);
   EmitVertex();

   fragPos = particlePos + right + up;
   texCoord = vec2(1.0,1.0);
   gl_Position = u_projection * u_view * vec4(fragPos,1.0);
   EmitVertex();
  
   EndPrimitive();
}














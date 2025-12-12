# version 460 core



layout(triangles) in;

layout(points, max_vertices = 1) out;




in vec3 vs_fragmentPos[];

layout(std140,binding = 0) uniform b_camera
{
  uniform mat4 u_view;
  uniform mat4 u_projection;
  uniform vec3 u_viewPos;
 
 
};

out vec3 normal;
out vec3 fragmentPos;

void main()
{
    
   vec3 v0 = vs_fragmentPos[1] - vs_fragmentPos[0];     
   vec3 v1 = vs_fragmentPos[2] - vs_fragmentPos[0];     

   
   normal = normalize(cross(v0,v1));

   // find average/center point of primitve triangle coming in  
   vec3 positionSum =  vs_fragmentPos[0] +  vs_fragmentPos[1] +  vs_fragmentPos[2];
   vec3 averagePos = positionSum / 3.0; 

   fragmentPos = averagePos;
   gl_Position =  u_projection * u_view  *  vec4(averagePos,1.0);
    
   EmitVertex();
   EndPrimitive();
  
     

  
 

}
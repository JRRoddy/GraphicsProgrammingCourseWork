# version 460 core





layout(triangles) in;


layout(line_strip, max_vertices = 2) out;





layout (std140, binding = 0) uniform b_camera
{
	uniform mat4 u_view;
	uniform mat4 u_projection;
	uniform vec3 u_viewPos;
};

in vec3 vs_fragmentPos[];

uniform float u_normalLength;


out vec3 normal;
out vec4 fragPosClip;
void main()
{
  
 

     vec3 normalStart = vs_fragmentPos[0];
     fragPosClip = u_projection * u_view * vec4(normalStart,1.0);
     
     gl_Position = u_projection * u_view * vec4(normalStart,1.0);

     EmitVertex();
      
     vec3 v0 =  vs_fragmentPos[1] - vs_fragmentPos[0];
     vec3 v1 =  vs_fragmentPos[2] - vs_fragmentPos[0];
      
     vec3 normalCalc = normalize(cross(v0,v1));
    
     normal = normalCalc;
     
     vec3 endPos = normalStart + normalCalc * u_normalLength;
   
     fragPosClip = u_projection * u_view * vec4(endPos,1.0);;
     gl_Position =  u_projection * u_view * vec4(endPos,1.0);
  
     EmitVertex();
      
    
     EndPrimitive();





}

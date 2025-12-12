#version 460 core 



layout(points) in;


layout(triangle_strip, max_vertices = 4 ) out;



uniform float u_scale;

layout (std140, binding = 0) uniform b_camera
{
	uniform mat4 u_view;
	uniform mat4 u_projection;
	uniform vec3 u_viewPos;
};
out vec2 texCoord;
out vec3 normal;
out vec3 fragmentPosition;




void main()
{


     vec3 pos = gl_in[0].gl_Position.xyz;
     
     
     vec3 toCam = u_viewPos - pos;
     vec3 up = vec3(0.0,1.0,0.0);
     vec3 forward = normalize(toCam) ;
     vec3 right = normalize(cross(forward,up)) * u_scale; 
     up *= u_scale;
     
     
      
     //bottom left
     vec3 corner0 = pos - right -  up;
    //bottom right
     vec3 corner1 = pos + right -  up;
     //top left
     vec3 corner2 = pos - right +  up;
     // top right 
     vec3 corner3 = pos + right +  up;
   
     // bottom left due to way opengl handles uv axis 
     normal = forward;
     texCoord = vec2(0.0,0.0);
     fragmentPosition = corner0;
     gl_Position = u_projection * u_view * vec4(corner0,1.0);
     EmitVertex();
     // bottom right 
     texCoord = vec2(1.0,0.0);
     fragmentPosition = corner1;
     gl_Position = u_projection * u_view * vec4(corner1,1.0);
     EmitVertex();
    // top left 
     texCoord = vec2(0.0,1.0);
     fragmentPosition = corner2;
     gl_Position = u_projection * u_view * vec4(corner2,1.0);
     EmitVertex();
  // top right 
     texCoord = vec2(1.0,1.0);
     fragmentPosition = corner3;
     gl_Position = u_projection * u_view * vec4(corner3,1.0);
     EmitVertex();
     EndPrimitive();
   


}


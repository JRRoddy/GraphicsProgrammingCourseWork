#version 460 core



layout(triangles) in;

layout(triangle_strip, max_vertices  = 5) out;

// we take in 3 points from the vertex shader that will be related to the gl_position calculated in the vertex shader of the now fragments
// there will be 3 of each piece of data as we have three points coming into the geomtery shader as the default primitve is triangles    
in vec3 vs_fragmentPos[];
in vec2 vs_texCoord[];
out mat3 TBN;
in vec3 vs_normal[];

layout (std140, binding = 0) uniform b_camera
{
	uniform mat4 u_view;
	uniform mat4 u_projection;
	uniform vec3 u_viewPos;
};

out vec3 fragmentPos;
out vec2 texCoord;

uniform int u_shouldUseCDM;

void main()
{
 
    vec3 v0 = vs_fragmentPos[1] - vs_fragmentPos[0];

    vec3 v1 = vs_fragmentPos[2] - vs_fragmentPos[0];

    
    vec3 normal =  normalize(cross(v0,v1)) ; 

    vec2 uv1 = vs_texCoord[1] - vs_texCoord[0];
    vec2 uv2 = vs_texCoord[2] - vs_texCoord[0];

    float r =  1.0 / (uv1.x * uv2.y -  uv1.y * uv2.x);

    vec3 tangent  = normalize( (v0 * uv2.y - v1 * uv1.y) * r);
    vec3 bitangent  = normalize((v1 * uv1.x - v0 * uv2.x) * r);


    TBN = mat3(tangent,bitangent,normal);
   for(int i = 0; i<3; i++)
   {
     
     // we have three vertices per triangle    
     // so we access the data at the specifc index for each vertex 
     // then we set up the data to be emitted for this vertex 
     // by setting the out variables for the fragment shader 
     // to be the data for that particualr index at the current index we are using 
     // this will prime this vertex data to be shipped to the fragment shader individually
      texCoord = vs_texCoord[i];
      fragmentPos = vs_fragmentPos[i];
      
    


     
     
      gl_Position = u_projection * u_view * vec4(fragmentPos,1.0);
       
      EmitVertex();
    

  
  }

    
   
  // need to know when we are finished emitting verticies
   EndPrimitive();

}
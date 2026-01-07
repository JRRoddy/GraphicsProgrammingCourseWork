#version 460 core



layout(triangles) in;

layout(triangle_strip, max_vertices  = 5) out;

// we take in 3 points from the vertex shader that will be related to the gl_position calculated in the vertex shader of the now fragments
// there will be 3 of each piece of data as we have three points coming into the geomtery shader as the default primitve is triangles    
in vec3 tse_fragmentPos[];
in vec2 tse_texCoord[];
in vec3 tse_normal[];

layout (std140, binding = 0) uniform b_camera
{
	uniform mat4 u_view;
	uniform mat4 u_projection;
	uniform vec3 u_viewPos;
};

out vec3 fragmentPos;
out vec3 normal;
out vec2 texCoord;
out mat3 TBN;
uniform int u_shouldUseCDM;

void main()
{
 
    vec3 v0 = tse_fragmentPos[1] - tse_fragmentPos[0];

    vec3 v1 = tse_fragmentPos[2] - tse_fragmentPos[0];

    
    normal =  normalize(cross(v0,v1)) ; 


    vec2 deltaUV0 = tse_texCoord[1]  - tse_texCoord[0]; 
    vec2 deltaUV1 = tse_texCoord[2]  - tse_texCoord[0]; 

    float r = 1.0 / (deltaUV0.x * deltaUV1.y - deltaUV0.y * deltaUV1.x);
     
    vec3 tangent = (v0 * deltaUV1.y - v1 * deltaUV0.y) * r;
    vec3 bitangent = (v1 * deltaUV0.x - v0 * deltaUV1.x) * r;
    
    TBN = mat3(tangent,bitangent,normal);
     
   for(int i = 0; i<3; i++)
   {
     
     // we have three vertices per triangle    
     // so we access the data at the specifc index for each vertex 
     // then we set up the data to be emitted for this vertex 
     // by setting the out variables for the fragment shader 
     // to be the data for that particualr index at the current index we are using 
     // this will prime this vertex data to be shipped to the fragment shader individually
      texCoord = tse_texCoord[i];
      fragmentPos = tse_fragmentPos[i];
      
      if(u_shouldUseCDM == 1)
      {
           normal = tse_normal[i];
      }


     
     
      gl_Position = u_projection * u_view * vec4(fragmentPos,1.0);
       
      EmitVertex();
    

  
  }

    
   
  // need to know when we are finished emitting verticies
   EndPrimitive();

}
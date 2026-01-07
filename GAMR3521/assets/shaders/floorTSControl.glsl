#version 460 core 


layout (vertices = 3) out;


in vec3 vs_fragmentPos[];
in vec3 vs_normal[];
in vec2 vs_texCoord[];

out vec3 tcs_fragmentPos[];
out vec2 tcs_texCoord[];
out vec3 tcs_normal[];
layout (std140, binding = 0) uniform b_camera
{
	uniform mat4 u_view;
	uniform mat4 u_projection;
	uniform vec3 u_viewPos;
};

float GetTesLevelDetail(float distance0, float distance1);

void main()
{

  // tessilation shaders are invoked per control point meanig for each patch(in this case a triangle) and as we are writing the same information for each control point 
  // we only need to send the data once
  // vertices in here are run in paralell


  float distance0 = distance(u_viewPos,vs_fragmentPos[0]);
  float distance1 = distance(u_viewPos,vs_fragmentPos[1]);
  float distance2 = distance(u_viewPos,vs_fragmentPos[2]);


   if(gl_InvocationID ==0)
   {
      
     // set the tessilsation levels for the primitve determinig the number of subdivisions at each level of the primitve
     gl_TessLevelOuter[0] = GetTesLevelDetail(distance1,distance2);
     gl_TessLevelOuter[1] = GetTesLevelDetail(distance2,distance0);
     gl_TessLevelOuter[2] = GetTesLevelDetail(distance0,distance1);
     gl_TessLevelInner[0] = gl_TessLevelOuter[2];

   }
   



   // invoation id is for each control point(vertex) and we have three vertcies being invoked so for each invocation we use that control point's id to get the correct data for that control point 
   tcs_fragmentPos[gl_InvocationID] = vs_fragmentPos[gl_InvocationID];
   tcs_texCoord[gl_InvocationID] = vs_texCoord[gl_InvocationID];
   tcs_normal[gl_InvocationID] = vs_normal[gl_InvocationID];


   

}

float GetTesLevelDetail(float distance0, float distance1)
{
  
  float avrgDistance = (distance0 + distance1) / 2; 
  
  float constA  = 0.8;
  float constB = 0.7; 
  float linDist =  abs((-constA*avrgDistance) + constB);
 
  if(linDist <= 2.0f)
  {
    return 20.0f;
  }
  else if(linDist <= 10.0f)
  {
     return 10.0f;
  }
  else if(linDist <= 20.0)
  {
    return 8.0f;
  }
  return 3.0f;



}

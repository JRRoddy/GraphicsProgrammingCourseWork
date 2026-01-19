# version 460 core 

// specify that we are taking in triangles as a primitve meaning we expect three vertcies and that the subdivsion spacing is even 
// whcih will control any rounding required when doing the subdivision 
// also specifcy the winding order of the vertices which specifces the order in which we trace the vertices and is used to interpret wheteher or not we 
// have a back or front face 

layout(triangles, fractional_even_spacing, ccw) in;




in vec3 tcs_fragmentPos[];
in vec2 tcs_texCoord[];
in vec3 tcs_normal[];


out vec3 tse_fragmentPos;
out vec2 tse_texCoord;
out vec3 tse_normal;

uniform sampler2D u_cdmNormalMap;

uniform float u_heightScalar;
uniform float u_terrainHeightOffset;
vec2 interpolate2D(vec2 v0, vec2 v1, vec2 v2);
vec3 interpolate3D(vec3 v0, vec3 v1, vec3 v2);

void main()
{
    
    
    tse_texCoord = interpolate2D(tcs_texCoord[0],tcs_texCoord[1],tcs_texCoord[2]);
    vec4 terrainInfo = texture(u_cdmNormalMap,tse_texCoord);
    float height = terrainInfo.a;
   
    float terrainHeightOffset = u_terrainHeightOffset;
  
    tse_fragmentPos = interpolate3D(tcs_fragmentPos[0],tcs_fragmentPos[1],tcs_fragmentPos[2]);

    tse_fragmentPos.y =  (u_heightScalar * height) + u_terrainHeightOffset;


    // centeral difference method for finding a normal using a singular axis 
    // calcualte the y value at the surrounding pixels in the height map(at each axis offset)
    //float right = (textureOffset(u_heightMap,tse_texCoord,ivec2(1,0)).r) * u_heightScalar;
    //float left  =  (textureOffset(u_heightMap,tse_texCoord,ivec2(-1,0)).r) * u_heightScalar;
    //float up = (textureOffset(u_heightMap,tse_texCoord,ivec2(0,-1)).r) * u_heightScalar;
    //float down = (textureOffset(u_heightMap,tse_texCoord,ivec2(0,1)).r) * u_heightScalar;

    // calculate difference between the neighbouring points
    //float lr = left - right;
    //float ud = up - down; 



    // approximation of normal using the central difference 
    // cdm normals are caluclated in a compute shader per pixel once to save on sampling the height map every frame
    // at different offsets(only to get the same data back every time as the height map is not dynamic)
    tse_normal = terrainInfo.xyz;
    

  

}


// used to interpolate all of the vec2 data being recived form the controml shader across the triangle using the baycentric fractional coordinates
//  provided by the primitive generation  step which takes place before the tess evaulation shader 
vec2 interpolate2D(vec2 v0, vec2 v1, vec2 v2)
{
   
   vec2 interp = vec2(gl_TessCoord.x) * v0 + vec2(gl_TessCoord.y) * v1 + vec2(gl_TessCoord.z) * v2;

   return interp;

 
}

vec3 interpolate3D(vec3 v0, vec3 v1, vec3 v2)
{
   
   vec3 interp = vec3(gl_TessCoord.x) * v0 + vec3(gl_TessCoord.y) * v1 + vec3(gl_TessCoord.z) * v2;

   return interp;

 
}
  



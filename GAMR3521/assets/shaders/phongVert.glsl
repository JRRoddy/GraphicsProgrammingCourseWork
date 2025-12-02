#version 460 core
			
layout(location = 0) in vec3 a_vertexPosition;
layout(location = 1) in vec3 a_vertexNormal;
layout(location = 2) in vec2 a_texCoord;
layout(location = 3) in vec3 a_tangent;
out vec4 fragmentPosLightSpace;
out vec3 fragmentPos;
out vec3 normal;
out vec2 texCoord; 
out mat3 tangentToWorld;
out vec4 clipSpaceCoords;
layout (std140, binding = 0) uniform b_camera
{
	uniform mat4 u_view;
	uniform mat4 u_projection;
	uniform vec3 u_viewPos;
};

uniform mat4 u_model;

void main()
{


	fragmentPos = vec3(u_model * vec4(a_vertexPosition, 1.0));
	normal = normalize(mat3(transpose(inverse(u_model))) * a_vertexNormal); 
        
    vec3 tangentWorld = (vec4(a_tangent,0.0f) * u_model).xyz; 
    tangentWorld = normalize(tangentWorld - dot(tangentWorld,normal) * normal);
        
    vec3 bitangent = cross(normal,tangentWorld); 
       
    bitangent = normalize(bitangent); 
        
    tangentToWorld = mat3(tangentWorld,bitangent,normal);
	texCoord = a_texCoord;


    clipSpaceCoords = u_projection * u_view * vec4(fragmentPos,1.0);
	gl_Position = clipSpaceCoords;
}
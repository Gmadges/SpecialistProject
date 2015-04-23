#version 330 core
/// @brief the vertex passed in
layout (location =0) in vec3 inVert;
/// @brief the normal passed in
layout (location =2) in vec3 inNormal;
/// @brief the in uv
layout (location =1) in vec2 inUV;
uniform mat4 MVP;

uniform float normalSize;
uniform vec4 vertNormalColour;
uniform vec4 faceNormalColour;

out vec4 normal;

uniform bool drawFaceNormals;
uniform bool drawVertexNormals;

void main()
{
        v_g_normal = a_normal;
        gl_Position = a_vertex;
}

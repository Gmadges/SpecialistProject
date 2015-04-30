#version 330 core

/// @brief MVP passed from app
uniform mat4 MVP;
// first attribute the vertex values from our VAO
layout (location =0) in vec3 inVert;

void main()
{
	// calculate the vertex position
        gl_Position = MVP*vec4(inVert, 1.0);

}

#version 330 core

// first attribute the vertex values from our VAO
layout (location =0)in vec3 inPos;


out vec2 vertUV;

void main()
{
    gl_Position = vec4(inPos, 1.0);

}

#version 330 core
layout (location =0)in vec3 inPos;
layout (location =2) in vec3 inNormal;

out vec4 normal;

uniform mat4 MVP;

void main()
{
    gl_Position = vec4(inPos, 1.0);

    normal = MVP*vec4(inNormal, 0);
}

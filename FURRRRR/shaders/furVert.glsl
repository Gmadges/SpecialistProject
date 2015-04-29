#version 330 core
layout (location =0)in vec3 inPos;
layout (location =2) in vec3 inNormal;
layout (location =1) in vec2 inUV;

out vec4 normal;
out vec2 v_UV;

uniform mat4 MVP;

void main()
{
    gl_Position = vec4(inPos, 1.0);

    normal = vec4(inNormal, 0);

    v_UV = inUV;
}

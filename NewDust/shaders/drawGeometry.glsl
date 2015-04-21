#version 330 core

layout (points) in;
layout (triangle_strip) out;
layout (max_vertices = 4) out;

uniform mat4 ViewProjection;
uniform vec3 camPosition;

out vec2 texCoord;

void main()
{
    //get the position of the point.
    vec3 pos = gl_in[0].gl_Position.xyz;

    //get camera and view info
    vec3 toCamera = normalize(camPosition - pos);
    vec3 up = vec3(0.0, 1.0, 0.0);
    vec3 right = cross(toCamera, up);

    //set each vertex of the quad.
    //
    //  2---4
    //  |   |
    //  1---3

    pos -= (right * 0.5);
    gl_Position = ViewProjection * vec4(pos, 1.0);
    texCoord = vec2(0.0, 0.0);
    EmitVertex();

    pos.y += 1.0;
    gl_Position = ViewProjection * vec4(pos, 1.0);
    texCoord = vec2(0.0, 1.0);
    EmitVertex();

    pos.y -= 1.0;
    pos += right;
    gl_Position = ViewProjection * vec4(pos, 1.0);
    texCoord = vec2(1.0, 0.0);
    EmitVertex();

    pos.y += 1.0;
    gl_Position = ViewProjection * vec4(pos, 1.0);
    texCoord = vec2(1.0, 1.0);
    EmitVertex();

    EndPrimitive();

}























































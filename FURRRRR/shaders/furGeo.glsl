#version 400

uniform mat4 MVP;

layout(triangles, invocations=5) in;
layout(points, max_vertices = 9) out;

in vec4 normal[];



void main()
{
  for(int i = 0; i < 3; i++)
  { // You used triangles, so it's always 3

        vec4 tmp = MVP * gl_in[i].gl_Position;

        vec4 offset = normal[0];

        tmp+=(offset*gl_InvocationID*0.01);

        gl_Position = tmp;
        EmitVertex();

  }
  EndPrimitive();
}

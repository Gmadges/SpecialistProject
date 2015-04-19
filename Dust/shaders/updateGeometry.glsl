#version 330

layout(points) in;
layout(points) out;
layout(max_vertices = 30) out;

in float Type0[];
in vec3 Position0[];
in vec3 Velocity0[];
in float Age0[];

out float Type1;
out vec3 Position1;
out vec3 Velocity1;
out float Age1;

uniform float gDeltaTimeMillis;
uniform float gTime;

void main()
{

    if(Age1 <= 50000)
    {
        Position1 = Position0[0] + Velocity0[0];

        Velocity1 = Velocity0[0] + vec3(0, -9.8, 0);
    }
    else
    {
        Position1 = vec3(0,0,0);

        vec3 tmp;

        tmp.x = noise1(gTime) * 5;
        tmp.y = 100;
        tmp.z = noise1(gTime) * 5;

        Velocity1 = tmp;

        Age1 = 0.0f;
    }

    Age1 = Age0 + 1;
}


#version 330

layout (location = 0) in float Type;
layout (location = 1) in vec3 Position;
layout (location = 2) in vec3 Velocity;
layout (location = 3) in float Age;

out float Type0;
out vec3 Position0;
out vec3 Velocity0;
out float Age0;

uniform float DeltaTimeMillis;
uniform float Time;

void main()
{
    if(Age <= 50000)
    {
        Position0 = Position + Velocity;

        Velocity0 = Velocity + vec3(0.0, -9.8, 0.0);
    }
    else
    {
        Position0 = vec3(0.0, 0.0, 0.0);

        vec3 tmp;

        tmp.x = noise1(Time + gl_VertexID) * 5.0;
        tmp.y = 100.0;
        tmp.z = noise1(Time + gl_VertexID) * 5.0;

        Velocity0 = tmp;

        Age0 = 0.0f;
    }

    Type0 = Type;

    Age0 += (DeltaTimeMillis/1000.0);

}

#version 330

layout (location = 0) in vec3 Position;
layout (location = 1) in vec3 Velocity;
layout (location = 2) in int Age;

out vec3 Position0;
out vec3 Velocity0;
out float Age0;

uniform int Time;

void main()
{

    if(Age <= 50000)
    {
        Position0 = Position + Velocity;

        Velocity0 = Velocity + vec3(0.0, -9.8, 0.0);

        Age0 = Age + Time;
    }
    else
    {
        Position0 = vec3(0.0, 0.0, 0.0);

        vec3 tmp;

        tmp.x = 2;
        tmp.y = 500.0;
        tmp.z = 2;

        Velocity0 = tmp;

        Age0 = 0;
    }
}

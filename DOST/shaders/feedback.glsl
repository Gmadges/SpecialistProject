#version 330

layout (location = 0) in vec3 Position;
layout (location = 1) in vec3 Velocity;
layout (location = 2) in float Age;

out vec3 Position0;
out vec3 Velocity0;
out float Age0;

//uniform float DeltaTimeMillis;
//uniform float Time;

float rand(vec2 n)
{
  return 0.5 + 0.5 *
     fract(sin(dot(n.xy, vec2(12.9898, 78.233)))* 43758.5453);
}

void main()
{
    if(Age <= 100)
    {
        Position0 = Position + Velocity;

        Velocity0 = Velocity;

        Age0 = Age + 1;
    }
    else
    {
        Position0 = vec3(0.0, 0.0, 0.0);

        float tmp = rand(Position.xy);

        vec3 vel;

        vel.x = tmp * 10;
        vel.y = tmp * 20;
        vel.z = tmp * 10;

        Velocity0 = Velocity;

        Age0 = 0.0f;
    }
}

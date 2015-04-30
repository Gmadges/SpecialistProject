#version 330

layout (location = 0) in vec3 Position;
layout (location = 1) in vec3 Velocity;
layout (location = 2) in float Age;

out vec3 Position0;
out vec3 Velocity0;
out float Age0;

uniform float DeltaTimeMillis;
uniform float Time;

float rand(vec2 n)
{
  return 0.5 + 0.5 *
     fract(sin(dot(n.xy, vec2(12.9898, 78.233)))* 43758.5453);
}

void main()
{
    float DeltaTime = DeltaTimeMillis/1000;

    if(Age <= 0)
    {
        Age0 = Age + DeltaTime;

        Position0 = Position;

        Velocity0 = Velocity;
    }
    else if(Age < 2)
    {
        vec3 newVelocity = Velocity - vec3(0,2,0);

        Velocity0 = newVelocity;

        Position0 = Position + (newVelocity*DeltaTime);

        Age0 = Age + DeltaTime;
    }
    else
    {
        Position0 = Position;

        Velocity0 = Velocity;

        Age0 = Age;
    }

}

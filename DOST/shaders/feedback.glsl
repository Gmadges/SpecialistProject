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

mat4 rotationMatrix(vec3 axis, float angle)
{
    axis = normalize(axis);
    float s = sin(angle);
    float c = cos(angle);
    float oc = 1.0 - c;

    return mat4(oc * axis.x * axis.x + c,           oc * axis.x * axis.y - axis.z * s,  oc * axis.z * axis.x + axis.y * s,  0.0,
                oc * axis.x * axis.y + axis.z * s,  oc * axis.y * axis.y + c,           oc * axis.y * axis.z - axis.x * s,  0.0,
                oc * axis.z * axis.x - axis.y * s,  oc * axis.y * axis.z + axis.x * s,  oc * axis.z * axis.z + c,           0.0,
                0.0,                                0.0,                                0.0,                                1.0);
}

vec3 vortex(vec3 vortPos, vec3 pos, vec3 vel)
{
    vec3 r = Position - vortPos;

    vec3 angularVelocity =  (cross(r, vel)) /
                            (length(r)*length(r));


    angularVelocity*=10;

    vec3 v = cross(angularVelocity, r);

    float factor = 1/(1+(r.x*r.x+r.z*r.z) / 1);

    return v*factor;
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
    else if(Age < 3)
    {
        vec3 newVelocity = Velocity - vec3(0,2,0);

        newVelocity+=vortex(vec3(10,5,-10), Position, newVelocity);

        newVelocity+=vortex(vec3(10,10, 40), Position, newVelocity);

        newVelocity+=vortex(vec3(-23,20, 10), Position, newVelocity);

        newVelocity+=vortex(vec3(-10,-5, -13), Position, newVelocity);


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

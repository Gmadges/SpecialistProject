#version 330

layout (location = 0) in vec3 Position;
layout (location = 1) in vec3 Velocity;
layout (location = 2) in float Age;

out vec3 Position0;
out vec3 Velocity0;
out float Age0;

uniform float DeltaTimeMillis;
uniform float Time;

uniform sampler3D Sampler;


float rand(vec2 n)
{
  return 0.5 + 0.5 *
     fract(sin(dot(n.xy, vec2(12.9898, 78.233)))* 43758.5453);
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

vec3 SampleVelocity(vec3 p)
{
    vec3 tc;
    vec3 Extent = vec3(1, 1, 1);

    tc.x = (p.x + Extent.x) / (2 * Extent.x);

    tc.y = (p.y + Extent.y) / (2 * Extent.y);

    tc.z = (p.z + Extent.z) / (2 * Extent.z);

    return texture(Sampler, tc).xyz;
}

vec3 ComputeCurl(vec3 P)
{
  float eps = 0.1;

  float n1,n2,a,b;

  vec3 curl;

  n1 = texture(Sampler, vec3(P.x,P.y+eps,P.z), 1).b;
  n2 = texture(Sampler, vec3(P.x,P.y-eps,P.z), 1).b;

  a = (n1-n2)/(2*eps);

  n1 = texture(Sampler,vec3(P.x,P.y,P.z+eps), 1).b;
  n2 = texture(Sampler,vec3(P.x,P.y,P.z-eps), 1).b;

  b = (n1 - n2) / (2 * eps);

  curl.x = a-b;

  n1 = texture(Sampler, vec3(P.x,P.y,P.z+eps), 1).b;
  n2 = texture(Sampler, vec3(P.x,P.y,P.z-eps), 1).b;

  a = (n1-n2)/(2*eps);

  n1 = texture(Sampler, vec3(P.x+eps,P.y,P.z), 1).b;
  n2 = texture(Sampler, vec3(P.x+eps,P.y,P.z), 1).b;

  b = (n1 - n2) / (2 * eps);

  curl.y = a-b;

  n1 = texture(Sampler, vec3(P.x+eps,P.y,P.z), 1).b;
  n2 = texture(Sampler, vec3(P.x-eps,P.y,P.z), 1).b;

  a = (n1 - n2) / (2 * eps);

  n1 = texture(Sampler, vec3(P.x,P.y+eps,P.z), 1).r;
  n2 = texture(Sampler, vec3(P.x,P.y-eps,P.z), 1).r;

  b = (n1-n2)/(2*eps);

  curl.z = a-b;

  return curl;
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
    else if(Age < 5)
    {
        vec3 newVelocity = Velocity + vec3(0,0,0);

        newVelocity += -Velocity*0.05;

        Velocity0 = newVelocity += ComputeCurl(normalize(Position));
        //Velocity0 = newVelocity += SampleVelocity(normalize(Position));

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

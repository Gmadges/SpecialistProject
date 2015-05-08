#version 400

#define FUR_LAYERS 50

#define FUR_LENGTH 0.05

uniform mat4 MVP;
uniform mat4 MV;
uniform mat3 normalMatrix;

layout(triangles, invocations=1) in;
//layout(triangle_strip, max_vertices = 9) out;
layout(triangle_strip, max_vertices = 87) out;

in vec4 normal[];
in vec2 v_UV[];

out vec3 v_g_normal;
out vec2 v_g_UV;
out float v_furStrength;

void main()
{
  vec3 norm;

  const float FUR_DELTA = 1.0 / float(FUR_LAYERS);

  float d = 0.0;

  for (int furLayer = 0; furLayer < FUR_LAYERS; furLayer++)
  {
          d += FUR_DELTA;

          for(int i = 0; i < gl_in.length(); i++)
          {
                  norm = normalize(normal[i]).xyz;

                  v_g_normal = normalMatrix* norm;

                  v_g_UV = v_UV[i];

                  // If the distance of the layer is getting bigger to the original surface, the layer gets more transparent.
                  v_furStrength = 1.0 - d;

                  // Displace a layer along the surface normal.
                  gl_Position = MVP * (gl_in[i].gl_Position + vec4(norm * d * FUR_LENGTH, 0.0));

                  EmitVertex();
          }

          EndPrimitive();
  }
}

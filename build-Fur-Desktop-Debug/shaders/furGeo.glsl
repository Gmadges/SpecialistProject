#version 330 core
layout(triangles) in;
layout(line_strip, max_vertices = 10) out;

in vec4 normal[];

uniform  float normalSize;
uniform  vec4 vertNormalColour;
uniform  vec4 faceNormalColour;
uniform bool drawFaceNormals;
uniform bool drawVertexNormals;
out vec4 perNormalColour;


void main(void)
{
        vec3 normal;

        const float FUR_DELTA = 1.0 / float(FUR_LAYERS);

        float d = 0.0;

        for (int furLayer = 0; furLayer < FUR_LAYERS; furLayer++)
        {
                d += FUR_DELTA;

                for(int i = 0; i < gl_in.length(); i++)
                {
                        normal = normalize(v_g_normal[i]);

                        v_normal = u_normalMatrix * normal;

                        v_texCoord = v_g_texCoord[i];

                        // If the distance of the layer is getting bigger to the original surface, the layer gets more transparent.
                        v_furStrength = 1.0 - d;

                        // Displace a layer along the surface normal.
                        gl_Position = u_projectionMatrix * u_modelViewMatrix * (gl_in[i].gl_Position + vec4(normal * d * FUR_LENGTH, 0.0));

                        EmitVertex();
                }

                EndPrimitive();
        }
}

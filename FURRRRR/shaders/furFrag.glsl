#version 330 core

// The higher the value, the bigger the contrast between the fur length.
#define FUR_STRENGTH_CONTRAST 1.5

// The higher the value, the less fur.
#define FUR_STRENGTH_CAP 0.3

// this is a pointer to the current 2D texture object
uniform sampler2D furStrengthTexture;

uniform sampler2D imageTexture;
// the vertex UV

in vec3 v_g_normal;
in float v_furStrength;
in vec2 v_g_UV;

in vec2 texCoord;
// the final fragment colour

out vec4 FragColour;

void main()
{
    vec3 light = vec3(200,1,100);

    vec3 normal = normalize(v_g_normal);

    // Orthogonal fur to light is still illumintated. So shift by one, that only fur targeting away from the light do get darkened.
    //float intensity = clamp(dot(normal, light) + 1.0, 0.0, 1.0);

    float intensity = 1.0f;

    float furStrength = clamp(v_furStrength * texture(furStrengthTexture, v_g_UV).g * FUR_STRENGTH_CONTRAST - FUR_STRENGTH_CAP,
                              0.0,
                              1.0);

    if(furStrength < 0.3)
    {
        discard;
    }

    //apply darker shades to bottom layers
    vec4 colour = vec4(texture(imageTexture, v_g_UV).rgb * intensity, furStrength);

    FragColour = colour;

    if(FragColour.r > 0.5 && FragColour.g > 0.5 && FragColour.b > 0.5)
    {
        discard;
    }


}

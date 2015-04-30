#version 330 core

// The higher the value, the bigger the contrast between the fur length.
#define FUR_STRENGTH_CONTRAST 2.0

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
    vec3 light = vec3(200,1,1);

    //FragColour = texture2D(tex, texCoord);

    //vec3 normal = normalize(v_normal);

    //FragColour = vec4(0.2,1,0.2,0.2);

    vec3 normal = normalize(v_g_normal);

    // Orthogonal fur to light is still illumintated. So shift by one, that only fur targeting away from the light do get darkened.
    float intensity = clamp(dot(normal, light) + 1.0, 0.0, 1.0);

    //float intensity = 0.5;

    float furStrength = clamp(v_furStrength * texture(furStrengthTexture, v_g_UV).r * FUR_STRENGTH_CONTRAST - FUR_STRENGTH_CAP, 0.0, 1.0);

    //float furStrength = 0.1;



    //FragColour = vec4(vec3(0.1, 1, 0.1) * intensity, furStrength);


    if(intensity != 0.0)
    {
        FragColour = vec4(texture(imageTexture, v_g_UV).rgb * intensity, furStrength);

        if (FragColour.r == 0 && FragColour.g == 0 && FragColour.b == 0)
        {
             discard;
        }
    }

    else
    {
        FragColour = vec4(texture(furStrengthTexture, v_g_UV).rgb * 1, furStrength);
    }

}

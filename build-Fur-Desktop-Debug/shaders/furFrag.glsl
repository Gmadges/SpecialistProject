#version 330 core

/// @brief our output fragment colour
layout (location =0 )out vec4 fragColour;
in vec4 perNormalColour;
void main()
{
        // Note: All calculations are in camera space.
        vec3 normal = normalize(v_normal);

        // Orthogonal fur to light is still illumintated. So shift by one, that only fur targeting away from the light do get darkened.
        float intensity = clamp(dot(normal, u_lightDirection) + 1.0, 0.0, 1.0);

        float furStrength = clamp(v_furStrength * texture(u_textureFurStrength, v_texCoord).r * FUR_STRENGTH_CONTRAST - FUR_STRENGTH_CAP, 0.0, 1.0);

        fragColor = vec4(texture(u_textureFurColor, v_texCoord).rgb * intensity, furStrength);
}

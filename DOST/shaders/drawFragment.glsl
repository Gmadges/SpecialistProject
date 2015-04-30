#version 410 core
// this is a pointer to the current 2D texture object
uniform sampler2D tex;

layout (location = 2) in float Age;

in vec2 texCoord;
// the final fragment colour

out vec4 FragColour;

void main()
{
    //FragColour = texture2D(tex, texCoord);

    //float alpha = v_g_age/1.0f;
    //float alpha = 0.5;

    FragColour = vec4(1,1,1, 0.5);

    //if (FragColour.r == 0 && FragColour.g == 0 && FragColour.b == 0)
    //{
    //    discard;
    //}
}

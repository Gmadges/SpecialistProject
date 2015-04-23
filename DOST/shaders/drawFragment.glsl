#version 330 core
// this is a pointer to the current 2D texture object
uniform sampler2D tex;
// the vertex UV

in vec2 texCoord;
// the final fragment colour

out vec4 FragColour;

void main()
{
    FragColour = texture2D(tex, texCoord);

    //FragColour = vec4(1,0,0,1);

    if (FragColour.r == 0 && FragColour.g == 0 && FragColour.b == 0)
    {
        discard;
    }
}

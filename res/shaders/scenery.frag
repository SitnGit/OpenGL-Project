#version 330 core
out vec4 FragColor;
in vec2 TexCoord;

// texture samplers
//uniform sampler2D texture1;
uniform sampler2D texture2;

void main()
{
        // linearly interpolate between both textures (80% container, 20% awesomeface)
        FragColor = texture(texture2, TexCoord);
        //FragColor = vec4(0.0f, 0.0f, 1.0f,1.0f);
}

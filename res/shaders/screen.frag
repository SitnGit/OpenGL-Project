#version 330 core
out vec4 FragColor;
in vec2 TexCoord;

// texture samplers
uniform sampler2D textureWin;
uniform sampler2D textureLose;
uniform sampler2D textureStart;
//uniform sampler2D texture1;
uniform int which;

void main()
{
        
        if(which==0)
        FragColor = texture(textureWin, TexCoord);
        else if(which == 1)
        FragColor = texture(textureLose, TexCoord);
        else
        FragColor = texture(textureStart, TexCoord);
        //FragColor = vec4(1.0f,0.0f,0.0f,0.0f);
}

#version 330 core
layout (location = 0) in vec3 aPos;

out vec3 FragPos;
out vec3 Normal;

uniform vec3 cent;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    vec3 Pos = aPos;
    //Pos += cent;
    FragPos = vec3(model * vec4(Pos, 1.0));
    Normal = normalize(FragPos - cent);

    gl_Position = projection * view * vec4(FragPos, 1.0);
}

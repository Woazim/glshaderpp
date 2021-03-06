#version 330 core

in vec4 color; //Même nom que la sortie du vertex shader
out vec4 fragColor;

void main()
{
    fragColor = color;
}

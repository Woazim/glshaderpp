#version 330 core
  
layout(location = 0) in vec2 position;
layout(location = 1) in vec3 inputcolor;
layout(location = 2) in float phase;

uniform float fTime;

out vec4 color;

void main()
{
    gl_Position = vec4(position, 0.0f, 1.0f);

	color = vec4(inputcolor, (sin(fTime+phase) + 1) / 2.0f);
}
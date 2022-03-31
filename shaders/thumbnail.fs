#version 330 core
layout(location = 0) out vec3 outColor;


in vec4 vertColor;
in vec2 TexCoords;
out vec4 FragColor;
in vec3 color;

uniform vec3 acolor = vec3(1);

void main()
{
  FragColor = vec4(1.0);
  outColor = FragColor.rgb;
}

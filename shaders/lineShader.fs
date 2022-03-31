#version 330

uniform vec2 resolution = vec2(500.0, 500.0);
uniform float Thickness = 0.003;
out vec4 fragColor;
in vec4 vertColor;
uniform vec3 color = vec3(1.0);

void main()
{
  fragColor = vertColor;
}

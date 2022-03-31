#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 8) in vec3 aOffset;

uniform mat4 model = mat4(1.0);
uniform mat4 view;
uniform mat4 projection;
out vec3 color;
out vec2 TexCoords;

void main()
{
  gl_Position = projection*view*model*vec4(aPos+aOffset, 1.0f);
}

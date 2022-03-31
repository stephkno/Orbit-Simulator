#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aColor;
layout (location = 4) in vec3 aTangent;
layout (location = 5) in vec3 aBitangent;
layout (location = 6) in ivec4 boneIds;
layout (location = 7) in vec4 weights;

uniform mat4 model = mat4(1.0);
uniform mat4 view;
uniform mat4 projection;
out vec3 color;

void main()
{
  color = aColor;
  gl_Position = projection * view * model * vec4(aPos, 1.0);
}

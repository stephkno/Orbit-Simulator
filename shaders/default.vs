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
out vec2 TexCoords;
uniform vec3 position=vec3(0.0);

void main()
{
  color = aColor;
  TexCoords = aTexCoords;
  vec3 p = aPos;
  p.y = p.y * -1.0;
  gl_Position = projection*view*model*vec4(p+position, 1.0f);
}

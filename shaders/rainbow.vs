#version 330


layout (location = 0) in vec3 vPosition;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aColors;
layout (location = 4) in vec3 aTangent;
layout (location = 5) in vec3 aBitangent;

uniform mat4 projection;
uniform mat4 view; //proj * view product, precomputed on CPU
uniform mat4 model; //model -> world
uniform float time;

out vec4 vertColor;
out vec2 TexCoords;

void main()
{
  vertColor = vec4(aColors, 1);
  gl_Position = projection * view * model * vec4(vPosition, 1);
}

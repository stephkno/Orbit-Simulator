#version 330 core
out vec4 FragColor;
layout (location = 0) out vec3 gSkyboxMap;

in vec3 TexCoords;

uniform samplerCube skybox;
uniform vec3 color = vec3(1.0f);

void main()
{
  FragColor = texture(skybox, TexCoords) * vec4(color,1);
}

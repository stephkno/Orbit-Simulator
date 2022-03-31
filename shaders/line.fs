#version 330 core
// 7 textures from vec3
// 15 from vec4
// otherwise you just need to use a SSBO int index for each vertex
out vec4 FragColor;
uniform vec3 color=vec3(1.0);

void main()
{
  FragColor = vec4(color,1.0);
}

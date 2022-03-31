#version 330 core

layout (location = 0) in vec4 vertex; // <vec2 position, vec2 texCoords>

out vec2 TexCoords;

uniform mat4 model;
uniform mat4 projection;

#define MAX_BATCH 512

// vec offset to be used with sprite instancing
uniform vec2 VecOffset[512];
uniform vec2 TexOffset = vec2(0,0);

uniform int units = 1;
uniform int batch = 1;

void renderSprite( vec2 vo, vec2 to )
{
  TexCoords = ( vertex.zw / units ) + to;
  gl_Position = projection * model * vec4(vertex.xy + vo, 0.0, 1.0);
}

void main()
{
  for(int i = 0; i < batch; i++){
    renderSprite(VecOffset[i], TexOffset);
  }
}

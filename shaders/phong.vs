#version 330
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 UV;
layout (location = 3) in vec3 color;
layout (location = 4) in vec3 tangent;
layout (location = 5) in vec3 bitangent;
layout (location = 6) in ivec4 boneIds;
layout (location = 7) in vec4 weights;

uniform mat4 projection;
uniform mat4 model;
uniform mat4 view;
uniform mat4 normalMat;
uniform mat4 lightSpaceMatrix;
uniform vec2 texScale = vec2(1.0,1.0);

out vec2 uv;
out vec3 fn;
out vec3 vertPos;
out vec3 FragPos;
out vec4 FragPosLightSpace;

const int MAX_BONES = 100;
const int MAX_BONE_INFLUENCE = 4;
uniform mat4 idleBones[MAX_BONES];
uniform mat4 fBones[MAX_BONES];
uniform mat4 bBones[MAX_BONES];
uniform mat4 lBones[MAX_BONES];
uniform mat4 rBones[MAX_BONES];

uniform bool animate;
uniform float blendX;
uniform float blendY;
uniform float idleAnimationBlend;

mat4 interpolate(mat4 a, mat4 b, float t){
  return ( a * (1.0-t) ) + (b * t);
}

void main(){
  uv = UV * texScale;
  //fn = vec3(normalMat * vec4(normal, 0.0));
  fn = normal;
  vec4 vertPos4 = model * view * vec4(position, 1.0);
  vertPos = vec3(vertPos4) / vertPos4.w;
  FragPos = vec3(model * vec4(position, 1.0));
  FragPosLightSpace = lightSpaceMatrix  * vec4(position, 1.0);

  if(animate){
    vec4 totalPosition = vec4(0.0);

    for(int i = 0 ; i < MAX_BONE_INFLUENCE ; i++)
    {

        if(boneIds[i] == -1){
          continue;
        }
        if(boneIds[i] >= MAX_BONES)
        {
            totalPosition = vec4(position,1.0f);
            break;
        }
        mat4 A;
        mat4 B;

        float xAxis = blendX;
        float yAxis = blendY;
        if(xAxis < 0.01 && yAxis < 0.01){
          xAxis = 0.0;
          yAxis = 0.0;
        }

        if(blendY < 0){
          A = interpolate(idleBones[boneIds[i]], fBones[boneIds[i]], abs(blendY));
        }
        else if(blendY > 0){
          A = interpolate(idleBones[boneIds[i]], bBones[boneIds[i]], abs(blendY));
        }else if(blendY == 0.0){
          B = idleBones[boneIds[i]];
        }
        if(blendX > 0){
          B = interpolate(idleBones[boneIds[i]], rBones[boneIds[i]], abs(blendX));
        }
        else if(blendX < 0){
          B = interpolate(idleBones[boneIds[i]], lBones[boneIds[i]], abs(blendX));
        }else if(blendX == 0.0){
          B = idleBones[boneIds[i]];
        }
        mat4 localPosition = interpolate(A, B, 0.5f);
        totalPosition += (localPosition * vec4(position, 1.0f)) * weights[i];
        vec3 localNormal = mat3(idleBones[boneIds[i]]) * normal;

    }

    gl_Position = projection * view * model * totalPosition;

  }else{

    gl_Position = projection * view * model * vec4(position,1);

  }

  //gl_Position = projection * view * model * vec4(position, 1.0);
}

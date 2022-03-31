#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aColor;
layout (location = 4) in vec3 aTangent;
layout (location = 5) in vec3 aBitangent;
layout (location = 6) in ivec4 boneIds;
layout (location = 7) in vec4 weights;
layout (location = 8) in vec3 translation;

const int MAX_BONES = 100;
const int MAX_BONE_INFLUENCE = 4;
const int CHUNK_SIZE = 200;

out vec3 FragPos;
out vec2 TexCoords;
out vec3 Normal;
out vec4 FragPosLightSpace;
out vec3 vectorPos;
out vec3 wNorm;
out vec3 color;

out vec3 TangentLightPos;
out vec3 TangentViewPos;
out vec3 TangentFragPos;

uniform mat4 model = mat4(1.0);
uniform mat4 view;
uniform mat4 projection;
uniform mat4 lightSpaceMatrix;
uniform vec3 LightPos;
uniform vec3 ViewPos;
uniform vec2 textureScale=vec2(1);
uniform float t;

uniform float animationBlend;
uniform bool animate;
uniform bool wind;
uniform mat4 finalBoneTransformsA[MAX_BONES];
uniform mat4 finalBoneTransformsB[MAX_BONES];
uniform float windAmplitude = 0.05f;

mat4 interpolate(mat4 a, mat4 b, float t){
  return ( a * (1.0-t) ) + (b * t);
}
void main()
{
    color = aColor;

    vec4 worldPos = model * vec4(aPos, 1.0);
    //FragPos = worldPos.xyz;
    TexCoords = aTexCoords*textureScale;
    mat3 normalMatrix = transpose(inverse(mat3(mat4(1))));
    vec3 T = normalize(normalMatrix * aTangent);
    vec3 N = normalize(normalMatrix * aNormal);
    T = normalize(T - dot(T, N) * N);
    vec3 B = cross(N, T);

    mat3 TBN = transpose(mat3(T, B, N));
    TangentLightPos = TBN * LightPos;
    TangentViewPos  = TBN * ViewPos;
    TangentFragPos  = TBN * FragPos;

    Normal = normalMatrix * aNormal;
    wNorm = aNormal;
    FragPos = vec3(model * vec4(aPos, 1.0));
    FragPosLightSpace = lightSpaceMatrix * vec4(FragPos, 1.0);

    if(animate){
      vec4 totalPosition = vec4(0.0);
      vec4 localPosition;

      for(int i = 0 ; i < MAX_BONE_INFLUENCE ; i++)
      {

          if(boneIds[i] == -1){
            continue;
          }
          if(boneIds[i] >= MAX_BONES)
          {
              totalPosition = vec4(aPos,1.0f);
              break;
          }
          localPosition = interpolate(finalBoneTransformsA[boneIds[i]], finalBoneTransformsB[boneIds[i]], animationBlend) * vec4(aPos, 1.0);
          totalPosition += localPosition * weights[i];
          vec3 localNormal = mat3(finalBoneTransformsA[boneIds[i]]) * Normal;
      }
      gl_Position = projection * view * model * totalPosition;
    }else if(wind){
      vec3 p = aPos;
      p.x += windAmplitude*sin(t+p.x+p.z)*p.y*cos(t-p.x+p.z+0.1f)*p.y;
      p.z += windAmplitude*cos(t+p.x+p.z)*p.y*sin(t-p.x+p.z+0.1f)*p.y;

      gl_Position = projection * view * model * vec4(p, 1.0);
    }else{
      gl_Position = projection * view * model * vec4(aPos, 1.0);
    }
}

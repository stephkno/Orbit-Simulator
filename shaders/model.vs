#version 330

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 uv;
layout (location = 3) in vec3 color;
layout (location = 4) in vec3 tangent;
layout (location = 5) in vec3 bitangent;
layout (location = 6) in ivec4 boneIds;
layout (location = 7) in vec4 weights;

out VS_OUT {
    vec3 FragPos;
    vec2 TexCoords;
    vec3 TangentLightPos;
    vec3 TangentViewPos;
    vec3 TangentFragPos;
    vec3 Normal;
    vec3 wNorm;
    vec4 Position;
    vec4 FragPosLightSpace;
} vs_out;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform mat4 lightSpaceMatrix;

uniform vec3 lightDirection = vec3(0,0,0);
uniform vec3 viewPos = vec3(0,0,0);

const int MAX_BONES = 100;
const int MAX_BONE_INFLUENCE = 4;
uniform mat4 bones[MAX_BONES];
uniform mat4 bonesb[MAX_BONES];

uniform bool animate;
uniform float animationBlend;

float z =  0.01;

mat4 interpolate(mat4 a, mat4 b, float t){
  return ( a * (1.0-t) ) + (b * t);
}

void main()
{

    //vs_out.Position = vec4(position, 1);
    vs_out.FragPos = vec3(model * vec4(position, 1.0));
    vs_out.TexCoords = uv;

    mat3 normalMatrix = transpose(inverse(mat3(model)));
    vec3 T = normalize(normalMatrix * tangent);
    vec3 N = normalize(normalMatrix * normal);
    T = normalize(T - dot(T, N) * N);
    vec3 B = cross(N, T);

    mat3 TBN = transpose(mat3(T, B, N));
    vs_out.TangentLightPos = TBN * lightDirection;
    vs_out.TangentViewPos  = TBN * viewPos;
    vs_out.TangentFragPos  = TBN * vs_out.FragPos;

    vs_out.Normal = normal;
    vs_out.wNorm = normalize(normalMatrix * normal);

    vs_out.FragPosLightSpace = lightSpaceMatrix  * vec4(position, 1.0);

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
          vec4 localPosition = interpolate(bones[boneIds[i]], bonesb[boneIds[i]], animationBlend) * vec4(position, 1.0f);
          totalPosition += localPosition * weights[i];
          vec3 localNormal = mat3(bones[boneIds[i]]) * normal;
      }
      vs_out.Position = projection * view * model * totalPosition;
      gl_Position = vs_out.Position;
    }else{
      vs_out.Position = projection * view * model * vec4(position,1);
      gl_Position = vs_out.Position;
    }

}

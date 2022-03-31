#version 330

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 UV;
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
    vec4 FragPosLightSpace;
} vs_out;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

const int MAX_BONES = 100;
const int MAX_JOINTS = 100;
const int MAX_BONE_INFLUENCE = 4;
uniform mat4 finalBoneMatrices[MAX_BONES];

uniform vec3 lightDirection = vec3(0,0,0);
uniform vec3 viewPos = vec3(0,0,0);

out vec2 TexCoords;

void main()
{
  mat3 normalMatrix = transpose(inverse(mat3(model)));

  vec3 T = normalize(normalMatrix * tangent);
  vec3 N = normalize(normalMatrix * normal);
  T = normalize(T - dot(T, N) * N);
  vec3 B = cross(N, T);

  mat3 TBN = transpose(mat3(T, B, N));

  vs_out.FragPos = vec3(model * vec4(position, 1.0));
  vs_out.TexCoords = UV;
  vs_out.TangentLightPos = TBN * lightDirection;
  vs_out.TangentViewPos  = TBN * viewPos;
  vs_out.TangentFragPos  = TBN * vs_out.FragPos;
  vs_out.Normal = normal;
  vs_out.FragPosLightSpace = mat4(1)  * vec4(position, 1.0);

  vec4 totalPosition = vec4(0.0f);
  for(int i = 0 ; i < MAX_BONE_INFLUENCE ; i++)
  {
      if(boneIds[i] == -1)
          continue;
      if(boneIds[i] >= MAX_JOINTS)
      {
          totalPosition = vec4(position,1.0f);
          break;
      }
      vec4 localPosition = finalBoneMatrices[boneIds[i]] * vec4(position,1.0f);
      totalPosition += localPosition * weights[i];
      vec3 localNormal = mat3(finalBoneMatrices[boneIds[i]]) * normal;
   }

    mat4 viewModel = view * model;
    gl_Position =  projection * viewModel * totalPosition;
	TexCoords = UV;
}

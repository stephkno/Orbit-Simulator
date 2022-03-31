#version 330
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 uv;
layout (location = 3) in vec3 color;
layout (location = 4) in vec3 tangent;
layout (location = 5) in vec3 bitangent;

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
uniform mat4 lightSpaceMatrix;

uniform vec3 lightDirection = vec3(0,0,0);
uniform vec3 viewPos = vec3(0,0,0);

void main()
{
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

    vs_out.FragPosLightSpace = lightSpaceMatrix  * vec4(position, 1.0);

    gl_Position = projection * view * model  * vec4(position, 1.0);
}

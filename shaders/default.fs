#version 330 core

in vec2 TexCoords;
in vec3 FragPos;
in vec3 Normal;
in vec4 FragPosLightSpace;
in vec3 vectorPos;
in vec3 wNorm;
in vec3 TangentLightPos;
in vec3 TangentViewPos;
in vec3 TangentFragPos;
in vec3 color;

struct Material{
  sampler2D diffuseTexture;
  sampler2D specularTexture;
  sampler2D normalTexture;
  sampler2D alphaTexture;
  sampler2D metalicTexture;
  sampler2D emissiveTexture;
  vec3 baseColor;
  vec3 specular;
  vec3 emissive;
  vec3 brightness;
  float normalScale;
  float shininess;
};

struct Sun{
  vec3 position;
  vec3 color;
  vec3 ambient;
};

out vec4 FragColor;

uniform Material material[15];
uniform Sun sun;
uniform sampler2D texture_shadow1;

uniform vec3 viewPos;
uniform float far_plane;
uniform float near_plane;
uniform vec2 textureScale = vec2(1);
uniform bool texEnable = true;
uniform float normal_intensity = 0.0f;
uniform int nummaterial = 1;
uniform bool lit = true;
uniform bool selected;

/*

// array of offset direction for sampling
vec3 gridSamplingDisk[20] = vec3[]
(
   vec3(1, 1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1, 1,  1),
   vec3(1, 1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1, 1, -1),
   vec3(1, 1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1, 1,  0),
   vec3(1, 0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1, 0, -1),
   vec3(0, 1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0, 1, -1)
);

float ShadowCalculation(vec4 fragPosLightSpace)
{
    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(texture_shadow1, projCoords.xy).r;
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    // calculate bias (based on depth map resolution and slope)
    vec3 normal = normalize(Normal);
    vec3 lightDir = normalize(-sunPos);// - FragPos);
    float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);
    // check whether current frag pos is in shadow
    // float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;
    // PCF
    float shadow = 0.0;
    vec2 texelSize = 1.0 / materialize(texture_shadow1, 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(texture_shadow1, projCoords.xy + vec2(x, y) * texelSize).r;
            shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;
        }
    }
    shadow /= 9.0;

    // keep the shadow at 0.0 when outside the far_plane region of the light's frustum.
    if(projCoords.z > 1.0){
      shadow = 0.0;
    }
    return shadow;
}*/

float lerp(float a, float b, float c){
  return (a*(1-c)+(b*c));
}

vec3 lerp(vec3 a, vec3 b, float c){
  vec3 d;
  d.x = lerp(a.x,b.x,c);
  d.y = lerp(a.y,b.y,c);
  d.z = lerp(a.z,b.z,c);
  return d;
}

vec3 SpecularColor(int i){
  vec3 viewDir = normalize(viewPos - FragPos);
  vec3 reflectDir = reflect(-sun.position, wNorm);
  float spec = pow(max(dot(viewDir, reflectDir), 0.0), material[i].shininess);
  return material[i].specular * spec * texture(material[i].specularTexture, TexCoords).rgb;
}

vec3 SunLight(vec3 n){
  vec3 lightDir = normalize(-sun.position);//s - FragPos);
  return sun.ambient + max(dot(n, lightDir), 0.0) * sun.color;
}

void main()
{
  // obtain normal from normal map in range [z0,1]
  vec3 d0 = texture(material[0].diffuseTexture, TexCoords).rgb;

  if(selected){
    d0 = d0 * vec3(0.25, 0, 0);
  }
  FragColor = vec4(d0,1);
}

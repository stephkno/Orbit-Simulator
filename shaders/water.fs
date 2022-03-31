#version 330

out vec4 FragColor;

in VS_OUT {
    vec3 FragPos;
    vec3 VertPos;
    vec2 TexCoords;
    vec2 TexCoordsB;
    vec3 TangentLightPos;
    vec3 TangentViewPos;
    vec3 TangentFragPos;
    vec3 Normal;
    vec4 FragPosLightSpace;
    float T;
} fs_in;

uniform sampler2D texture_diffuse1;     //0
uniform sampler2D texture_specular1;    //1
uniform sampler2D texture_normal1;      //2
uniform sampler2D texture_height1;      //3
uniform sampler2D texture_emission1;    //4
uniform sampler2D texture_alpha1;       //5
uniform sampler2D texture_shadow1;      //6
uniform sampler2D texture_shadow2;      //7

bool diff;
bool spec;
bool norm;
bool emis;

uniform vec3 lightPos = vec3(-0.2, -0.1, -0.4);
uniform vec3 lightDirection;
uniform vec3 viewPos = vec3(0,0,0);
uniform vec3 lightColor = vec3(0.1,0.1,0.1);
uniform vec3 baseColor = vec3(0.1,0.1,0.1);
uniform vec3 specularColor = vec3(0.1,0.1,1);

uniform float brightness = 1.0;
uniform float specPow = 10.0;
uniform float specularV = 1.0;
uniform float shiny = 5.0;
uniform float shadowValue = 85.0;
// specular power
// norm
uniform float norma = 1.4;
uniform float normb = 1.0;
uniform vec3 ambient = vec3(0,0,0);

uniform vec2 texScale = vec2(1,1);

uniform float a;
uniform float minb = 0.0004f;
uniform float maxb = 0.0f;// = 0.001f;
uniform float far_plane;

uniform float biasa = 0.0f;

float DirectionalShadowCalculation(sampler2D shadowmap, vec4 FragPosLightSpace)
{
    // perform perspective divide
    vec3 projCoords = fs_in.FragPosLightSpace.xyz / fs_in.FragPosLightSpace.w;

    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;

    // get closest depth value from light's perspective (using [0,1] range fs_in.FragPosLight as coords)
    float closestDepth = texture(shadowmap, projCoords.xy).r;

    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;

    // calculate bias (based on depth map resolution and slope)
    vec3 normal = normalize(fs_in.Normal);
    vec3 lightDir = normalize(lightPos - fs_in.FragPos);
    float bias = max(minb * (1.0 - dot(normal, lightDir)), maxb);

    // check whether current frag pos is in shadow
    //float shadow = (curentDepth - bias > closestDepth) ? 1.0 : 0.0;
    float shadow = 0.0f;
    int n = 2;
    vec2 texelSize = vec2(1.0 / 2048.0, 1.0 / 2048.0);

    for(int x = -1; x <= n; ++x)
    {
        for(int y = -1; y <= n; ++y)
        {
            float pcfDepth = texture(shadowmap, projCoords.xy + vec2(x, y) * texelSize).r;
            shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;
        }
    }
    shadow /= 20.0;

    // keep the shadow at 0.0 when outside the far_plane region of the light's frustum.
    if(projCoords.z > 1.0){
      shadow = 0.0;
    }
    return shadow;
}

vec2 Directional(vec3 lightDir, vec3 viewDir, vec3 normal, vec3 phongSpecularCol, float phongShininess, vec3 lightColor) {
  vec3 normalmap = texture(texture_normal1, fs_in.TexCoords).rgb;
  // transform normal vector to range [-1,1]
  normalmap = normalize(normalmap * norma - normb);  // this normal is in tangent space
  float diff = max(dot(lightPos-fs_in.FragPos, normalmap), 0.0);
  vec3 halfDir = normalize(viewDir + lightDir);
  float specDot = max(dot(halfDir, normal), 0.0);
  float specular = pow(specDot, phongShininess);
  return vec2(diff, specular);
}

float ShadowCalculation()
{
    // perform perspective divide
    vec3 projCoords = fs_in.FragPosLightSpace.xyz / fs_in.FragPosLightSpace.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    // get closest depth value from light's perspective (using [0,1] range fs_in.FragPosLight as coords)
    float closestDepth = texture(texture_shadow1, projCoords.xy).r;
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    // calculate bias (based on depth map resolution and slope)
    vec3 normal = normalize(fs_in.Normal);
    vec3 lightDir = normalize(lightDirection - fs_in.FragPos);
    float bias = max(maxb * (1.0 - dot(normal, lightDir)), minb);
    // check whether current frag pos is in shadow
    // float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;
    // PCF
    float shadow = 0.0;
    int n = 1;

    vec2 texelSize = vec2(1.0 / 2048.0, 1.0 / 2048.0);

    for(int x = -1; x <= n; ++x)
    {
        for(int y = -1; y <= n; ++y)
        {
            float pcfDepth = texture(texture_shadow1, projCoords.xy + vec2(x, y) * texelSize).r;
            shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;
        }
    }

    shadow /= (100.0-shadowValue);

    // keep the shadow at 0.0 when outside the far_plane region of the light's frustum.
    if(projCoords.z > 1.0){
      shadow = 0.0;
    }
    return shadow;
}

void main() {

  vec3 lightDir = normalize(lightDirection - fs_in.FragPos);
  vec3 viewDir = normalize(viewPos);
  vec3 n = normalize(fs_in.Normal);
  vec3 diff = texture(texture_specular1, fs_in.TexCoords).rrr;
  vec3 diff2 = texture(texture_specular1, fs_in.TexCoordsB*2.0).rrr;

  float illuminance = max(dot(lightDir, n), 0.5);
  vec3 luminance = vec3(0);

  vec2 d = Directional(lightDir, viewDir, n, specularColor.rgb, shiny, lightColor.rgb);
  float sun = d.x;
  float specular = d.y;

  //vec3 point = PointLight();
  float shadow = ShadowCalculation();

  vec3 diff3 = (diff + diff2) * specular;

  luminance += ( baseColor * sun ) + diff3;
  //luminance *= (1.0f - shadow);

  FragColor = vec4(luminance, 0.5);

}

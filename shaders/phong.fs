#version 330
out vec4 FragColor;

in vec2 uv;
in vec3 fn;
in vec3 vertPos;
in vec3 FragPos;
in vec4 FragPosLightSpace;

uniform int mode;
uniform vec3 lightPos;
uniform vec3 lightDirection;
uniform vec3 viewPos;
uniform float opacity = 1.0;
uniform float brightness = 1.0;

const vec4 ambientColor = vec4(0.25, 0.25, 0.25, 1);
const vec4 diffuseColor = vec4(1, 1, 1, 1);
const vec4 specularColor = vec4(0.25, 0.25, 0.25, 1);
const float shiny = 0.0;

uniform float shadowValue = 90.0;
uniform vec3 lightColor = vec3(1,1,1);

// norm
uniform float norma = -12.0;
uniform float normb = -6.0;

uniform float a;
uniform float minb = 0.00115f;
uniform float maxb = 0.00115f;
uniform float biasa = 0.0f;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;
uniform sampler2D texture_normal1;
uniform sampler2D texture_emission1;
uniform sampler2D texture_alpha1;
uniform sampler2D texture_shadow1;

uniform bool alphaClip = false;

vec3 Directional(vec3 lightDir, vec3 viewDir, vec3 normal, vec3 phongSpecularCol, float phongShininess, vec3 lightColor) {
  vec3 color = vec3(0);

  vec3 normalmap = texture(texture_normal1, uv).rgb;
  // transform normal vector to range [-1,1]
  normalmap = normalize(normalmap * norma - normb);  // this normal is in tangent space
  float light = max(dot(-lightDir, normalmap), 0.0);
  vec3 halfDir = normalize(viewDir + lightDir);
  float specDot = max(dot(halfDir, normal), 0.0);
  float specular = pow(specDot, phongShininess);
  vec3 s = specular * texture(texture_specular1, uv).rgb;
  color += light * brightness;
  return color;
}

float ShadowCalculation()
{
    // perform perspective divide
    vec3 projCoords = FragPosLightSpace.xyz / FragPosLightSpace.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(texture_shadow1, projCoords.xy).r;
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    // calculate bias (based on depth map resolution and slope)
    vec3 normal = normalize(fn);
    vec3 lightDir = normalize(lightPos - FragPos);
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

  if(alphaClip){
    float alpha = texture(texture_alpha1, uv).r;
    if(alpha<0.5){
      discard;
    }
  }

  vec3 lightDir = normalize(lightPos - FragPos);
  vec3 viewDir = normalize(viewPos);
  vec3 n = normalize(fn);
  vec3 diff = texture(texture_diffuse1, uv).rgb;
  vec3 light = Directional(lightDir, viewDir, n, specularColor.rgb, shiny, lightColor.rgb);
  //vec3 point = PointLight();
  float shadow = ShadowCalculation();

  vec3 luminance = diff * light;
//  if(opacity<1.0){
//    FragColor = vec4(diff, opacity);
//  }else{
  FragColor = vec4(luminance, 1.0);
//  }
}

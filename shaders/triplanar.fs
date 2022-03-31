#version 330

out vec4 FragColor;

in VS_OUT {
    vec3 FragPos;
    vec2 TexCoords;
    vec3 TangentLightPos;
    vec3 TangentViewPos;
    vec3 TangentFragPos;
    vec3 Normal;
    vec3 wNorm;
    vec4 Position;
    vec4 FragPosLightSpace;
} fs_in;

uniform sampler2D texture_diffuse1;     //0
uniform sampler2D texture_normal1;      //2
uniform sampler2D texture_diffuse2;     //0
uniform sampler2D texture_normal2;      //2
uniform sampler2D texture_diffuse3;     //0
uniform sampler2D texture_normal3;      //2

uniform sampler2D texture_shadow1;      //6
uniform sampler2D texture_shadow2;      //7

bool diff;
bool spec;
bool norm;
bool emis;

uniform vec3 lightPos;// = vec3(-0.2, -0.1, -0.4);
uniform vec3 lightDirection = vec3(0,0,0);
uniform vec3 viewPos = vec3(0,0,0);
uniform vec3 lightColor = vec3(1,1,1);
uniform vec3 baseColor = vec3(1,1,1);

uniform float brightness = 1.0;
uniform float specPow = 0.0;
uniform float specularV = 100.0;
uniform float shiny = 100.0;
uniform float shadowValue = 85.0;
// specular power
// norm
uniform float norma = -12.0;
uniform float normb = -6.0;
uniform vec3 ambientColor = vec3(1,1,1);

uniform vec2 texScale = vec2(1,1);

uniform float a;
uniform float minb = 0.0004f;
uniform float maxb = 0.0f;// = 0.001f;
uniform float far_plane;

uniform float biasa = 0.0f;

// array of offset direction for sampling
vec3 gridSamplingDisk[20] = vec3[]
(
   vec3(1, 1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1, 1,  1),
   vec3(1, 1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1, 1, -1),
   vec3(1, 1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1, 1,  0),
   vec3(1, 0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1, 0, -1),
   vec3(0, 1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0, 1, -1)
);


float DirectionalShadowCalculation(sampler2D shadowmap, vec4 fragPosLightSpace)
{
    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;

    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;

    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(shadowmap, projCoords.xy).r;

    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;

    // calculate bias (based on depth map resolution and slope)
    vec3 normal = normalize(fs_in.Normal);
    vec3 lightDir = normalize(lightPos);// - fs_in.FragPos);
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

vec3 GetTriplanarTexture(){
  vec3 blending = abs( fs_in.wNorm );
  blending = normalize(max(blending, 0.00001)); // Force weights to sum to 1.0
  float b = (blending.x + blending.y + blending.z);
  blending /= vec3(b, b, b);
  vec3 xaxis = texture2D( texture_diffuse1, fs_in.Position.yz).rgb;
  vec3 yaxis = texture2D( texture_diffuse2, fs_in.Position.xz).rgb;
  vec3 zaxis = texture2D( texture_diffuse3, fs_in.Position.xy).rgb;
  // blend the results of the 3 planar projections.
  vec3 tex = xaxis * blending.x + yaxis * blending.y + zaxis * blending.z;

  return tex;

}

void main()
{
   vec2 UV = fs_in.TexCoords * texScale;
  // obtain normal from normal map in range [0,1]
   vec3 normal = texture(texture_normal1, UV).rgb;
   // transform normal vector to range [-1,1]
   //normal = normalize(normal * norma - normb);  // this normal is in tangent space

   // get diffuse color
   // triplanar
   vec3 color = GetTriplanarTexture();
   //vec3 color = texture(texture_diffuse1, UV).rgb;

   // diffuse
   vec3 light = normalize(lightPos);// - fs_in.FragPos);
   float diff = max(dot(light, normal), 0.0);
   vec3 diffuse = (diff * lightColor);
   // specular
   vec3 viewDir = normalize(fs_in.TangentViewPos - fs_in.TangentFragPos);

   // blinn phong lighting calculation
   vec3 reflectDir = reflect(-light, normal);
   vec3 halfwayDir = normalize(lightDirection + viewDir);

   float spec = pow(max(dot(normal, halfwayDir), 0.0), shiny);

   //  shadowmap calculation
   //vec3 lightCoords = fs_in.FragPosLightSpace.xyz / fs_in.FragPosLightSpace.w;
   //float shadowD = DirectionalShadowCalculation(texture_shadow1, fs_in.FragPosLightSpace);
   //float shadowD1 = DirectionalShadowCalculation(texture_shadow2, fs_in.FragPosLightSpace);

   vec3 specular = spec * lightColor;
   vec3 lighting = color * (diffuse + specular);

   FragColor = vec4(lighting, 1.0);
}

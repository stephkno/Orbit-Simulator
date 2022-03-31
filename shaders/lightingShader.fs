#version 330 core
out vec4 FragColor;
in vec3 FragPos;
in vec2 TexCoords;

layout (location = 0) out vec3 FragOutput;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;
uniform sampler2D gSunLightMap;
uniform sampler2D gPickerMap;
uniform sampler2D gSkyboxMap;

struct Light {
    vec3 Position;
    vec3 Color;
    float Linear;
    float Quadratic;
    float Brightness;
};

struct Sun{
  vec3 position;
  vec3 color;
  vec3 ambient;
  float linear;
  float quadratic;
  float brightness;
};

const int NR_LIGHTS = 5;
uniform Light lights[NR_LIGHTS];
uniform Sun sun;
uniform vec3 viewPos=vec3(0);
uniform float time=0.0f;
uniform bool lit=true;

vec3 PointLight(int i, vec3 FragPos, vec3 Normal, float Specular, vec3 viewDir){
  // diffuse
  vec3 lightDir = normalize(lights[i].Position - FragPos);
  vec3 diffuse = max(dot(Normal, lightDir), 0.0) * lights[i].Color * lights[i].Brightness;
  // specular
  vec3 halfwayDir = normalize(lightDir + viewDir);
  float spec = pow(max(dot(Normal, halfwayDir), 0.0), 1);
  vec3 specular = lights[i].Color * spec * Specular;
  // attenuation
  float d = length(lights[i].Position - FragPos);
  float attenuation = 1.0 / (1.0 + lights[i].Linear * d + lights[i].Quadratic * d * d);
  diffuse *= attenuation;
  specular *= attenuation;
  return diffuse;// + specular;
}


vec3 SunPointLight(vec3 pos, vec3 FragPos, vec3 Normal, float Specular, vec3 viewDir){

  // diffuse
  vec3 lightDir = normalize(pos - FragPos);
  vec3 diffuse = vec3(max(dot(Normal, lightDir), 0.0));
  // specular
  vec3 halfwayDir = normalize(lightDir + viewDir);
  float spec = pow(max(dot(Normal, halfwayDir), 0.0), 1);
  vec3 specular = vec3(spec * Specular);
  // attenuation
  float d = length(pos - FragPos);
  float attenuation = 1.0 / (1.0 + sun.linear * d + sun.quadratic * d * d);
  diffuse *= attenuation;
  specular *= attenuation;
  return diffuse;// + specular;
}


void main()
{
  //float ambient = max(distance(sunPos,FragPos), 0.0)/100;
  // retrieve data from gbuffer
  vec3 FragPos = texture(gPosition, TexCoords).rgb;
  vec3 Normal = texture(gNormal, TexCoords).rgb;
  vec4 SunTexture = texture(gSunLightMap, TexCoords).rgba;
  vec4 Diffuse = texture(gAlbedoSpec, TexCoords).rgba;

  // then calculate lighting as usual
  vec3 viewDir = normalize(viewPos - FragPos);
  vec3 lighting = sun.ambient;// (d * sunBrightness); // hard-coded ambient component

  // specular
  vec3 halfwayDir = normalize(sun.position + viewDir);
  float spec = pow(max(dot(Normal, halfwayDir), 0.0), 32.0);
  vec3 specular = vec3(0);//sunColor * spec * Specular * specular_intensity;

  for(int i = 0; i < 5; i++)
  {
    lighting += PointLight(i, FragPos, Normal, 0.0f, viewDir);
  }

  //lighting += SunPointLight(-sun.position, FragPos, Normal, 0.0f, viewDir);

  SunTexture.rgb;

  if(Diffuse.a == 0.0f){
    discard;
  }
  // lit diffuse
  float brightness = 2.0;
  FragColor = Diffuse*(SunTexture*brightness)+vec4(lighting,1);
  /*
  if(renderMode ==  0){
      FragColor = vec4(c,alpha);
  }
  // unlit diffuse
  else if(renderMode == 1){
    FragColor = vec4(Diffuse.rgb+specular,alpha);
  }
  else if(renderMode == 2){
    FragColor = vec4(1.0f);
  }
  // depth buffer
  else if(renderMode == 3){
    FragColor = vec4(FragPos,alpha);
  }
  // normal buffer
  else if(renderMode == 4){
    FragColor = vec4(Normal,alpha);
  }
*/
  //FragColor = vec4(mousepick,1);
}

#version 330
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 uv;
layout (location = 3) in vec3 color;
layout (location = 4) in vec3 tangent;
layout (location = 5) in vec3 bitangent;

out VS_OUT {
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
} vs_out;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform mat4 lightSpaceMatrix;

uniform vec3 lightDirection = vec3(0,0,0);
uniform vec3 viewPos = vec3(0,0,0);

uniform float t;
uniform float amplitude = 0.05;
uniform float speed = 4;
float texScale = 0.1;

#define M_PI 3.14159265358979323846

float rand(vec2 c){
	return fract(sin(dot(c.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

float noise(vec2 p, float freq ){
	float unit = 2048/freq;
	vec2 ij = floor(p/unit);
	vec2 xy = mod(p,unit)/unit;
	//xy = 3.*xy*xy-2.*xy*xy*xy;
	xy = 0.5 * ( 1.0 - cos( M_PI * xy ) );

	float a = rand((ij+vec2(0.,0.)));
	float b = rand((ij+vec2(1.,0.)));
	float c = rand((ij+vec2(0.,1.)));
	float d = rand((ij+vec2(1.,1.)));
	float x1 = mix(a, b, xy.x);
	float x2 = mix(c, d, xy.x);
	return mix(x1, x2, xy.y);
}

float getHeight(float t, float speed, float x, float y, float amplitude){
  float n = amplitude * noise(vec2(x*10,y*10), 0.1);
  float a = sin(n/10.0);
  float b = cos(n/10.0);
  float f = amplitude * ( sin((t * speed * a) + x) + cos((t * speed * b) + y) );
  return f;
}


void main()
{
    vs_out.FragPos = vec3(model * vec4(position, 1.0));
    vs_out.TexCoords *= texScale;
    vs_out.TexCoords = uv + getHeight(t, speed, position.x, position.y, 4.0*amplitude) * 0.25;
    vs_out.TexCoordsB *= texScale * 0.1;
    vs_out.TexCoordsB = uv + getHeight(t, speed, position.x, position.y, 4.0*amplitude) * 0.5;

    mat3 normalMatrix = transpose(inverse(mat3(model)));
    vec3 T = normalize(normalMatrix * tangent);
    vec3 N = normalize(normalMatrix * normal);
    T = normalize(T - dot(T, N) * N);
    vec3 B = cross(N, T);

    mat3 TBN = transpose(mat3(T, B, N));
    vs_out.TangentLightPos = TBN * lightDirection;
    vs_out.TangentViewPos  = TBN * viewPos;
    vs_out.TangentFragPos  = TBN * vs_out.FragPos;

    vs_out.FragPosLightSpace = lightSpaceMatrix  * vec4(position, 1.0);

    vec3 pos = position;
    pos.y += getHeight(t, speed, position.x, position.z, amplitude);
    //pos.y += getHeight(t, speed*2.0f, position.x, position.y, amplitude);
    //pos.y *= sin((t*speed*3) + position.x+1 + position.z+1);

    float a = getHeight(t, speed, position.x-1, position.z-1, amplitude);
    float b = getHeight(t, speed, position.x+1, position.z-1, amplitude);
    float c = getHeight(t, speed, position.x-1, position.z+1, amplitude);
    float d = getHeight(t, speed, position.x+1, position.z+1, amplitude);

    vs_out.T = t;
    vec3 n = vec3(a-b, c-d, 2.0);
    vs_out.Normal = normalize(n);

    vs_out.VertPos = pos;
    gl_Position = projection * view * model  * vec4(pos, 1.0);
}

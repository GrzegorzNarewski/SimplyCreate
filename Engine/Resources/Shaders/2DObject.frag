#version 450

layout(location = 0) in vec2 TexCoord;
layout(location = 1) in vec3 Normal;
layout(location = 2) in vec3 FragPos;

layout(set = 0, binding = 3) uniform sampler2D texSampler;
layout(set = 0, binding = 0) uniform UniformBufferObject {
    mat4 model;
    vec4 color;
};

layout(set = 0, binding = 2) uniform Light {
    vec4 lightColor[128];
    vec4 lightPos[128];
    vec4 lightdetails[128];
};

layout(location = 0) out vec4 outColor;

vec3 calcLight(vec4 diffuseTex, vec3 posl, vec3 colorl, vec3 details);

vec3 viewPos = vec3(9.0f, 0.0f, 0.0f);

void main()
{
    vec4 diffuseTex = texture(texSampler, TexCoord);
    if(diffuseTex.a <= 0.7f) discard;
    else diffuseTex *= color;

    vec3 result = vec3(0.0f);
    for(int i=0; i<128; i++) {
        if(lightPos[i].a == 1.0f) result += calcLight(diffuseTex, lightPos[i].xyz, lightColor[i].rgb, lightdetails[i].rgb);
        else break;
    }

    outColor = vec4(result, diffuseTex.a);
}

vec3 calcLight(vec4 diffuseTex, vec3 posl, vec3 colorl, vec3 details)
{
    // ambient
    vec3 ambient = vec3(0.2f) * (diffuseTex.rgb * colorl);
  	
    // diffuse 
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(posl - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = vec3(1.0f) * diff * (diffuseTex.rgb * colorl);
    
    // specular
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    float spec = pow(max(dot(norm, halfwayDir), 0.0), 64.0);
    vec3 specular = vec3(1.0f) * spec * vec3(1.0f) * colorl;  
    
    // attenuation
    float distance    = length(posl - FragPos);
    float attenuation = 1.0 / (details[0] + details[1] * distance + details[2] * (distance * distance));    

    ambient  *= attenuation;  
    diffuse   *= attenuation;
    specular *= attenuation;   
        
    return ambient + diffuse;
}
#version 450

layout(location = 0) out vec4 FragColor;

layout(location = 0) in vec2 TexCoords;
layout(location = 1) in vec3 Normals;
layout(location = 2) in vec3 FragPos;

layout(set = 0, binding = 2) uniform sampler2D diffuseMap;
layout(set = 0, binding = 3) uniform sampler2D emissionMap;

layout(set = 0, binding = 0) uniform UniformBufferObject {
    mat4 animeModels[25];
};

layout(set = 0, binding = 1) uniform UniformBufferObject2 {
    vec4 light_pos;
    vec4 light_color;
};

layout( push_constant ) uniform ColorBlock {
  mat4 view;
  mat4 proj;
  vec4 camPos;
};

vec4 doPointLight(vec3 pos, vec3 col)
{
    // ambient lighting
    float ambientStrength = 0.45f;
    vec3 ambient = vec3(ambientStrength);

    // diffuse lighting
    vec3 norm = normalize(Normals);
    vec3 lightDir = normalize(pos - FragPos);
    float dif = max(dot(lightDir, norm), 0.0);
    vec3 diffuse = vec3(dif);

     // specular
    vec3 viewDir = normalize(camPos.xyz - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 64.0f);
    vec3 specular = vec3(spec);

     // attenuation
    float distance    = length(pos - FragPos);
    float attenuation = 1.0 / (1.0f + 0.09f * distance + 0.032f * (distance * distance));    

    ambient  *= attenuation;  
    diffuse   *= attenuation;
    specular *= attenuation;  

    return vec4((ambient + diffuse + specular) * col, 1.0f);
}

void main()
{
    // ambient lighting
    float ambientStrength = 0.45f;
    vec3 ambient = vec3(ambientStrength);

    // diffuse lighting
    vec3 norm = normalize(Normals);
    vec3 lightDir = normalize(vec3(0.5f, -0.2f, 0.4f));
    float dif = max(dot(lightDir, norm), 0.0);
    vec3 diffuse = vec3(dif);

    // emission
    const float emissionStrength = 10.0f;
    vec3 emission = texture(emissionMap, TexCoords).xyz * vec3(emissionStrength);

    vec4 lightingResult = vec4(ambient + diffuse + emission, 1.0f);
    lightingResult += doPointLight(light_pos.xyz, light_color.xyz);

    vec4 result = texture(diffuseMap, TexCoords) * lightingResult;
    FragColor = result;
}
#version 450

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 innormal;
layout(location = 2) in vec2 inTexCoord;
layout(location = 3) in vec4 inWeights;
layout(location = 4) in ivec4 inJoints;
layout(location = 5) in vec3 inTangents;

layout(location = 0) out vec2 TexCoord;
layout(location = 1) out vec3 Normal;
layout(location = 2) out vec3 FragPos;

layout(set = 0, binding = 0) uniform UniformBufferObject {
    mat4 animeModels[25];
};

layout( push_constant ) uniform ColorBlock {
  mat4 view;
  mat4 proj;
};

void main() 
{
    vec4 totalPos = vec4(0.0f);
    vec4 totalNor = vec4(0.0f);

    for(int i=0; i<4; i++) {
        if(inJoints[i] >= 0) {
            vec4 posePosition = animeModels[inJoints[i]] * vec4(inPosition, 1.0f);
            totalPos += posePosition * inWeights[i];

            vec4 worldNormal = animeModels[inJoints[i]] * vec4(innormal, 1.0f);
            totalNor += worldNormal;
        }
        else break;
    }
    FragPos = totalPos.xyz;
    gl_Position = proj * view * totalPos;
    Normal = mat3(mat4(1.0f)) * totalNor.xyz;  
    TexCoord = inTexCoord.xy; 
}



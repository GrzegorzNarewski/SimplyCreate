#version 450

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormals;
layout(location = 2) in vec2 inTexCoord;

layout(location = 0) out int context;
layout(location = 1) out vec2 TextCoords;

layout(set = 0, binding = 0) uniform UniformBufferObject {
    mat4 model;
    vec4 color;
    int contexts;
};

layout( push_constant ) uniform PushConstantProj {
  mat4 view; 
  mat4 proj;
} PushConstant;


void main() 
{
    gl_Position = PushConstant.proj * model * vec4(inPosition.x, inPosition.y, inPosition.z, 1.0f);
    TextCoords = inTexCoord;
    context = contexts - 32;
}

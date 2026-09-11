#version 450

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexCoord;

layout(set = 0, binding = 0) uniform UniformBufferObject {
    mat4 model;
    vec4 color;
};

layout(set = 0, binding = 1) uniform UniformBufferObject2 {
    mat4 proj;
    mat4 view;
};

void main() 
{
    gl_Position = proj * view * model * vec4(inPosition, 1.0f);
}



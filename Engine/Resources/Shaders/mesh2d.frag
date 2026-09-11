#version 450
layout(location = 0) out vec4 FragColor;

layout(location = 0) in vec2 TexCoords;

layout(set = 0, binding = 1) uniform sampler2D diffuse;

layout(set = 0, binding = 0) uniform UniformBufferObject {
    mat4 model;
    vec4 color;
};

void main()
{
    vec4 result = texture(diffuse, TexCoords);
    //if(result.a < 0.7f) discard;
    FragColor = vec4(result) * color;
}
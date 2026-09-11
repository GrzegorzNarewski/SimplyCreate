#version 450

layout(location = 0) in vec2 TexCoord;

layout(location = 0) out vec4 outColor;

layout(set = 0, binding = 2) uniform sampler2D texture1;

layout(set = 0, binding = 0) uniform UniformBufferObject {
    mat4 model;
    vec4 color;
    int context;
};

const vec3 Pos = {0.0f, 0.0f, 0.0f};

void main()
{
    outColor = vec4(texture(texture1, TexCoord).xyz, 1.0f);
    if(outColor.x == 0.0f)
        discard;
    outColor = vec4(color);
}

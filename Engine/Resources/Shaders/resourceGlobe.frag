#version 450
layout(location = 0) out vec4 FragColor;

layout(location = 0) in vec2 TexCoords;
layout(location = 1) in vec3 FragPos;

layout(set = 0, binding = 2) uniform sampler2D diffuse;

layout(set = 0, binding = 0) uniform UniformBufferObject {
    mat4 model;
    vec4 color;
};
layout(set = 0, binding = 1) uniform UniformBufferObject2 {
    float progress;
    float progress2;
};

void main()
{
    vec4 result = texture(diffuse, TexCoords);
    if(result.a < 0.9f) discard;

    FragColor.a = 1.0f;
    vec4 color2 = color;
    if(result.xyz != vec3(0.0f, 0.0f, 0.0f)) {
        if((FragPos.y + 1.0f)/2.0f < 1.0f-progress) {
            FragColor.a = 0.0f;
        }
        if((FragPos.y + 1.0f)/2.0f > 1.0f-progress2) {
            color2.xyz += vec3(0.0f, 2.5f, 5.0f) * ((FragPos.x + 1.0f) / 2.5f);
            if(FragColor.a == 0.0f) {
                color2.xyz = vec3(0.0f, 2.5f, 5.0f) * ((FragPos.x + 1.0f) / 2.5f);
                FragColor.a = ((FragPos.x + 1.0f) / 2.5f);
            }
        }
        FragColor.xyz = vec3(result.xyz * color2.xyz);
    }
    else FragColor = result;
}
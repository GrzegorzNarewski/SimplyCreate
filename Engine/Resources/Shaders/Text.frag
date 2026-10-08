#version 450

layout(location = 0) in vec2 TexCoord;

layout(location = 0) out vec4 FragColor;

layout(set = 0, binding = 1) uniform sampler2D image;

layout(set = 0, binding = 0) uniform UniformBufferObject {
    mat4 model;
    vec4 color;
    int context;
    int texturesizex;
    int texturesizey;
    float LetterWidth;
};

void main()
{
	FragColor = texture(image, TexCoord);
    if(FragColor.a < 0.5f) discard;
	FragColor = color;
}

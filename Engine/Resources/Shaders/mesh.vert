#version 450
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

layout(location = 0) out vec2 TexCoords;
layout(location = 1) out vec3 Normals;
layout(location = 2) out vec3 FragPos;

layout(set = 0, binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    vec4 color;
};
 
layout( push_constant ) uniform ColorBlock {
    mat4 proj;
    vec4 camPos;
};

void main()
{
    Normals = transpose(inverse(mat3(model))) * aNormal;
    FragPos = vec3(vec4(model * vec4(aPos, 1.0f)).xyz);
	gl_Position = proj * view * model * vec4(aPos, 1.0f);
    TexCoords = aTexCoord;
}
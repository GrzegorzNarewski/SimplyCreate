#version 450
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormals;
layout (location = 2) in vec2 aTexCoord;

layout(location = 0) out vec2 TexCoords;

layout(set = 0, binding = 0) uniform UniformBufferObject {
    mat4 model;
    vec4 color;
};

layout( push_constant ) uniform PushConstantProj {
  mat4 proj;
};

void main()
{
	  gl_Position = proj * model * vec4(aPos, 1.0f);
    TexCoords = aTexCoord;
}
#version 450

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormals;
layout(location = 2) in vec2 inTexCoord;

layout(location = 0) out vec2 TexCoords;

layout(set = 0, binding = 0) uniform UniformBufferObject {
    mat4 model;
    vec4 color;
    int context;
    int texturesizex;
    int texturesizey;
    float LetterWidth;
};

layout( push_constant ) uniform PushConstantProj {
  mat4 proj2d;
} ;


void main() 
{
	gl_Position = proj2d * model * vec4(inPosition, 1.0);
	TexCoords = inTexCoord;
}

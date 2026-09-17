#version 450

layout( triangles ) in;
layout( triangle_strip, max_vertices = 5 ) out;

layout(set = 0, binding = 0) uniform UniformBufferObject {
    mat4 model;
    vec4 color;
    int context;
    int texturesizex;
    int texturesizey;
    float LetterWidth;
};

layout(location = 0) in vec2 TexCoords[3];

layout(location = 0) out vec2 TexCoord;

void build_letter(vec2 Tex, vec4 position)
{    
	const float lettersizex = 16, lettersizey = 32;
	//float texturesizex = 256, texturesizey = 256;
	const float LetterHeight = 4;

	float avgletterRealSizex = lettersizex / texturesizex, avgletterRealSizey = lettersizey / texturesizey;
	float letterRealSizex = LetterWidth / texturesizex, letterRealSizey = LetterHeight / texturesizey;

	float x=context-32, y=0;
	while(x >= texturesizex/lettersizex) {
		x -= texturesizex/lettersizex;
		y ++;
	}

	float letterPositionx = avgletterRealSizex * x, letterPositiony = avgletterRealSizey * y;

	if(Tex.x == 0.0f && Tex.y == 0.0f) {
		gl_Position = position;
		TexCoord = vec2(letterPositionx                  , letterPositiony + letterRealSizey);
   		EmitVertex(); 
	}
	if(Tex.x == 1.0f && Tex.y == 0.0f) {
		gl_Position = position;
		TexCoord = vec2(letterPositionx + letterRealSizex, letterPositiony + letterRealSizey);
   		EmitVertex(); 
	}
	if(Tex.x == 0.0f && Tex.y == 1.0f) {
		gl_Position = position;
		TexCoord = vec2(letterPositionx                  , letterPositiony + avgletterRealSizey - 0.05f);
   		EmitVertex(); 
	}
	if(Tex.x == 1.0f && Tex.y == 1.0f) {
		gl_Position = position;
		TexCoord = vec2(letterPositionx + letterRealSizex, letterPositiony + avgletterRealSizey- 0.05f);
   		EmitVertex(); 
	}
}

void main() {    
    build_letter(TexCoords[0], gl_in[0].gl_Position);
	build_letter(TexCoords[1], gl_in[1].gl_Position);
	build_letter(TexCoords[2], gl_in[2].gl_Position);
	EndPrimitive();
}

#version 450

layout( triangles ) in;
layout( triangle_strip, max_vertices = 3 ) out;

layout(location = 0) in int context[];
layout(location = 1) in vec2 TextCoords[];

layout(location = 0) out vec2 TexCoords;

vec2 getTextureCoordinate(vec2 origin);

void main() {
    gl_Position = gl_in[0].gl_Position;
    TexCoords = getTextureCoordinate(TextCoords[0]);
    EmitVertex();

    gl_Position = gl_in[1].gl_Position;
    TexCoords = getTextureCoordinate(TextCoords[1]);
    EmitVertex();

    gl_Position = gl_in[2].gl_Position;
    if(TextCoords[2].x == 1.0f && TextCoords[2].y == 1.0f)
    {
        TexCoords = getTextureCoordinate(TextCoords[2]);
    }
    else if(TextCoords[2].x == 0.0f && TextCoords[2].y == 0.0f)
    {
        TexCoords = getTextureCoordinate(TextCoords[2]);
    }
    EmitVertex();

    EndPrimitive();
}

vec2 getTextureCoordinate(vec2 origin)
{
    if(origin.x == 0.0f && origin.y == 0.0f) {
        int xx = context[0];
        int yy = 0;
        while(xx > 17) {
            xx -= 18;
            yy++;
        }
        float x = 2.0f + (14 * xx);
        x /= 256.0f;

        float y = 2.0f + (18 * yy);
        y /= 128.0f;
        return vec2(x, y);
    }
    if(origin.x == 1.0f && origin.y == 0.0f) {
        int xx = context[0];
        int yy = 0;
        while(xx > 17) {
            xx -= 18;
            yy++;
        }
        float x = 12.0f + (14 * xx);
        x /= 256.0f;

        float y = 2.0f + (18 * yy);
        y /= 128.0f;
        return vec2(x, y);
    }
    if(origin.x == 0.0f && origin.y == 1.0f) {
        int xx = context[0];
        int yy = 0;
        while(xx > 17) {
            xx -= 18;
            yy++;
        }
        float x = 2.0f + (14 * xx);
        x /= 256.0f;

        float y = 16.0f + (18 * yy);
        y /= 128.0f;
        return vec2(x, y);
    }
    if(origin.x == 1.0f && origin.y == 1.0f) {
        int xx = context[0];
        int yy = 0;
        while(xx > 17) {
            xx -= 18;
            yy++;
        }
        float x = 12.0f + (14 * xx);
        x /= 256.0f;

        float y = 16.0f + (18 * yy);
        y /= 128.0f;
        return vec2(x, y);
    }
}

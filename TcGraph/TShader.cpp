#include "TShader.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

const char* const freeTypeVertex = "#version 330 core\
layout(location = 0) in vec4 vertex; // <vec2 pos, vec2 tex>\
out vec2 TexCoords;\
\
uniform mat4 projection;\
\
void main()\
{\
    gl_Position = projection * vec4(vertex.xy, 0.0, 1.0);\
    TexCoords = vertex.zw;\
}";

const char* const freeTypeFragment = "#version 330 core \
in vec2 TexCoords; \
out vec4 color;\
\
uniform sampler2D text;\
uniform vec4 textColor;\
\
void main()\
{\
    vec4 sampled = vec4(1.0, 1.0, 1.0, texture(text, TexCoords).r);\
    color = vec4(textColor) * sampled;\
}";

const char* const textureVertex = "#version 330 core\nlayout(location = 0) in vec3 aPos;\nlayout(location = 1) in vec4 aColor;\nlayout(location = 2) in vec2 aTexCoord;\n\nout vec4 ourColor;\nout vec2 TexCoord;\n\n void main()\n{\n    gl_Position = vec4(aPos, 1.0);\n    ourColor = aColor;\n    TexCoord = aTexCoord;\n}";
const char* const textureFragment = "#version 330 core\nout vec4 FragColor;\n\nin vec4 ourColor;\nin vec2 TexCoord;\n\nuniform sampler2D ourTexture;\n\nvoid main()\n{\n    FragColor = texture(ourTexture, TexCoord);\n}";


TrecPointer<TShader> TShader::GenerateShader(TrecPointer<TFileShell> )
{
    return TrecPointer<TShader>();
}

TShader::~TShader()
{
    if (shaderId > 0)
        glDeleteProgram(shaderId);
}

bool TShader::Use()const 
{
    if(shaderId < 1)
        return false;
    glUseProgram(shaderId);
    return true;
}

int TShader::GetShaderId()
{
    return shaderId;
}

TFreeTypeShader::TFreeTypeShader()
{
    unsigned int vertex = glCreateShader(GL_VERTEX_SHADER); 
    unsigned int fragment = glCreateShader(GL_FRAGMENT_SHADER);

    // Create Vertex Shader
    glShaderSource(vertex, 1, &freeTypeVertex, 0);
    glCompileShader(vertex);

    // Create Fragment Shader
    glShaderSource(fragment, 1, &freeTypeFragment, 0);
    glCompileShader(fragment);

    shaderId = glCreateProgram();
    glAttachShader(shaderId, vertex);
    glAttachShader(shaderId, fragment);

    glLinkProgram(shaderId);

    glDeleteShader(vertex);
    glDeleteShader(fragment);


}

Texture2DShader::Texture2DShader()
{
    unsigned int vertex = glCreateShader(GL_VERTEX_SHADER);
    unsigned int fragment = glCreateShader(GL_FRAGMENT_SHADER);

    // Create Vertex Shader
    glShaderSource(vertex, 1, &textureVertex, 0);
    glCompileShader(vertex);

    // Create Fragment Shader
    glShaderSource(fragment, 1, &textureFragment, 0);
    glCompileShader(fragment);

    shaderId = glCreateProgram();
    glAttachShader(shaderId, vertex);
    glAttachShader(shaderId, fragment);

    glLinkProgram(shaderId);

    glDeleteShader(vertex);
    glDeleteShader(fragment);
}
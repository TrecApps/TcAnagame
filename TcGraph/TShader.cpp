#include "TShader.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

const char* const freeTypeVertex = "#version 330 core \nlayout(location = 0) in vec4 vertex; // <vec2 pos, vec2 tex>\nout vec2 TexCoords;\n\nuniform mat4 projection;\n\nvoid main()\n{\n    gl_Position = projection * vec4(vertex.xy, 0.0, 1.0);\n    TexCoords = vertex.zw;\n}";

const char* const freeTypeFragment = "#version 330 core\nlayout(location = 0) in vec4 vertex; // <vec2 pos, vec2 tex>\nout vec2 TexCoords;\n\nuniform mat4 projection;\n\nvoid main()\n{\n    gl_Position = projection * vec4(vertex.xy, 0.0, 1.0);\n    TexCoords = vertex.zw;\n}";

const char* const textureVertex = "#version 330 core\nlayout(location = 0) in vec3 aPos;\nlayout(location = 1) in vec3 aColor;\nlayout(location = 2) in vec2 aTexCoord;\n\nout vec3 ourColor;\nout vec2 TexCoord;\n\n void main()\n{\n    gl_Position = vec4(aPos, 1.0);\n    ourColor = aColor;\n    TexCoord = aTexCoord;\n}";
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

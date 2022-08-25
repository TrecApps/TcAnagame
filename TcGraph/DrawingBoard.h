#pragma once

#include "TBrush.h"
#include "TShader.h"

using shader_type = enum class shader_type {
    shader_2d,
    shader_write,
    shader_3d
};

class _TC_GRAPH DrawingBoard :
    public TVObject
{
    GLFWwindow* window;
    TrecPointerSoft<DrawingBoard> self;
    TColor defaultClearColor;

    shader_type shaderType;
    TrecPointer<TShader> currentShader;

    TrecPointer<TShader> shader2D;
    TrecPointer<TShader> shaderWrite;

protected:
    
    explicit DrawingBoard(GLFWwindow* window);


    
public:
    bool GetDisplayResolution(int& width, int& height);

    ~DrawingBoard() override;

    void BeginDraw() const;
    void ConfirmDraw();

    void SetShader(TrecPointer<TShader> shader, shader_type shaderType = shader_type::shader_3d);
    TrecPointer<TShader> GenerateShader(TrecPointer<TFileShell> shaderFile);

    TrecPointer<TBrush> GetSolidColorBrush(const TColor& color) const;
};


#pragma once

#include "TBrush.h"
#include "TShader.h"
#include <TPoint.h>
#include "GraphicsDef.h"
#include <map>
#include <glm/gtc/matrix_transform.hpp>
#include <TcRunner.h>
#include <TTextIntercepter.h>
#include <TDataMap.h>

class CharWithSize
{
public:
    WCHAR ch;
    int weight;
    CharWithSize() = default;
    CharWithSize(const CharWithSize& copy) = default;
    CharWithSize(WCHAR ch, int weight);

    int compare(const CharWithSize& other) const;

    bool operator<(const CharWithSize& other) const;
};

using shader_type = enum class shader_type {
    shader_2d,
    shader_write,
    shader_texture,
    shader_3d
};

using animation_mode = enum class animation_mode
{
    run_once,   // Only run the animation once
    forward_reverse_once,   // runs the animations forward once and reverse once
    forward_loop,           // Runs the animations forward in a loop
    forward_reverse_loop,   // Runs the animations forward and backwards over and over again
};

class Animation : public TCoreObject
{
    friend class StoryBoard;
protected:

    UINT millisecondRefresh;

    UINT millisecondLength;

    bool reverse;
public:
    Animation() = default;
    UINT GetMillisecondRefresh();
    UINT GetMillisecondLength();
    void SetMillisecondRefresh(UINT);
    void SetMillisecondLength(UINT);
    bool DoReverse();
    void DoReverse(bool reverse);
    virtual bool Update(float progress) = 0;
};

typedef struct AnagameCaret
{
    TrecPointer<TTextIntercepter> intercepter;
    TPoint top, bottom;
    TrecPointer<TBrush> brush;
    float thickness;
    bool OnDraw;
};

using ag_mouse_pointer = enum class ag_mouse_pointer
{
    standard,
    input,
    crosshair,
    hand,
    h_arrows,
    v_arrows
};

class _TC_GRAPH DrawingBoard :
    public TVObject
{
    friend class CaretRunner;

    TDataArray<GLFWcursor*> cursors;


    TColor defaultClearColor;

    shader_type shaderType;
    TrecPointer<TShader> currentShader;

    TrecPointer<TShader> shader2D;
    TrecPointer<TShader> shaderWrite;
    TrecPointer<TShader> shaderTex2D;

    TrecPointer<TBrush> textHighlight;


    

    void InitializeCaretRunner();

    void DrawCaret();

protected:
    TrecPointer<TcAsyncRunner> caretRunner;
    TrecPointerSoft<DrawingBoard> self;

    TDataMap<std::map<CharWithSize, GLuint>> charMap;

    glm::mat4 orthoProjection; 

    AnagameCaret caret;
    GLFWwindow* window;
    explicit DrawingBoard(GLFWwindow* window);


    RECT_F origArea;
    RECT_F area;

    TDataArray<RECT_F> layers;

    ThreadBlocker drawingThread;
    
    bool needsRefresh, needsConstantRefresh;

    void ResetProjection();

    UINT tabSpace; // How many spaces a tab represents

public:

    virtual void SetFocusObject(TrecPointer<TObject> focusObject);

    UINT GetTabSpace();
    bool RetrieveFontEntry(const TString& name, std::map<CharWithSize, GLuint>& map);
    void SetFontEntry(const TString& name, const std::map<CharWithSize, GLuint>& map);

    void PrepRefresh();
    void SetCursor(ag_mouse_pointer mPointer);

    void NormalizePoint(const TPoint& input, TPoint& output);

    void LockDrawing();
    void UnlockDrawing();

    void SetCaret(TrecPointer<TTextIntercepter> texter, const TPoint& top, const TPoint& bottom, float thickness = 0.0f);
    void SetCaret(TrecPointer<TTextIntercepter> texter, const TPoint& top, const TPoint& bottom, const TColor& color, float thickness = 0.0f);
    void ShowCaret();
    void PauseCaret();

    void AddLayer(const RECT_F& ref);

    void PopLayer();

    GLuint GetTextureShaderId();

    void SetSelf(TrecPointer<DrawingBoard> s);

    RECT_F GetArea();
    RECT_F GetOrigArea();

    GLFWwindow* GetGlfwWindow();

    TrecPointer<TBrush> GetHighlightBrush();

    static bool IsContained(const TPoint& point, const RECT_F& loc);
    static bool IsContained(const TPoint& cp, const ELLIPSE_F& el);
    bool GetDisplayResolution(int& width, int& height);

    ~DrawingBoard() override;

    void BeginDraw() const;
    void ConfirmDraw();

    GLint SetShader(TrecPointer<TShader> shader, shader_type shaderType = shader_type::shader_3d);
    TrecPointer<TShader> GenerateShader(TrecPointer<TFileShell> shaderFile);

    TrecPointer<TBrush> GetSolidColorBrush(const TColor& color) const;
    TrecPointer<TBrush> GetImageBrush(TrecPointer<TFileShell> file) const;

    TrecPointer<TBrush> GetImageBrush(UCHAR* data, UINT width, UINT height, GLenum unum = GL_RGBA)const;
};


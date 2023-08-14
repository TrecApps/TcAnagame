#include "TColorBrush.h"

TString solidShader(GetDirectoryWithSlash(CentralDirectories::cd_Executable) + L"Shaders/Single_Color_2d.json");
TString vertexShader(GetDirectoryWithSlash(CentralDirectories::cd_Executable) + L"Shaders/Vertex_Color_2d.json");

bool TColorBrush::NormalizeRect(RECT_F& output, const RECT_F& input, TrecPointer<DrawingBoard> board)
{
    if(!board.Get())
        return false;

    auto r = board->GetArea();
    r.bottom /= 2.0f;
    r.right /= 2.0f;

    output.right = (input.right / r.right) - 1.0f;
    output.left = (input.left / r.right) - 1.0f;
    output.bottom = -((input.bottom / r.bottom) - 1.0f);
    output.top = -((input.top / r.bottom) - 1.0f);

    return true;
}

void TColorBrush::PrepShader()
{
    if (colors.Size() == 1)
    {
        auto file = TFileShell::GetFileInfo(solidShader);
        TrecPointerKey::TrecFromSoft<>(window)->PrepVulkanShader(file);
    }
    else {
        
        auto file = TFileShell::GetFileInfo(vertexShader);
        TrecPointerKey::TrecFromSoft<>(window)->PrepVulkanShader(file);
    }
}

TrecPointer<DrawingBoard> TColorBrush::GetDrawingBoard()
{
    return TrecPointerKey::TrecFromSoft<>(this->window);
}

brush_type TColorBrush::GetBrushType()
{
    return brushType;
}

UINT TColorBrush::GetMaxColors()
{
    return colors.Size();
}

void TColorBrush::SetColor(const TColor& color, UINT index)
{
    if (index >= colors.Size())
        return;
    colors[index] = color;
}

TColor TColorBrush::GetColor(UINT index)
{
    if (index >= colors.Size())
        return TColor();

    return colors[index];
}

void TColorBrush::DrawRectangle(const RECT_F& r, float thickness)
{
    TrecPointer<DrawingBoard> board = TrecPointerKey::TrecFromSoft<>(window);
    if (!board.Get())
        return;
   
    PrepShader();
    glBegin(GL_LINES);

    glLineWidth(thickness);

    TColor color;

   
    if(brushType == brush_type::brush_type_solid)
    {
        color.SetColor(this->GetColor());

        glColor4f(color.GetRed(), color.GetGreen(), color.GetBlue(), color.GetOpacity());

        glVertex2d(r.left, r.top);
        glVertex2d(r.right, r.top);

        glVertex2d(r.right, r.top);
        glVertex2d(r.right, r.bottom);

        glVertex2d(r.right, r.bottom);
        glVertex2d(r.left, r.bottom);

        glVertex2d(r.left, r.bottom);
        glVertex2d(r.left, r.top);
    }


    glEnd();
}

void TColorBrush::FillRectangle(const RECT_F& r)
{    TrecPointer<DrawingBoard> board = TrecPointerKey::TrecFromSoft<>(window);
    if (!board.Get())
        return;
    board->SetShader(TrecPointer<TShader>(), shader_type::shader_2d);
    TColor color;
    if (brushType == brush_type::brush_type_solid)
    {
        glBegin(GL_QUADS);
        color.SetColor(this->GetColor());

        glColor4f(color.GetRed(), color.GetGreen(), color.GetBlue(), color.GetOpacity());
        glVertex2d(r.left, r.top);
        glVertex2d(r.right, r.top);
        glVertex2d(r.right, r.bottom);
        glVertex2d(r.left, r.bottom);

        glEnd();
    }
}

void TColorBrush::DrawRoundedRectangle(const ROUNDED_RECT_F& , float )
{
}

void TColorBrush::FillRoundedRectangle(const ROUNDED_RECT_F& r)
{
}

void TColorBrush::DrawEllipse(const ELLIPSE_F& , float )
{
}

void TColorBrush::FillEllipse(const ELLIPSE_F& r)
{
}

void TColorBrush::DrawGeometry(const TDataArray<TPoint>& points, float thickness)
{
    if (points.Size() < 3)
    {
        if (points.Size() == 2)
            DrawLine(points[0], points[1]);
        return;
    }
    TrecPointer<DrawingBoard> board = TrecPointerKey::TrecFromSoft<>(window);
    if (!board.Get())
        return;
    board->SetShader(TrecPointer<TShader>(), shader_type::shader_2d);

    glBegin(GL_LINE_STRIP);
    TColor color(GetColor());
    glLineWidth(thickness);
    glColor4f(color.GetRed(), color.GetGreen(), color.GetBlue(), color.GetOpacity());

    for (UINT Rust = 1; Rust < points.Size(); Rust++)
    {
        glVertex2d(points[Rust - 1].x, points[Rust - 1].y);
        glVertex2d(points[Rust].x, points[Rust].y);
    }

    glEnd();
    DrawLine(points[0], points[points.Size() - 1], thickness);
}

void TColorBrush::FillGeometry(const TDataArray<TPoint>& points)
{
    if (points.Size() < 3)
    {
        if(points.Size() == 2)
            DrawLine(points[0], points[1]);
        return;
    }
    TrecPointer<DrawingBoard> board = TrecPointerKey::TrecFromSoft<>(window);
    if (!board.Get())
        return;
    board->SetShader(TrecPointer<TShader>(), shader_type::shader_2d);

    glBegin(GL_POLYGON);
    TColor color(GetColor());
    glColor4f(color.GetRed(), color.GetGreen(), color.GetBlue(), color.GetOpacity());

    for (UINT Rust = 0; Rust < points.Size(); Rust++)
    {
        glVertex2d(points[Rust].x, points[Rust].y);
    }

    glEnd();
}

void TColorBrush::DrawLine(const TPoint& p1, const TPoint& p2, float thickness)
{
    TrecPointer<DrawingBoard> board = TrecPointerKey::TrecFromSoft<>(window);
    if (!board.Get())
        return;
    board->SetShader(TrecPointer<TShader>(), shader_type::shader_2d);
    TColor color(GetColor());

    glBegin(GL_LINES);

    glLineWidth(thickness);

    glColor4f(color.GetRed(), color.GetGreen(), color.GetBlue(), color.GetOpacity());
    glVertex2d(p1.x, p1.y);
    glVertex2d(p2.x, p2.y);

    glEnd();
}

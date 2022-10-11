#include "TColorBrush.h"

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
    board->SetShader(TrecPointer<TShader>(), shader_type::shader_2d);
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

void TColorBrush::DrawGeometry(const TDataArray<VERTEX_2D>& , float )
{
}

void TColorBrush::FillGeometry(const TDataArray<VERTEX_2D>& geo)
{
}

void TColorBrush::DrawLine(const VERTEX_2D& p1, const VERTEX_2D& p2, float )
{
    TrecPointer<DrawingBoard> board = TrecPointerKey::TrecFromSoft<>(window);
    if (!board.Get())
        return;
    board->SetShader(TrecPointer<TShader>(), shader_type::shader_2d);
    TColor color(GetColor());

    glBegin(GL_LINES);

    glVertex2d(p1.x, p1.y);
    glVertex2d(p2.x, p2.y);

    glEnd();
}

#pragma once
#include "DrawingBoard.h"
#include "GraphicsDef.h"
class _TC_GRAPH TColorBrush :
    public TBrush
{
    friend class DrawingBoard;
    brush_type brushType;
    TrecPointerSoft<DrawingBoard> window;

    TDataArray<TColor> colors;
public:
    brush_type GetBrushType() override;

	/**
	 * number of colors stored in the Brush
	 */ 
	UINT GetMaxColors();


	/**
	 * Purpose: Sets one of the gradients to a specific color
	 * Parameters: const TColor& color - the color to set the brush to
	 *				UINT index - the index of the gradient to apply the color to (default is 0 for solid brushes will only have 1 gradient)
	 * Note: if the index is out of bounds, the operation will silently fail. to ensure success, call the GetMaxColors() to ensure your index is within bounds
	 */
	void SetColor(const TColor& color, UINT index = 0);

	/**
	 * Purpose: Retrieves the Specified color being drawn by the brush
	 * Parameters: UINT index - the index of the gradient to get the color from (defaults to 0 as a solid brush will only have 1 gradient)
	 * Note: If the index is out of bounds, the color black will be returned. To ensure that the returned color is legitimate, call GetMaxColors()
	 */
	TColor GetColor(UINT index = 0);

	void DrawRectangle(const RECT_F& r, float thickness = 1.0f);

	/**
	 * Method: TBrush::FillRectangle
	 * Purpose: fills the given Rectangle on the DrawingBoard
	 * Parameters: const RECT_2D& r - The Rectangle to apply
	 * Returns: void
	 *
	 * Attributes: virtual
	 */
	virtual void FillRectangle(const RECT_F& r);

	/**
	 * Method: TBrush::DrawRoundedRectangle
	 * Purpose: draws the given Rounded Rectangle on the DrawingBoard
	 * Parameters: const ROUNDED_RECT_2D& r - The Rounded Rectangle to apply
	 *				float thickness -  the thickness of the line (default is 1.0f)
	 * Returns: void
	 */
	void DrawRoundedRectangle(const ROUNDED_RECT_F& r, float thickness = 1.0f);

	/**
	 * Method: TBrush::FillRoundedRectangle
	 * Purpose: fills the given Rounded Rectangle on the DrawingBoard
	 * Parameters: const ROUNDED_RECT_2D& r - The Rounded Rectangle to apply
	 * Returns: void
	 *
	 * Attributes: virtual
	 */
	virtual void FillRoundedRectangle(const ROUNDED_RECT_F& r);

	/**
	 * Method: TBrush::DrawEllipse
	 * Purpose: draws the given Ellipse on the DrawingBoard
	 * Parameters: const ELLIPSE_2D& r - The Ellipse to apply
	 *				float thickness -  the thickness of the line (default is 1.0f)
	 * Returns: void
	 */
	void DrawEllipse(const ELLIPSE_F& r, float thickness = 1.0f);

	/**
	 * Method: TBrush::FillEllipse
	 * Purpose: fills the given Ellipse on the DrawingBoard
	 * Parameters:const ELLIPSE_2D& r - The Ellipse to apply
	 * Returns: void
	 *
	 * Attributes: virtual
	 */
	virtual void FillEllipse(const ELLIPSE_F& r);

	/**
	 * Method: TBrush::DrawGeometry
	 * Purpose: draws the given Geometry on the DrawingBoard
	 * Parameters: TrecPointer<TGeometry> geo - The Geometry to apply
	 *				float thickness -  the thickness of the line (default is 1.0f)
	 * Returns: void
	 */
	void DrawGeometry(const TDataArray<VERTEX_2D>& geo, float thickness = 1.0f);

	/**
	 * Method: TBrush::FillGeometry
	 * Purpose: fills the given Geometry on the DrawingBoard
	 * Parameters: TrecPointer<TGeometry> geo - The Geometry to apply
	 * Returns: void
	 *
	 * Attributes: virtual
	 */
	virtual void FillGeometry(const TDataArray<VERTEX_2D>& geo);


	/**
	 * Method: TBrush::DrawLine
	 * Purpose: Draws a specific line to the Drawing Board
	 * Parameters: const POINT_2D& p1 - the beginning of the line
	 *				const POINT_2D& p2 - the end of the line
	 *				float thickness -  the thickness of the line (default is 1.0f)
	 * Returns: void
	 */
	void DrawLine(const VERTEX_2D& p1, const VERTEX_2D& p2, float thickness = 1.0f);
};


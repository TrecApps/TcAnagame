#include "TGradientStop.h"
#include <TString.h>

TGradientStop::TGradientStop(const TColor& color, float position)
{
	SetColor(color);
	SetPosition(position);
}

TGradientStop::TGradientStop(const TGradientStop& stop)
{
	this->position = stop.position;
	SetColor(stop);
}

/**
 * Method: TGradientStop::TGradientStop
 * Purpose: Default Constructor
 * Parameters: void
 * Returns: New TGradientStop Object
 */
TGradientStop::TGradientStop()
{
	position = 0.0f;
}


/**
 * Method: TGradientStop::SetPosition
 * Purpose: Sets the position of the gradient
 * Parameters: float position - the position to set the grdient to
 * Returns: void
 */
void TGradientStop::SetPosition(float position)
{
	TObjectLocker lock(&thread);
	this->position = position;
	return;
}


/**
 * Method: TGradientStop::GetColor
 * Purpose:  retrieves the color of the gradient
 * Parameters: void
 * Returns: TColor - the color of the gradient
 */
TColor TGradientStop::GetColor()const
{
	TObjectLocker lock(&thread);

	auto ret =  TColor(*this);
	return ret;
}

/**
 * Method: TGradientStop::GetPosition
 * Purpose: Retrieves the position of the gradient
 * Parameters: void
 * Returns: float - the position of the gradient
 */
float TGradientStop::GetPosition()const
{
	return position;
}


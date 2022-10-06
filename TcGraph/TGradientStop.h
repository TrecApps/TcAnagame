#pragma once
#include <TObject.h>
#include "TColor.h"
#include "GraphicsDef.h"
/**
 * Class: TGradientStop
 * Purpose: Holds GradientStops
 * 
 * SuperClass: TObject
 */
class _TC_GRAPH TGradientStop : public TColor
{
public:

	/**
	 * Method: TGradientStop::TGradientStop
	 * Purpose: Attribute Constructor
	 * Parameters: const TColor& color - the color of the new gradient stop
	 *				float position - position of the grdient stop
	 * Returns: New TGradientStop Object
	 */
	TGradientStop(const TColor& color, float position);

	/**
	 * Method: TGradientStop::TGradientStop
	 * Purpose: Copy Constructor
	 * Parameters: const TGradientStop& sto - the object to copy from
	 * Returns: New TGradientStop Object
	 */
	TGradientStop(const TGradientStop& stop);

	/**
	 * Method: TGradientStop::TGradientStop
	 * Purpose: Default Constructor
	 * Parameters: void
	 * Returns: New TGradientStop Object
	 */
	TGradientStop();


	/**
	 * Method: TGradientStop::SetPosition
	 * Purpose: Sets the position of the gradient
	 * Parameters: float position - the position to set the grdient to
	 * Returns: void
	 */
	void SetPosition(float position);



	/**
	 * Method: TGradientStop::GetColor
	 * Purpose:  retrieves the color of the gradient
	 * Parameters: void
	 * Returns: TColor - the color of the gradient
	 * 
	 * Attributes: const
	 */
	TColor GetColor()const;

	/**
	 * Method: TGradientStop::GetPosition
	 * Purpose: Retrieves the position of the gradient
	 * Parameters: void
	 * Returns: float - the position of the gradient
	 * 
	 * Attributes: const
	 */
	float GetPosition()const;

protected:
	float position;
};


#pragma once
#include <TObject.h>
#include "TGradientStop.h"
#include <TDataArray.h>


/**
 * Class: TGradientStopCollection
 * Purpose: Holds a collection of gradientStops for use in creating non-solid TBrush's
 * 
 * SuperClass: TObject
 */
class _TC_GRAPH TGradientStopCollection :	public TObject
{
public:

	/**
	 * Method: TGradientStopCollection::TGradientStopCollection
	 * Purpose: Default Constructor
	 * Parameters: void
	 * Returns: new empty TGradientStopCollection Object
	 */
	TGradientStopCollection();

	/**
	 * Method: TGradientStopCollection::TGradientStopCollection
	 * Purpose: copy constructor
	 * Parameters: const TGradientStopCollection& col - the collection to copy
	 * Returns: new TGradientStopCollection Object
	 */
	TGradientStopCollection(const TGradientStopCollection& col);


	/**
	 * Method: TGradientStopCollection::AddGradient
	 * Purpose: Adds a new gradient to the collection
	 * Parameters: const TGradientStop& newGradient - the grdient to add
	 * Returns: UINT - the new number of gradients currently held
	 */
	UINT AddGradient(const TGradientStop& newGradient);

	/**
	 * Method: TGradientStopCollection::GetGradientCount
	 * Purpose: Reports the current count of the collected gradients
	 * Parameters: void
	 * Returns: UINT - the number of gradients currently held
	 * 
	 * Attributes: const
	 */
	UINT GetGradientCount()const;
	

	/**
	 * Method: TGradientStopCollection::IsValid
	 * Purpose: Reports whether the desired index is within bounds
	 * Parameters: UNIT index - the index of the gradient
	 * Returns: bool - whether the specified index is within bounds
	 * 
	 * Attributes: const
	 */
	bool IsValid(UINT index)const;

	/**
	 * Method: TGradientStopCollection::GetGradientStopAt
	 * Purpose: Retrieves the Gradient at the specified location
	 * Parameters: UINT index - the index of the gradient
	 * Returns: TGradientStop - the Gradient requested
	 * 
	 * Attributes: const
	 */
	TGradientStop GetGradientStopAt(UINT index)const;

	/**
	 * Method: TGradientStopCollection::GetColorAt
	 * Purpose: Gets the color at the specified index
	 * Parameters: UINT index - the index of the gradient
	 * Returns: TColor - the color requested
	 *
	 * Note: if the index is out of bounds, Black is returned
	 */
	TColor GetColorAt(UINT index);


	/**
	 * Method: TGradientStopCollection::SetGradientAt
	 * Purpose: Sets the gradient at the specified location
	 * Parameters: const TGradientStop& gradient - the gradient to set
	 *				UINT index - the index of the gradient to operate on
	 * Returns: bool - whether the operation worked (if the index was in bounds)
	 */
	bool SetGradientAt(const TGradientStop& gradient, UINT index);

	/**
	 * Method: TGradientStopCollection::SetColorAt
	 * Purpose: Sets the color of the specified gradient
	 * Parameters: const TColor& color - the color to set the gradient to
	 *				UINT index - the index of the gradient to operate on
	 * Returns:bool  - whether the operation worked (if the index was in bounds)
	 */
	bool SetColorAt(const TColor& color, UINT index);

	/**
	 * Method: TGradientStopCollection::SetPositionAt
	 * Purpose: Sets the position of the specified gradient
	 * Parameters: float position - the position to set
	 *				UINT index - the index of the gradient to operate on
	 * Returns: bool - whether the operation worked (if the index was in bounds)
	 */
	bool SetPositionAt(float position, UINT index);


	/**
	 * Method: TGradientStopCollection::Empty
	 * Purpose: Empties the collection
	 * Parameters: void
	 * Returns: void
	 */
	void Empty();
protected:
	/**
	 * The collection of gradients
	 */
	TDataArray<TGradientStop> gradients;
};


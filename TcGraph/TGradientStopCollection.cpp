#include "TGradientStopCollection.h"
#include <TString.h>


/**
 * Method: TGradientStopCollection::TGradientStopCollection
 * Purpose: Default Constructor
 * Parameters: void
 * Returns: new empty TGradientStopCollection Object
 */
TGradientStopCollection::TGradientStopCollection()
{
}

/**
 * Method: TGradientStopCollection::TGradientStopCollection
 * Purpose: copy constructor
 * Parameters: const TGradientStopCollection& col - the collection to copy
 * Returns: new TGradientStopCollection Object
 */
TGradientStopCollection::TGradientStopCollection(const TGradientStopCollection& col)
{
	TObjectLocker lock(&thread);
	for (UINT Rust = 0; col.IsValid(Rust); Rust++)
	{
		gradients.push_back(col.GetGradientStopAt(Rust));
	}
	return;
}

/**
 * Method: TGradientStopCollection::AddGradient
 * Purpose: Adds a new gradient to the collection
 * Parameters: const TGradientStop& newGradient - the grdient to add
 * Returns: UINT - the new number of gradients currently held
 */
UINT TGradientStopCollection::AddGradient(const TGradientStop& newGradient)
{
	TObjectLocker lock(&thread);
	gradients.push_back(newGradient);
	UINT ret = gradients.Size() - 1;
	return ret;
}

/**
 * Method: TGradientStopCollection::GetGradientCount
 * Purpose: Reports the current count of the collected gradients
 * Parameters: void
 * Returns: UINT - the number of gradients currently held
 */
UINT TGradientStopCollection::GetGradientCount() const
{
	TObjectLocker lock(&thread);
		UINT ret = gradients.Size();
	return ret;
}

/**
 * Method: TGradientStopCollection::IsValid
 * Purpose: Reports whether the desired index is within bounds
 * Parameters: UNIT index - the index of the gradient
 * Returns: bool - whether the specified index is within bounds
 */
bool TGradientStopCollection::IsValid(UINT index) const
{
	TObjectLocker lock(&thread);
		bool ret = index < gradients.Size();
	return ret;
}

/**
 * Method: TGradientStopCollection::GetGradientStopAt
 * Purpose: Retrieves the Gradient at the specified location
 * Parameters: UINT index - the index of the gradient
 * Returns: TGradientStop - the Gradient requested
 */
TGradientStop TGradientStopCollection::GetGradientStopAt(UINT index)const
{
	TObjectLocker lock(&thread);
		if (!IsValid(index))
		{
			return TGradientStop();
		}
	auto ret =  gradients.data()[index];
	return ret;
}

/**
 * Method: TGradientStopCollection::GetColorAt
 * Purpose: Gets the color at the specified index
 * Parameters: UINT index - the index of the gradient
 * Returns: TColor - the color requested
 *
 * Note: if the index is out of bounds, Black is returned
 */
TColor TGradientStopCollection::GetColorAt(UINT index)
{
	TObjectLocker lock(&thread);
		if (!IsValid(index))
		{
			return TColor();
		}
	TColor ret = gradients.data()[index].GetColor();
	return ret;
}

/**
 * Method: TGradientStopCollection::SetGradientAt
 * Purpose: Sets the gradient at the specified location
 * Parameters: const TGradientStop& gradient - the gradient to set
 *				UINT index - the index of the gradient to operate on
 * Returns: bool - whether the operation worked (if the index was in bounds)
 */
bool TGradientStopCollection::SetGradientAt(const TGradientStop& gradient, UINT index)
{
	TObjectLocker lock(&thread);
		if (!IsValid(index))
		{
			return false;
		}
	gradients[index] = gradient;
	return true;
}

/**
 * Method: TGradientStopCollection::SetColorAt
 * Purpose: Sets the color of the specified gradient
 * Parameters: const TColor& color - the color to set the gradient to
 *				UINT index - the index of the gradient to operate on
 * Returns:bool  - whether the operation worked (if the index was in bounds)
 */
bool TGradientStopCollection::SetColorAt(const TColor& color, UINT index)
{
	TObjectLocker lock(&thread);
		if (!IsValid(index))
		{
			return false;
		}
	gradients[index].SetColor(color);
	return true;
}

/**
 * Method: TGradientStopCollection::SetPositionAt
 * Purpose: Sets the position of the specified gradient
 * Parameters: float position - the position to set
 *				UINT index - the index of the gradient to operate on
 * Returns: bool - whether the operation worked (if the index was in bounds)
 */
bool TGradientStopCollection::SetPositionAt(float position, UINT index)
{
	TObjectLocker lock(&thread);
		if (!IsValid(index))
		{
			return false;
		}
	gradients[index].SetPosition(position);
	return true;
}

/**
 * Method: TGradientStopCollection::Empty
 * Purpose: Empties the collection
 * Parameters: void
 * Returns: void
 */
void TGradientStopCollection::Empty()
{
	TObjectLocker lock(&thread);
	gradients.RemoveAll();
	return;
}

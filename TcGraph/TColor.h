#pragma once
#include <TString.h>
#include "TcGraph.h"

using color_struct = struct color_struct {
	float r, b, g, a;
};

class _TC_GRAPH TColor :
    public TVObject
{
public:

	TColor();

	TColor(float r, float g, float b);

	TColor(float r, float g, float b, float a);

	TColor(const TColor& c);

	TColor(const color_struct& color);

	TColor(const TString& colorStr);

	void SetColor(const TString& colorStr);

	void SetColor(float r, float g, float b);

	void SetColor(float r, float g, float b, float a);

	void SetColor(const TColor& co);

	color_struct GetColor() const;

	float GetRed()const;

	float GetBlue()const;

	float GetGreen()const;

	float GetOpacity()const;

	static TColor GetColorFromString(const TString& color, bool& worked);

private:
	color_struct color;
};


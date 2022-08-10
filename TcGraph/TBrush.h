#pragma once
#include "TcGraph.h"
#include <TString.h>

#include <GLFW/glfw3.h>

#include "TColor.h"

using brush_type = enum class brush_type
{
	brush_type_solid,	// Basic Brush
	brush_type_linear,	// multiple colors drawn linearly
	brush_type_radial,
	brush_type_bitmap,
	brush_type_video,
	brush_type_arena
};

class _TC_GRAPH TBrush :
    public TVObject
{
public:
	virtual brush_type GetBrushType() = 0;
};


#pragma once


using RECT_F = struct {
    float left, top, right, bottom;
};
using RECT_L = struct {
    long left, top, right, bottom;
};

using VERTEX_2D = struct {
    float x, y;
};

using ROUNDED_RECT_F = struct {
    RECT_F rect;
    float x, y;
};

using ELLIPSE_F = struct {
    VERTEX_2D point;
    float x, y;
};
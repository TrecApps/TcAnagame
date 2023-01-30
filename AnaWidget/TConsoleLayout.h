#pragma once
#include "TLayout.h"
#include "TTextLayout.h"

class TConsoleLayout :
    public TLayout
{
    bool isInput;
public:
    virtual bool onCreate(const RECT_F& loc, TrecPointer<TFileShell> d)override;


};


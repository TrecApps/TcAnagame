#pragma once


#include <TObject.h>
#include <TFile.h>

class TShader :
    public TObject
{
protected:
    int shaderId;

public:

    static TrecPointer<TShader> GenerateShader(TrecPointer<TFileShell> filePath);

    ~TShader()override;

    bool Use()const;
};

class TFreeTypeShader : public TShader
{
public:
    TFreeTypeShader();
};
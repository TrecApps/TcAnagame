#include "TConsoleLayout.h"

bool TConsoleLayout::onCreate(const RECT_F& loc, TrecPointer<TFileShell> d)
{
    TString valpoint;
    if (attributes.retrieveEntry(L"IsInput", valpoint) && !valpoint.CompareNoCase(L"false"))
        isInput = false;

    this->childControls.RemoveAll();

    AddRow(1, true);
    auto topText = TrecPointerKey::GetNewSelfTrecPointerAlt<TPage, TTextLayout>(drawingBoard, styles);
    dynamic_cast<TControl*>(topText.Get())->AddAttribute(L"IsLocked", L"true");
    AddPage(topText, 0, 0);

    if (isInput)
    {
        AddRow(20, false);
        AddRow(30, false);
        auto directoryInput = TrecPointerKey::GetNewSelfTrecPointerAlt<TPage, TTextInput>(drawingBoard, styles);
        dynamic_cast<TTextInput*>(directoryInput.Get())->LockText(true);
        AddPage(directoryInput, 1, 0);
        AddPage(TrecPointerKey::GetNewSelfTrecPointerAlt<TPage, TTextInput>(drawingBoard, styles), 2, 0);
    }

    return TRandomLayout::onCreate(loc, d);
}

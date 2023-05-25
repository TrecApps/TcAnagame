#pragma once
#include <TPage.h>
class FindTextHandler :
    public TPage::EventHandler
{
    typedef void (FindTextHandler::* FindTextCall)(TrecPointer<TPage> tc, EventArgs ea);
    TDataArray<FindTextCall> calls;


public:
    FindTextHandler();
};


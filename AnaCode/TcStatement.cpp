#include "TcStatement.h"

UINT TcObjectProcedure::GetSize()
{
    return statements.Size();
}

TcStatement& TcObjectProcedure::GetStatement(UINT index)
{
    if (index >= statements.Size())
        throw 1;

    return statements.at(index);
}




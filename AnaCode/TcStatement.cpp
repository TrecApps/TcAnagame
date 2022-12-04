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

TrecPointer<TVariable> TcObjectProcedure::Clone()
{
    return TrecPointer<TVariable>();
}

procedure_type TcObjectProcedure::GetProcedureType(void)
{
    return procedure_type::object;
}




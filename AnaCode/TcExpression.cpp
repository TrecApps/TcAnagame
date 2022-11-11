#include "TcExpression.h"

var_type TcExpression::GetVarType()
{
    return var_type::expression;
}

bool TcExpression::CanAssign()
{
    return false;
}

bool TcAssignableExpression::CanAssign()
{
    return true;
}

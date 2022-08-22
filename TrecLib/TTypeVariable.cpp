#include "TTypeVariable.h"

var_type TTypeVariable::GetVarType()
{
    return var_type::type;
}

UINT TTypeVariable::Get4Value()
{
    return 0;
}

UINT TTypeVariable::GetSize()
{
    return 0;
}

type_category TArrayType::GetTypeCategory()
{
    return type_category::array_type;
}

int TArrayType::Size() const
{
    return size;
}

TrecPointer<TTypeVariable> TArrayType::GetVariableType() const
{
    return TrecPointer<TTypeVariable>();
}

TArrayType::TArrayType() : TArrayType(-1)
{
}

TArrayType::TArrayType(int size): size(size)
{
}

TArrayType::TArrayType(TrecPointer<TTypeVariable> varType): size(-1), variableType(varType)
{
}

TArrayType::TArrayType(int size, TrecPointer<TTypeVariable> varType): size(size), variableType(varType)
{
}

type_category TFeatureType::GetTypeCategory()
{
    return type_category::feature;
}

bool TFeatureType::IsMutable() const
{
    return isMut;
}

bool TFeatureType::IsPointer() const
{
    return pointerReference > 0;
}

bool TFeatureType::IsPlain() const
{
    return pointerReference == 0;
}

bool TFeatureType::IsReference() const
{
    return pointerReference < 1;
}

TrecPointer<TTypeVariable> TFeatureType::GetType() const
{
    return type;
}

TrecPointer<TFeatureType> TFeatureType::GetPointerType(TrecPointer<TTypeVariable> t, bool isMut)
{
    if (t.Get())
        return TrecPointer<TFeatureType>();
    auto base = TrecPointerKey::GetNewSelfTrecPointerAlt<TVariable, TFeatureType>();
    auto ret = TrecPointerKey::ConvertPointer<TVariable, TFeatureType>(base);
    ret->isMut = isMut;
    ret->pointerReference = 1;
    ret->type = t;
    return ret;
}

TrecPointer<TFeatureType> TFeatureType::GetPlainType(TrecPointer<TTypeVariable> t, bool isMut)
{
    if (t.Get())
        return TrecPointer<TFeatureType>();
    auto base = TrecPointerKey::GetNewSelfTrecPointerAlt<TVariable, TFeatureType>();
    auto ret = TrecPointerKey::ConvertPointer<TVariable, TFeatureType>(base);
    ret->isMut = isMut;
    ret->pointerReference = 0;
    ret->type = t;
    return ret;
}

TrecPointer<TFeatureType> TFeatureType::GetReferenceType(TrecPointer<TTypeVariable> t, bool isMut)
{
    if (t.Get())
        return TrecPointer<TFeatureType>();
    auto base = TrecPointerKey::GetNewSelfTrecPointerAlt<TVariable, TFeatureType>();
    auto ret = TrecPointerKey::ConvertPointer<TVariable, TFeatureType>(base);
    ret->isMut = isMut;
    ret->pointerReference = -1;
    ret->type = t;
    return ret;
}

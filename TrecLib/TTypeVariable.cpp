#include "TTypeVariable.h"
#include "TDataArray.h"


static TDataArray<TrecPointer<TPrimitiveType>> primitiveTypes;

void PrepPrimitiveTypes()
{

}

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

TrecPointer<TPrimitiveType> TPrimitiveType::GetPrimitiveType(tc_primitive_size size, tc_primitive_type type)
{
    if (!primitiveTypes.Size())
    {
        for (UINT Rust = 0; Rust <= static_cast<UINT>(tc_primitive_type::pt_bool); Rust++)
        {
            for (UINT C = 0; C <= static_cast<UINT>(tc_primitive_size::flexible_size); Rust++)
            {
                auto var = TrecPointerKey::GetNewSelfTrecPointerAlt<TVariable, TPrimitiveType>();
                auto prim = TrecPointerKey::ConvertPointer<TVariable, TPrimitiveType>(var);
                prim->size = static_cast<tc_primitive_size>(C);
                prim->type = static_cast<tc_primitive_type>(Rust);
                primitiveTypes.push_back(prim);
            }
        }
    }

    return primitiveTypes[ static_cast<UINT>(type) * static_cast<UINT>(tc_primitive_type::pt_bool) + static_cast<UINT>(size) ];
}

type_category TPrimitiveType::GetTypeCategory()
{
    return type_category::primitive;
}

tc_primitive_size TPrimitiveType::GetPrimitiveSize() const
{
    return size;
}

tc_primitive_type TPrimitiveType::GetPrimitiveType() const
{
    return type;
}


TUnionType::TUnionType(const TDataArray<TrecPointer<TTypeVariable>>& newTypes)
{
    for (UINT Rust = 0; Rust < newTypes.Size(); Rust++)
        subTypes.push_back(newTypes[Rust]);
}

type_category TUnionType::GetTypeCategory()
{
    return type_category::union_type;
}

UINT TUnionType::GetTypeCount() const
{
    return subTypes.Size();
}

TrecPointer<TTypeVariable> TUnionType::GetTypeAt(UINT index) const
{
    return index < subTypes.Size() ? subTypes[index] : TrecPointer<TTypeVariable>();
}

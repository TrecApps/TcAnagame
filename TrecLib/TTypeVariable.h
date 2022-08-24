#pragma once
#include "TObject.h"
#include "TDataArray.h"

using type_category = enum class type_category
{
    array_type, // Raw Array
    class_type, // Class Types
    feature,    // holds modifiers such as pointer, reference, mutability
    primitive,  // Primitive variables
    union_type  // holds multiple types, one of which is active
};

class TTypeVariable :
    public TVariable
{
public:

    virtual type_category GetTypeCategory() = 0;

    var_type GetVarType() override;
    UINT Get4Value() override;

    UINT GetSize() override;
};

class TArrayType : public TTypeVariable
{
private:
    int size;

    TrecPointer<TTypeVariable> variableType;

public:
    type_category GetTypeCategory() override;


    // if negative, this array's size is dynamic
    int Size() const;

    // If Null, then elements could be of ay type
    TrecPointer<TTypeVariable> GetVariableType() const;

    // Produces a spec of a dynamic array of any type
    TArrayType();
    // Produces a spec of an 'any' type array of the specified type
    explicit TArrayType(int size);
    // Procuces a spec of a dynamic array of the specified type
    explicit TArrayType(TrecPointer<TTypeVariable> varType);
    // Produces a spec of an array of the specific type and size
    explicit TArrayType(int size, TrecPointer<TTypeVariable> varType);
};

class TFeatureType : public TTypeVariable
{
private:
    signed char pointerReference;
    bool isMut;
    TrecPointer<TTypeVariable> type;
public:
    type_category GetTypeCategory() override;
    bool IsMutable()const;
    bool IsPointer()const;
    bool IsPlain()const;
    bool IsReference()const;
    TrecPointer<TTypeVariable> GetType()const;

    static TrecPointer<TFeatureType> GetPointerType(TrecPointer<TTypeVariable>, bool isMut = true);

    static TrecPointer<TFeatureType> GetPlainType(TrecPointer<TTypeVariable>, bool isMut = true);

    static TrecPointer<TFeatureType> GetReferenceType(TrecPointer<TTypeVariable>, bool isMut = true);
};

using tc_primitive_type = enum class tc_primitve_type {
    pt_int,
    pt_uint,
    pt_float,
    pt_char,
    pt_bool
};

using tc_primitive_size = enum class tc_primitive_size {
    one_byte,       // 8-bits
    two_byte,       // 16-bits
    four_byte,      // 32-bits
    eight_byte,     // 64-bits
    twelve_byte,    // 96-bits
    sixteen_byte,   // 128-bits
    flexible_size   // Size varies (should not be used in Binary-Runners)
};

class TPrimitiveType : public TTypeVariable
{
    friend class TrecPointerKey;
    tc_primitive_size size;
    tc_primitive_type type;
    explicit TPrimitiveType() = default;
public:
    static TrecPointer<TPrimitiveType> GetPrimitiveType(tc_primitive_size size, tc_primitive_type type);

    type_category GetTypeCategory() override;
    tc_primitive_size GetPrimitiveSize() const;
    tc_primitive_type GetPrimitiveType() const;
};

class TUnionType : public TTypeVariable
{
    TDataArray<TrecPointer<TTypeVariable>> subTypes;
public:
    explicit TUnionType(const TDataArray<TrecPointer<TTypeVariable>>& newTypes);
    type_category GetTypeCategory() override;
    UINT GetTypeCount() const;
    TrecPointer<TTypeVariable> GetTypeAt(UINT index) const;
};

class TClassType : public TTypeVariable
{

};
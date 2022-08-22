#pragma once
#include "TObject.h"

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

class TPrimitiveType : public TTypeVariable
{

};

class TUnionType : public TTypeVariable
{

};

class TClassType : public TTypeVariable
{

};
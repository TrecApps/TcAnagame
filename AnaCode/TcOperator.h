#pragma once
#include <TcRunner.h>
class TcOperator :
    public TCoreObject
{
public:
    virtual void IsTruthful(TrecPointer<TVariable> v1, ReturnObject& obj) = 0;

    virtual void Add(TrecPointer<TVariable> v1, TrecPointer<TVariable> v2, ReturnObject& obj) = 0;
    virtual void Sub(TrecPointer<TVariable> v1, TrecPointer<TVariable> v2, ReturnObject& obj) = 0;
    virtual void Mul(TrecPointer<TVariable> v1, TrecPointer<TVariable> v2, ReturnObject& obj) = 0;
    virtual void Div(TrecPointer<TVariable> v1, TrecPointer<TVariable> v2, ReturnObject& obj) = 0;
    virtual void Mod(TrecPointer<TVariable> v1, TrecPointer<TVariable> v2, ReturnObject& obj) = 0;
    virtual void Pow(TrecPointer<TVariable> v1, TrecPointer<TVariable> v2, ReturnObject& obj) = 0;
};


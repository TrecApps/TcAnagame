#pragma once
#include <TcRunner.h>

class _TREC_LIB_DLL TcOperator :
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
union doubleLong
{
    LONG64 s;
    ULONG64 u;
    double d;
};

/**
 * Enum Class: double_long
 * Purpose: used to keep track of the status of the doubleLong union
 */
enum class double_long
{
    dl_sign,  // Set to a signed integer
    dl_unsign,// Set to an unsigned integer
    dl_double,// Set to double
    dl_invalid // Not properly set
};


class _TREC_LIB_DLL DoubleLong
{
public:
    DoubleLong(ULONG64 val);
    DoubleLong(LONG64 val);
    DoubleLong(double val);
    DoubleLong();
    doubleLong value;
    double_long type;

    bool operator<(const DoubleLong& o);
    bool operator<=(const DoubleLong& o);
    bool operator>=(const DoubleLong& o);
    bool operator>(const DoubleLong& o);
    bool operator==(const DoubleLong& o);

    ULONG64 ToUnsignedLong()const;

    ULONG64 GetBitAnd(const DoubleLong& o);
    ULONG64 GetBitOr(const DoubleLong& o);
    ULONG64 GetBitXor(const DoubleLong& o);

    static DoubleLong GetValueFromPrimitive(TrecPointer<TVariable> var);

    static DoubleLong GetValueFromString(const TString& str);

    TString ToString();
};

DoubleLong _TREC_LIB_DLL Add(const DoubleLong& v1, const DoubleLong& v2);
DoubleLong _TREC_LIB_DLL Subtract(const DoubleLong& v1, const DoubleLong& v2);
DoubleLong _TREC_LIB_DLL Multiply(const DoubleLong& v1, const DoubleLong& v2);
DoubleLong _TREC_LIB_DLL Divide(const DoubleLong& v1, const DoubleLong& v2);
DoubleLong _TREC_LIB_DLL ModDivide(const DoubleLong& v1, const DoubleLong& v2);
DoubleLong _TREC_LIB_DLL Exponent(const DoubleLong& v1, const DoubleLong& v2);

TString _TREC_LIB_DLL GetStringFromPrimitive(TrecPointer<TVariable> var);

typedef enum class tc_int_op
{
    none, // There is no special operation involved
    // Basic Arithmetic
    add,
    sub,
    mult,
    div,
    mod,
    pow,
    add_assign,
    sub_assign,
    mul_assign,
    div_assign,
    mod_assign,
    pow_assign,
    pre_inc,
    post_inc,
    pre_dec,
    post_dec,
    // Logical Operators
    and_l,
    or_l,
    xor_l,
    not_l,
    and_assign,
    or_assign,
    xor_assign,
    // equality
    eq,
    eq_t,
    gt,
    //gt_t,
    gte,
    //gte_t,
    lt,
    //lt_t,
    lte,
    //lte_t,
    not_e,
    not_e_t,
    // expansion
    basic_exp,
    point_exp,
    // Bitwise
    and_b,
    or_b,
    xor_b,
    left_b,
    right_b,
    b_and_assign,
    b_or_assign,
    b_xor_assign,
    b_left_assign,
    b_right_assign,
    // Misclanous
    reg_assign,
    conditional,
    separator
}tc_int_op;
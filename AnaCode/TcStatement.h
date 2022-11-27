#pragma once
#include <TrecPointer.h>
#include <TcRunner.h>
#include "AnaCode.h"
#include "TcExpression.h"

using statement_type = enum class statement_type
{
    st_regular,
    // Regular Control
    st_if,
    st_n_if,
    st_while,
    st_until,
    st_do_while,
    st_for_1,
    st_for_3,

    // Switch Control
    st_switch,
    st_case,
    st_switch_default,

    // Declarations
    st_declare_new,
    st_declare_new_const,
    st_declare_new_hoist,

    // Exception handling
    st_try,
    st_catch,
    st_finally,
    st_throw,

    // internal control
    st_return,
    st_break,
    st_continue,
    st_yeild,
    st_goto,
    st_goto_target


};


class _ANA_CODE TcObjectProcedure : public TcProcedure
{
public:
    class TcStatement :
        public TCoreObject
    {
    public:
        TrecPointer<TcExpression> primeExpression;
        TrecPointer<TcExpression> secondaryExpression;
        TrecPointer<TcExpression> tertiaryEpression;
        statement_type statementType;

        TrecPointer<TcObjectProcedure> block;

        TrecPointer<TcStatement> next;

        TcStatement() = default;
        TcStatement(const TcStatement& copy) = default;
    };

protected:
    TrecPointer<TDataArray<TcStatement>> statements;

    TString name;

public:
    TcObjectProcedure() = default;
    TcObjectProcedure(const TcObjectProcedure& copy) = default;

};
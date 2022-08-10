#pragma once
#include "TString.h"

#include <thread>

using runner_block_mode = enum class runner_block_mode {
    block,          // This Runner Only supports running in the calling thread
    supports_async, // This runner runs in the calling thread but can run in a separate thread if requested
    async           // This runner runs in a separate thread by Default, but can be called upon to run in the calling thread
};

using runner_method_mode = enum class runner_method_mode {
    regular,        // This Runner does not consider itself a method
    method,         // This Runner considers itself a method and could modify the object in question
    method_const    // This Runner considers itself a method and declares it will not alter the object in question
};

using return_mode = enum class return_mode
{
    rm_regular,     // Nothing special
    rm_break,       // Break was detected
    rm_continue,    // Continue was detected
    rm_return,       // Return was detected
    rm_super_return, // Return was detected, and the caller should also return
    rm_yield        // Similar to Return, but function is expected to resume from the yield point
};


class _TREC_LIB_DLL ReturnObject
{
public:
    TString errorMessage;               // String Information
    TrecPointer<TVariable> errorObject; // A Variable Generated
    TrecPointer<TVariable> errorObject2;// Backup Variable
    USHORT returnCode;                  // 0 if working, error otherwise
    TDataArray<TString> stackTrace;     // stack trace info
    return_mode mode;                   // Mode, the context in which an operation ended
    USHORT nextCount;                   // How many Statements were part of a single Expression (statement and next statement)

    ReturnObject();

    ReturnObject(const ReturnObject& copy);

    ReturnObject& operator=(const ReturnObject& copy);

    const static USHORT ERR_NO_ERROR = 0;
    const static USHORT ERR_BRACKET_MISMATCH = 1;
    const static USHORT ERR_PARENTH_MISMATCH = 2;
    const static USHORT ERR_NOT_NUMBER = 3;
    const static USHORT ERR_TOO_FEW_PARMS = 4;
    const static USHORT ERR_EXISTING_VAR = 5;
    const static USHORT ERR_UNDEFINED = 6;
    const static USHORT ERR_BROKEN_REF = 7;
    const static USHORT ERR_INCOMPLETE_STATEMENT = 8;
    const static USHORT ERR_IMPROPER_TYPE = 9;
    const static USHORT ERR_IMPROPER_NAME = 10;
    const static USHORT ERR_UNEXPECTED_TOK = 11;
    const static USHORT ERR_UNSUPPORTED_OP = 12;
    const static USHORT ERR_INVALID_FILE_PARAM = 13;

    const static USHORT ERR_THROWN = 19;
    const static USHORT ERR_GENERIC_ERROR = 20;
    const static USHORT ERR_INTERNAL = 21;          // Indicates an Issue that the Interpretor has detected witin itself

};

class VariableHolder : public TVObject
{
public:
    VariableHolder();
    VariableHolder(const VariableHolder& copy);
    VariableHolder& operator=(const VariableHolder& copy);
    ~VariableHolder();

    /*
     * The type of variable being held (if blank, assume anything) 
     */
    TString type;
    /*
     * Level of mutability
     * 0 = completely mutable (though the type should be maintained)
     * 1 = the Variable can't change but it can be operated upon
     * 2 = The variable can change, but it must not be operated upon
     * 3 = the variable can't change and it must not be operated upon
     */
    UCHAR mutLevel;

    /*
     * In a parameter List, determines if this could hold an array of values of the specified type. Ignored otherwise
     */
    bool isArray;

    /*
     * Whether the Parameter can be null (true by default)
     */
    bool nullable;

    /*
     * The actual value
     */
    TrecPointer<TVariable> value;
};



class TcRunner :
    public TVariable
{
protected:
    TDataArray<VariableHolder> variableSpecs;       // Variables that the Runner needs before running
    TDataArray<VariableHolder> declaredVariables;   // Variables held by the Runner during the Run

    ULONG64 termination;                              // Whether the Runner Should terminate after a set amout of time (0 for no)

    bool doAsync;       // Whether to run this asyncronously
    bool doTerminate;   // Whether to terminate or not, should start out as false

    virtual void RunDetails(ReturnObject& ret) = 0;
    
public:

    TcRunner() = default;

    void GetParameterSpecs(TDataArray<VariableHolder>& specs);

    void SetParameterSpecs(const TDataArray<VariableHolder>& specs);

    virtual void SetParameters(TrecPointer<TVariable>& params, ReturnObject& result);

    void SetTerminate(ULONG64 termination);

    void Terminate();

    virtual runner_block_mode GetBlockMode() = 0;

    virtual bool SetAsync() = 0;

    void Run(ReturnObject& ret);
};

class TcAsyncVariable : public TVariable
{
private:
    TrecPointer<TcRunner> mainFunction;
    VariableHolder varHolder;           // The Place to retrieve the resulting value once complete
    signed char process;
protected:

    using AsyncResponseRunner = struct AsyncResponseRunner {
        TrecPointer<TcRunner> accepted;
        TrecPointer<TcRunner> rejected;
    };

    TDataArray<AsyncResponseRunner> response;

    TrecPointer<TcRunner> finalCall;

    mutable ThreadBlocker thread;

    std::thread* threadReference;

    bool doTerminate;

public:

    TcAsyncVariable(TrecPointer<TcRunner> mainRunner);

    ~TcAsyncVariable();

    static void Invoke_(TrecPointer<TcAsyncVariable> thisVar);

    void SetExpectedType(const TString& type);

    void Invoke();

    TrecPointer<TVariable> GetResult();

    signed char GetProgess();

    void AppendResponse(TrecPointer<TcRunner> success, TrecPointer<TcRunner> rejected);

    void SetFinalResponse(TrecPointer<TcRunner> finallyRunner);
};
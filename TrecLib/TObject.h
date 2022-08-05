#pragma once
#include "BaseTypes.h"
#include "TrecLib.h"

class _TREC_LIB_DLL TObjectLocker
{
public:
	explicit TObjectLocker(ThreadBlocker* section);
	~TObjectLocker();

private:
	ThreadBlocker* section;
};

class _TREC_LIB_DLL TObject
{
public:
	using ObjectType = enum class ObjectType
	{
		ot_variable,	// Contians some value
		ot_expression,	// Capable of Yielding some value
		ot_runner,		// Basic Runner (like an object based function)
		ot_compiler,	// Generates Runners
		ot_environment,	// Holds resources used by Other objects
		ot_handler,		// Handles events directed it's way
		ot_page,		// Represents something that can be drawn
		ot_writer,		// Writes to some target
		ot_reader,		// Reads from some source
		ot_other		// Does not fit into the previous categories
	};

protected:
	mutable ThreadBlocker thread;

	bool deleteLock;
public:

	TObject();

	virtual ~TObject();

	bool IsDeleteLocked() const;

	void ThreadLock() const;

	void ThreadRelease()const;
};


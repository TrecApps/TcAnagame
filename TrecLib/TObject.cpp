
#include "TObject.h"

TObjectLocker::TObjectLocker(ThreadBlocker* section) : section(section)
{
	TcLockObject(section);
}

TObjectLocker::~TObjectLocker()
{
	TcUnlockObject(section);
}

TObject::TObject(): deleteLock(false)
{
	TcInitLock(&thread);
}

TObject::~TObject()
{
	TcRemoveLock(&thread);
}

bool TObject::IsDeleteLocked() const
{
	return deleteLock;
}


void TObject::ThreadLock() const
{

	EnterCriticalSection(&thread);
}

void TObject::ThreadRelease() const
{
	LeaveCriticalSection(&thread);

}

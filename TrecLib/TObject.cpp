
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

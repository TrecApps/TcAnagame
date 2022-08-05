#pragma once



#ifdef _WINDOWS
#include <Windows.h>
using ThreadBlocker = CRITICAL_SECTION;

#define TcLockObject(section)		EnterCriticalSection(section)
#define TcUnlockObject(section)		LeaveCriticalSection(section)
#define TcInitLock(section)			InitializeCriticalSection(section)
#define TcRemoveLock(section)		LeaveCriticalSection(section)

#elif  defined(__linux__) || (defined (__APPLE__) && defined (__MACH__))

#include <pthread.h>
using ThreadBlocker = pthread_mutex_t;

#define TcLockObject(section)		pthread_mutex_lock(section)
#define TcUnlockObject(section)		pthread_mutex_unlock(section)
#define TcInitLock(section)			pthread_mutex_init(section, nullptr)
#define TcRemoveLock(section)		pthread_mutex_destroy(section)

#endif
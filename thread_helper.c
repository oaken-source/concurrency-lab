
#include "thread_helper.h"

int
thread_helper_create(thread_helper_t *thread, thread_helper_return_t(*thread_func)(void*), void *arg)
{
#ifdef _WIN32
  // Windows Implementation based on CreateThread
  *thread = CreateThread(NULL, 0, thread_func, arg, 0, NULL);
  return *thread != NULL ? 0 : -1;
#else
  // POSIX Implementation based on pthread_create
  return pthread_create(thread, NULL, thread_func, arg);
#endif
}

int
thread_helper_join(thread_helper_t thread)
{
#ifdef _WIN32
  // Windows Implementation based on WaitForSingleObject
  DWORD res = WaitForSingleObject(thread, INFINITE);
  CloseHandle(thread);
  return res == WAIT_OBJECT_0 ? 0 : res;
#else
  // POSIX Implementation based on pthread_join
  return pthread_join(thread, NULL);
#endif
}

int
thread_helper_mutex_init(thread_helper_mutex_t *semaphore)
{
#ifdef _WIN32
  // Windows Implementation based on InitializeCriticalSection
  InitializeCriticalSection(semaphore);
  return 0;
#else
  // POSIX Implementation based on pthread_mutex_init
  return pthread_mutex_init(semaphore, NULL);
#endif
}

int
thread_helper_mutex_lock(thread_helper_mutex_t *semaphore)
{
#ifdef _WIN32
  // Windows Implementation based on EnterCriticalSection
  EnterCriticalSection(semaphore);
  return 0;
#else
  // POSIX Implementation based on pthread_mutex_lock
  return pthread_mutex_lock(semaphore);
#endif
}

int
thread_helper_mutex_unlock(thread_helper_mutex_t *semaphore)
{
#ifdef _WIN32

  // Windows Implementation based on LeaveCriticalSection
  LeaveCriticalSection(semaphore);
  return 0;
#else
  // POSIX Implementation based on pthread_mutex_unlock
  return pthread_mutex_unlock(semaphore);
#endif
}

int
thread_helper_test_and_set_lock(int *lock)
{
#ifdef _MSC_VER
  // cl.exe Implementation based on _interlockedbittestandset intrinsic
  return _interlockedbittestandset(lock, 0);
#else
  // gcc Implementation based on __sync_lock_test_and_set intrinsic
  return __sync_lock_test_and_set(lock, 1);
#endif
// TODO: this probably does not work with clang.
}

void
thread_helper_test_and_set_unlock(int *lock)
{
#ifdef _MSC_VER
  // cl.exe Implementation based on _interlockedbittestandreset
  _interlockedbittestandreset(lock, 0);
#else
  // gcc Implementation based on __sync_lock_release
  __sync_lock_release(lock);
#endif
// TODO: this probably does not work with clang.
}

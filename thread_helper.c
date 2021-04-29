
#include "thread_helper.h"

int
thread_helper_create(thread_helper_t *thread, thread_helper_return_t(*thread_func)(void*), void *arg)
{
#ifdef _WIN32
  // Windows Implementation based on CreateThread
  //   see: https://docs.microsoft.com/en-us/windows/win32/api/processthreadsapi/nf-processthreadsapi-createthread
  *thread = CreateThread(NULL, 0, thread_func, arg, 0, NULL);
  return *thread != NULL ? 0 : -1;
#else
  // POSIX Implementation based on pthread_create
  //   see: https://man7.org/linux/man-pages/man3/pthread_create.3.html
  return pthread_create(thread, NULL, thread_func, arg);
#endif
}

int
thread_helper_join(thread_helper_t thread)
{
#ifdef _WIN32
  // Windows Implementation based on WaitForSingleObject
  //   see: https://docs.microsoft.com/en-us/windows/win32/api/synchapi/nf-synchapi-waitforsingleobject
  DWORD res = WaitForSingleObject(thread, INFINITE);
  CloseHandle(thread);
  return res == WAIT_OBJECT_0 ? 0 : res;
#else
  // POSIX Implementation based on pthread_join
  //   see: https://man7.org/linux/man-pages/man3/pthread_join.3.html
  return pthread_join(thread, NULL);
#endif
}

int
thread_helper_mutex_init(thread_helper_mutex_t *semaphore)
{
#ifdef _WIN32
  // Windows Implementation based on InitializeCriticalSection
  //   see: https://docs.microsoft.com/en-us/windows/win32/api/synchapi/nf-synchapi-initializecriticalsection
  InitializeCriticalSection(semaphore);
  return 0;
#else
  // POSIX Implementation based on pthread_mutex_init
  //   see: https://man7.org/linux/man-pages/man3/pthread_mutex_destroy.3p.html
  return pthread_mutex_init(semaphore, NULL);
#endif
}

int
thread_helper_mutex_lock(thread_helper_mutex_t *semaphore)
{
#ifdef _WIN32
  // Windows Implementation based on EnterCriticalSection
  //   see: https://docs.microsoft.com/en-us/windows/win32/api/synchapi/nf-synchapi-entercriticalsection
  EnterCriticalSection(semaphore);
  return 0;
#else
  // POSIX Implementation based on pthread_mutex_lock
  //   see: https://man7.org/linux/man-pages/man3/pthread_mutex_lock.3p.html
  return pthread_mutex_lock(semaphore);
#endif
}

int
thread_helper_mutex_unlock(thread_helper_mutex_t *semaphore)
{
#ifdef _WIN32

  // Windows Implementation based on LeaveCriticalSection
  //   see: https://docs.microsoft.com/en-us/windows/win32/api/synchapi/nf-synchapi-leavecriticalsection
  LeaveCriticalSection(semaphore);
  return 0;
#else
  // POSIX Implementation based on pthread_mutex_unlock
  //   see: https://man7.org/linux/man-pages/man3/pthread_mutex_lock.3p.html
  return pthread_mutex_unlock(semaphore);
#endif
}

int
thread_helper_test_and_set_lock(int *lock)
{
#ifdef _MSC_VER
  // cl.exe Implementation based on _interlockedbittestandset intrinsic
  //   see: https://docs.microsoft.com/en-us/cpp/intrinsics/interlockedbittestandset-intrinsic-functions?view=msvc-160
  return _interlockedbittestandset(lock, 0);
#else
  // gcc and clang Implementation based on __sync_lock_test_and_set intrinsic
  //   see: https://gcc.gnu.org/onlinedocs/gcc-4.1.1/gcc/Atomic-Builtins.html
  return __sync_lock_test_and_set(lock, 1);
#endif
}

void
thread_helper_test_and_set_unlock(int *lock)
{
#ifdef _MSC_VER
  // cl.exe Implementation based on _interlockedbittestandreset
  //   see: https://docs.microsoft.com/en-us/cpp/intrinsics/interlockedbittestandreset-intrinsic-functions?view=msvc-160
  _interlockedbittestandreset(lock, 0);
#else
  // gcc and clang Implementation based on __sync_lock_release
  //   see: https://gcc.gnu.org/onlinedocs/gcc-4.1.1/gcc/Atomic-Builtins.html
  __sync_lock_release(lock);
#endif
}

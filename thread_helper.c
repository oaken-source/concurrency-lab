
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


// this is an include guard. It ensures that, if this header is
// included more than once, duplicate declarations are prvented.
#ifndef THREAD_HELPER_H
#define THREAD_HELPER_H

// define and declare different types, depending on the operating system
#ifdef _WIN32
// Windows Threads Declarations
#include <windows.h>

typedef HANDLE thread_helper_t;
typedef DWORD thread_helper_return_t;
#define thread_helper_empty_result 0

typedef CRITICAL_SECTION thread_helper_mutex_t;
#else
// POSIX Threads Declarations
#include <pthread.h>

typedef pthread_t thread_helper_t;
typedef void* thread_helper_return_t;
#define thread_helper_empty_result NULL

typedef pthread_mutex_t thread_helper_mutex_t;
#endif

// based on the definitions and declarations above, provide portable functions
// for thread creation and thread join
int thread_helper_create(thread_helper_t*, thread_helper_return_t(*)(void*), void*);
int thread_helper_join(thread_helper_t);

int thread_helper_mutex_init(thread_helper_mutex_t*);
int thread_helper_mutex_lock(thread_helper_mutex_t*);
int thread_helper_mutex_unlock(thread_helper_mutex_t*);

int thread_helper_test_and_set_lock(int*);
void thread_helper_test_and_set_unlock(int*);

#endif

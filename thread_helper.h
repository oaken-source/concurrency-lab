
// this is an include guard. It ensures that, if this header is included more
// than once, duplicate declarations are prevented.
#ifndef THREAD_HELPER_H
#define THREAD_HELPER_H

// define and declare different types, depending on the operating system
#ifdef _WIN32
// Declarations compatible with Windows Threads
#include <windows.h>

typedef HANDLE thread_helper_t;
typedef DWORD thread_helper_return_t;

typedef CRITICAL_SECTION thread_helper_mutex_t;
#else
// Declarations compatible with POSIX Threads
#include <pthread.h>

typedef pthread_t thread_helper_t;
typedef void* thread_helper_return_t;

typedef pthread_mutex_t thread_helper_mutex_t;
#endif

// based on the definitions and declarations above, declare portable functions
// for thread creation and thread join

typedef thread_helper_return_t(*thread_func_t)(void*);

// thread_helper_create
//
//   this function creates a new thread that starts executing the given
//   function immediately.
//
// parameters:
//
//   thread - this is a pointer to a thread_helper_t, it holds the reference to
//   the thread object in the used implementation.
//
//   thread_func - this is a pointer to a function that will be executed by the
//   created thread. The function takes a single argument of type void* and
//   returns a value of type void* when POSIX threads are used, and a value of
//   type DWORD, ant integer type, when Windows threads are used.
//
//   arg - this is the argument passed to the created threads, that is
//   accessible as the first argument to the thread_func function.
//
// return value:
//
//   the function returns 0 on success, and 1 otherwise.
int thread_helper_create(thread_helper_t *thread, thread_helper_return_t(*thread_func)(void*), void *arg);

// thread_helper_join
//
//   this function joins a previously created thread. This means that it waits
//   for the passed thread to terminate on its own, and then collects its
//   return value and frees all operating system resources related to the
//   thread object. This is important in long running programs because the
//   number of threads that the operating system can support is limited, and
//   terminated, but not joined threads continue to count towards this limit.
//
// parameters:
//
//   thread - this is a thread_helper_t, it holds the reference to the thread
//   object in the used implementation.
//
// return value:
//
//   the function returns 0 on success, and 1 otherwise.
int thread_helper_join(thread_helper_t thread);

// thread_helper_mutex_init
//
//   this function initializes a thread mutex, an object used to syncronize
//   access to a shared resource between multiple threads of the same process.
//
// parameters:
//
//   mutex - a pointer to a thread_helper_mutex_t that holds the reference to
//   the mutex object in the used implementation
//
// return value:
//
//   the function returns 0 on success, and 1 otherwise.
int thread_helper_mutex_init(thread_helper_mutex_t *mutex);

// thread_helper_mutex_lock
//
//   this function locks a mutex. To correctly syncronize access to a critical
//   section, the piece of code that performs access to the shared resource,
//   all threads that request entry to the critical section need to attempt to
//   lock the same mutex object, which is itself shared across all threads.
//   This function will be blocking for all but one of the requesting threads,
//   meaning that only one thread will be able to gain entry to the critical
//   section.
//
//   When the locking thread releases the mutex using
//   thread_helper_mutex_unlock, one of the waiting threads is resumed to gain
//   entry into the critical section. This process guarantees that at any given
//   time, only one thread can access the shared resource, preventing
//   corruption.
//
// parameters:
//
//   mutex - a pointer to a thread_helper_mutex_t that holds the reference to
//   the mutex object in the used implementation
//
// return value:
//
//   the function returns 0 on success, and 1 otherwise.
int thread_helper_mutex_lock(thread_helper_mutex_t *mutex);

// thread_helper_mutex_unlock
//
//   this function releases a mutex previously acquired by
//   thread_helper_mutex_lock. Releasing a mutex signals that the executing
//   thread is leaving its critical section.
//
// parameters:
//
//   mutex - a pointer to a thread_helper_mutex_t that holds the reference to
//   the mutex object in the used implementation
//
// return value:
//
//   the function returns 0 on success, and 1 otherwise
int thread_helper_mutex_unlock(thread_helper_mutex_t *mutex);

// thread_helper_test_and_set_lock
//
//   this function performs an atomic test_and_set instruction on the location
//   in memory identified by the given pointer. If the memory location is
//   shared between multiple threads of the same process, these threads can
//   syncronize access to a critical section by spin-locking with the
//   test_and_set function.
//
//   test_and_set uses hardware support to atomically, meaning without being
//   interrupted by concurrently or in parallel executing threads on different
//   CPUs, store the value of a memory location and immediately setting it to
//   1. This allows the implementation of spin-locks, where a number of threads
//   concurrently try to lock the memory location in a loop, and a thread that
//   reads a 0 will interpret this result as the signal to enter its critical
//   section. This will make sure that at any time in the execution, only one
//   thread will be executing inside the critical section, preventing data
//   corruption.
//
//   The downside of spin-locks when compared to mutexes is the active waiting,
//   which burns CPU cycles and drains unneccessary energy. As such, spin-locks
//   should be avoided wherever possible.
//
// parameters:
//
//   lock - a pointer to a valid memory location
//
// return value:
//
//   this function returns the previous value stored at the given memory
//   location
int thread_helper_test_and_set_lock(int *lock);

// thread_helper_test_and_set_unlock
//
//   this function unlocks a spin-lock previously locked by
//   thread_helper_test_and_set_lock by resetting the memory location
//   indentified by the given pointer to zero. Unlocking the spin-lock
//   indicates that the executing thread is leaving the critical section.
//
// parameters:
//
//   lock - a pointer to a valid memory location
void thread_helper_test_and_set_unlock(int *lock);

#endif

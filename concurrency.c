
#include <stdio.h>

// this custom header provides portable functions for Windows and POSIX threads
#include "thread_helper.h"

// define the number of concurrent threads to syncronize. Some implementations
// below will not support more than two therads, and will fall back to two if a
// larger number is used.
#define THREADS 2

// define the limit of the sum of consecutive integers to calculate
#define SUM_TO 1000000LLU

// this is a static shared variable, accessed by multiple threads concurrently
volatile unsigned long long res = 0;

// this thread function will access the shared resource without any protection.
// consequently, many write accesses will be lost and the result of the
// computation will be much lower than expected.
thread_helper_return_t
sum_unguarded (void *args)
{
  int id = *((int*)args);

  unsigned long i;
  for (i = id; i <= SUM_TO; i += THREADS)
    {
      /* enter critical section *********************************************/
      // no-op
      /**********************************************************************/

      res += i;

      /* leave critical section *********************************************/
      // no-op
      /**********************************************************************/
    }

  return 0;
}

// this thread function will take turns between two accessing threads. this
// will usually produce correct results, because mutual exclusion is
// guaranteed, but will lead to unreasonably long run times for concurrent and
// not parallel execution of two threads on the same CPU, since every thread
// will only execute one iteration per scheduling quantum, and will spend the
// rest of its quantum spin-locking and waiting for the other thread, which is
// blocked by scheduling, to have a go.
thread_helper_return_t
sum_turns (void *args)
{
  int id = *((int*)args);

  static volatile int turn = 0;

  unsigned long i;
  for (i = id; i <= SUM_TO; i += THREADS)
    {
      /* enter critical section *********************************************/
      while (turn != id);
      /**********************************************************************/

      res += i;

      /* leave critical section *********************************************/
      turn = id ^ 1;
      /**********************************************************************/
    }

  return 0;
}

// this thread function will attempt to guarantee mutual exclusion by having
// each thread attempting to enter the critical section raise a flag, and then
// waiting until the flag of the other thread is lowered. This can easily lead
// to deadlocks where both threads raise their flags at the same time, and no
// progress will be made towards the computation.
thread_helper_return_t
sum_flags (void *args)
{
  int id = *((int*)args);

  static volatile int flags[2] = { 0 };

  unsigned long i;
  for (i = id; i <= SUM_TO; i += 2)
    {
      /* enter critical section *********************************************/
      flags[id] = 1;
      while (flags[id ^ 1] == 1);
      /**********************************************************************/

      res += i;

      /* leave critical section *********************************************/
      flags[id] = 0;
      /**********************************************************************/
    }

  return 0;
}

// this thread function implements peterson's algorithm for two threads.
// Peterson's Algorithm solves the critical section problem correctly in
// theory, but will not work in practice due to hardware effects and the
// non-atomic nature of C statements.
thread_helper_return_t
sum_peterson (void *args)
{
  int id = *((int*)args);

  static volatile int flags[2] = { 0 };
  static volatile int turn = 0;

  unsigned long i;
  for (i = id; i <= SUM_TO; i += 2)
    {
      /* enter critical section *********************************************/
      flags[id] = 1; turn = id ^ 1;
      // __sync_synchronize();
      while ((flags[id ^ 1] == 1) && turn == (id ^ 1));
      /**********************************************************************/

      res += i;

      /* leave critical section *********************************************/
      flags[id] = 0;
      /**********************************************************************/
    }

  return 0;
}

// this thread function implements Dekker's Algorithm for two threads.
// Similarly to Peterson's Algorithm, this approach solves the problem in
// theory, but does not perform correctly in practice.
thread_helper_return_t
sum_dekker (void *args)
{
  int id = *((int*)args);

  static volatile int flags[2] = { 0 };
  static volatile int turn = 0;

  unsigned long i;
  for (i = id; i <= SUM_TO; i += 2)
    {
      /* enter critical section *********************************************/
      flags[id] = 1;
      while (flags[id ^ 1] == 1)
        if (turn == (id ^ 1))
          {
            flags[id] = 0;
            while (turn == (id ^ 1));
            flags[id] = 1;
          }
      /**********************************************************************/

      res += i;

      /* leave critical section *********************************************/
      turn = id ^ 1;
      flags[id] = 0;
      /**********************************************************************/
    }

  return 0;
}

// this is a helper function used by the Bakery algorithm below.
static int
bakery_max (volatile long long int *v, size_t n)
{
  size_t i;
  int res = 0;
  for (i = 0; i < n; ++i)
    if (v[i] > res)
      res = v[i];
  return res;
}

// this thread function implements Lamport's Bakery algorithm for two or more
// threads. This is the first of the software approaches that is implemented
// here that supports more than two threads. However, this approach does suffer
// from the same caveats as Peterson's algorithm and Dekker's algorithm and
// does not work correctly in practice.
//
// Additionally, it becomes clear that the runtime overhead of making decisions
// with these sorts of algorithms is unfealibly high, and does not scale well
// with the number of threads involved in the waiting queue.
thread_helper_return_t
sum_bakery (void *args)
{
  int id = *((int*)args);

  static volatile int choosing[THREADS] = { 0 };
  static volatile long long int num[THREADS] = { 0 };

  unsigned long i;
  for (i = id; i <= SUM_TO; i += THREADS)
    {
      /* enter critical section *********************************************/
      choosing[id] = 1;
      num[id] = bakery_max(num, THREADS) + 1;
      choosing[id] = 0;
      int j;
      for (j = 0; j < THREADS; ++j)
        {
          while (choosing[j] == 1);
          while ((num[j] != 0) && (num[j] < num[id] || (num[j] == num[id] && j < id)));
        }
      /**********************************************************************/

      res += i;

      /* leave critical section *********************************************/
      num[id] = 0;
      /**********************************************************************/
    }

  return 0;
}

// this thread function attempts to solve the critical section problem by using
// the atomic hardware instruction test_and_set. By delegating this problem
// from software to hardware, we solve the issues that the software based
// approaches had, and gain a scalable, working and efficient way to syncronize
// thread access to critical sections.
//
// However, the act of waiting for entrance to the critical section is still
// implemented through busy waiting, which will waste CPU resources when the
// other threads take a long time in their critical section.
thread_helper_return_t
sum_test_and_set (void *args)
{
  int id = *((int*)args);

  static int lock = 0;

  unsigned long i;
  for (i = id; i <= SUM_TO; i += THREADS)
    {
      /* enter critical section *********************************************/
      while (thread_helper_test_and_set_lock(&lock)) {
        while (lock);
      }
      /**********************************************************************/

      res += i;

      /* leave critical section *********************************************/
      thread_helper_test_and_set_unlock(&lock);
      /**********************************************************************/
    }

  return 0;
}

// a shared mutex for the thread function below
thread_helper_mutex_t mutex;

// this thread function syncronizes the critical section by using operating
// system APIs. These APIs internally syncronize the participating threads
// using hardware constructs, like test_and_set, but instead of doing busy
// waiting they will implement the waiting by making scheduling decisions,
// evacuating threads blocked by a locked mutex from the CPU and making space
// for other threads instead.
//
// This is usually the most efficient construct to use, and the easiest to get
// right without any caveats.
thread_helper_return_t
sum_semaphore (void *args)
{
  int id = *((int*)args);

  unsigned long i;
  for (i = id; i <= SUM_TO; i += THREADS)
    {
      /* enter critical section *********************************************/
      thread_helper_mutex_lock(&mutex);
      /**********************************************************************/

      res += i;

      /* leave critical section *********************************************/
      thread_helper_mutex_unlock(&mutex);
      /**********************************************************************/
    }

  return 0;
}

// this function is a blank space for you to experiment with your own
// solutions. Be creative, but remember that solutions only based in software
// have been shown above to fail in non-trivial ways.
thread_helper_return_t
sum_custom (void *args)
{
  int id = *((int*)args);

  unsigned long i;
  for (i = id; i <= SUM_TO; i += THREADS)
    {
      /* enter critical section *********************************************/
      // TODO!
      /**********************************************************************/

      res += i;

      /* leave critical section *********************************************/
      // TODO!
      /**********************************************************************/
    }

  return 0;
}

// the code below chooses the active guard type from the funtions above by
// checking the preprocessor macros passed in the GNUMakefile.
struct guard_type_t
{
  thread_func_t func;
  const char *name;
  size_t max_threads;
};

static const struct guard_type_t guard =
#if defined(HAVE_UNGUARDED)
  { sum_unguarded, "unguarded", 0 };
#elif defined(HAVE_TURNS)
  { sum_turns, "take turns", 2 };
#elif defined(HAVE_FLAGS)
  { sum_flags, "raise flags", 2 };
#elif defined(HAVE_PETERSON)
  { sum_peterson, "Peterson's Algorithm", 2 };
#elif defined(HAVE_DEKKER)
  { sum_dekker, "Dekker's Algorithm", 2 };
#elif defined(HAVE_BAKERY)
  { sum_bakery, "Bakery Algorithm (Lamport)", 0 };
#elif defined(HAVE_TEST_AND_SET)
  { sum_test_and_set, "test&set", 0 };
#elif defined(HAVE_SEMAPHORE)
  { sum_semaphore, "semaphore", 0 };
#elif defined(HAVE_CUSTOM)
  { sum_custom, "custom", 2 };
#endif

// this is the main function. Program execution begins here.
int
main (void)
{
  // prepare an array of thread objects, and an array of thread arguments
  thread_helper_t threads[THREADS] = { 0 };
  int args[THREADS] = { 0 };

  // initialize the shared mutex for the corresponding thread functions above
  thread_helper_mutex_init(&mutex);

  // limit the number of threads by the number of threads supported by the
  // selected guard type
  size_t nthreads = (guard.max_threads > 0 && guard.max_threads < THREADS) ? guard.max_threads : THREADS;
  printf("starting experiment \"%s\" with %zu threads\n", guard.name, nthreads);

  // create the threads. The threads will start executing immediately.
  size_t i;
  for (i = 0; i < nthreads; ++i)
    {
      args[i] = i;
      if (thread_helper_create(threads + i, guard.func, args + i) != 0)
        {
          perror("thread_helper_create");
          return 1;
        }
    }

  // join the threads. This blocks until the threads have terminated.
  for (i = 0; i < nthreads; ++i)
    if (thread_helper_join(threads[i]) != 0)
      {
        perror("thread_helper_join");
        return 1;
      }

  // print the result.
  //
  //   Observe how the result of the computetaion will for most of the guard
  //   types usually not add up to the expected value of n * (n-1) / 2. The
  //   actual result is unpredictable and appears random, even though it is not
  //   truly random.
  printf("sum is:        %20llu\n", res);
  printf("sum should be: %20llu\n", (SUM_TO * (SUM_TO + 1)) / 2);

  return 0;
}

/*
  trunner.h
  2014 JSK (kutani@projectkutani.com)

  Released to the public domain. See LICENSE for details.
*/
#ifndef _TRUNNER_H
 #define _TRUNNER_H

/// Opaque pointer to a trunner data structure
typedef struct _proc* proc;

/// A function that will operate on data passed to it as a void pointer
typedef int (*thrd_fnc)(void *ptr);

/// A function pointer def for manipulating mutexes
typedef void (*mutex_fnc)(void *ptr);

/// Add new data and function to run on the given processor
/*!
  Adds data to the queue held by prc. Expects a void pointer
  to the data and a function to which the data will be passed.

  An optional callback can be specified which will be run
  after fn returns 0.

  fn and callback should be functions that accept a single
  void pointer, and return an int.
*/
void run_on_thread(proc prc, void *data, thrd_fnc fn, thrd_fnc callback);

/// Main processor
/*!
   Processes all queued data in prc.

   Returns 1 if there was any queued data, and 0 only if there was
   no waiting data on function entry.
*/
int trunner_process(proc prc);

/// Create a new trunner
/*!
   Creates a new trunner proc, which will then use mutex
   (passed as a void pointer) for thread safety. lckfn, unlockfn,
   and freefn are functions the trunner will use to Lock, Unlock,
   and Free the mutex, respectively.

   Returns an opaque trunner proc pointer.
*/
proc trunner_create(void *mutex, mutex_fnc lckfn, mutex_fnc unlckfn, mutex_fnc freefn);

/// Destroys a passed trunner proc
/*!
  Uncaringly frees the data queue then frees the mutex given at
  trunner creation using freefn if freefn != NULL.
 */
void trunner_destroy(proc prc);

#endif

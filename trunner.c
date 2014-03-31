/*
  trunner.c
  2014 JSK (kutani@projectkutani.com)

  Released to the public domain. See LICENSE for details.
*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/// A function that will operate on data passed to it as a void pointer
typedef int (*thrd_fnc)(void *ptr);

/// A function pointer def for manipulating mutexes
typedef void (*mutex_fnc)(void *ptr);


/// A single trunner queue item
typedef struct _thrd_proc {
	void *data;              ///< Pointer to data
	thrd_fnc fnptr;          ///< Function pointer to pass *data to
	thrd_fnc callback;       ///< Callback function to pass *data to after fnptr returns 0
	struct _thrd_proc *next; ///< Pointer to next item in queue
} _thrd_proc;


/** \brief Core trunner processor data.
	
	Contains a singly-linked list of _thrd_proc items, a
	locking mutex, and the functions for using that mutex.

	From a user-standpoint, this is an opaque structure as
	it should never be modified directly.
*/
typedef struct _proc {
	_thrd_proc *first;  ///< Pointer to the first item
	_thrd_proc *last;   ///< Pointer to the last item
	void *lock;         ///< Pointer to a lockable mutex
	mutex_fnc lockfn;   ///< Function pointer used to lock the mutex
	mutex_fnc unlockfn; ///< Function pointer used to unlock the mutex
	mutex_fnc freefn;   ///< Function pointer used to free the mutex
} _proc;


typedef _proc* proc;


void
run_on_thread(proc prc, void *data, thrd_fnc fn, thrd_fnc callback) {
	_thrd_proc *p = malloc(sizeof(_thrd_proc));
	p->data = data;
	p->fnptr = fn;
	p->callback = callback;
	p->next = NULL;

	(prc->lockfn)(prc->lock);
	if(! prc->first) {
		prc->first = p;
		prc->last = p;
	} else {
		prc->last->next = p;
		prc->last = p;		
	}
	(prc->unlockfn)(prc->lock);
}

int
trunner_process(proc prc) {
	int ret = 1;
	(prc->lockfn)(prc->lock);

	if(! prc->first) {
		ret = 0;
		goto TRUN_EXIT;
	}

	_thrd_proc *t, *l=NULL, *p=prc->first;

	while(p) {
		if(! (p->fnptr)(p->data)) {
			// Run configured callback if we have one
			if(p->callback)
			  (p->callback)(p->data);

			if(p == prc->first)
				prc->first = p->next;
			else
				l->next = p->next;

			t = p;
			p = p->next;
			free(t);
		} else {
			l = p;
			p = p->next;
		}
	}

	TRUN_EXIT:
	(prc->unlockfn)(prc->lock);
	return ret;
}

proc
trunner_create(void *mutex, mutex_fnc lckfn, mutex_fnc unlckfn, mutex_fnc freefn) {
	proc prc = malloc(sizeof(_proc));
	memset(prc, 0, sizeof(_proc));
	prc->lock = mutex;
	prc->lockfn = lckfn;
	prc->unlockfn = unlckfn;
	prc->freefn = freefn;
	return prc;
}

void
trunner_destroy(proc prc) {
	_thrd_proc *p, *n;
	(prc->lockfn)(prc->lock);

	p = prc->first;
	while(p) {
		n = p->next;
		free(p);
		p = n;
	}

	(prc->unlockfn)(prc->lock);

	if(prc->freefn)
		(prc->freefn)(prc->lock);

	memset(prc, 0, sizeof(_proc));

	free(prc);
}

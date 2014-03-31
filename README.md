thread-runner
=============

This is a simple C library for managing cross-thread data
handling when context sensitivity requires that certain actions
only take place on certain threads.

This is primarily useful for programs that make use of OpenGL,
where only one thread is allowed access to the GL context. However,
it is general enough for any use case.

Core data structures are fully documented but are not accessible
outside of trunner.c; this opacity is intended to simplify usage.


## Compiling

thread-runner comes with a simple `Makefile` and a `config.mk` file
for setting build options. I recommend using these if you're going
to build it as a static library.

Easy mode is to pass just `make`, assuming your system has clang
installed.

If you want to auto-install the library and header to the appropriate
directories, call `make install`.  Please see `config.mk` for options
to control this.

A likely build command, without editing config.mk, might be:

`PREFIX=/usr/local make -e install`

Or, for 64-bit systems:

`BITS=64 PREFIX=/usr/local make -e install`

Though, I do recommend just editing config.mk if you're going to
do this.


### Doxygen

There is a Makefile target (`docs`) that will build Doxygen
documentation (html, latex, man) if your system has it.


## Usage

Either add trunner.c to your project or build it into a static
library, and include trunner.h where you want to use it.

Create a new trunner proc via `trunner_create()`.

`trunner_create()` requires a void pointer to a mutex as well as
pointers to lock, unlock, and free functions for the mutex. You
can optionally pass `NULL` to the `freefn` argument if you intend
to clean up the data yourself.

Your thread's loop should call `trunner_process()` with the proc
pointer given by `trunner_create()` as its only argument.  It will
return 1 if the proc's queue had any data pending, and 0 only if
there was no data at function entry.

Other threads should call `run_on_thread()` to pass data to the
thread runner.  It accepts a void pointer to the data to process
and two function pointers, `fn` and `callback`.  `fn` will be run
with `data` as its only argument. If it returns successfully (0) and
`callback` is not `NULL`, `callback` will also run on success.

After `fn` returns 0 (and optional `callback` has run) the queued
item will be removed.  If `fn` returns 1, it will stay in the queue
until the next time `trunner_process()` is called.

When finished, call `trunner_destroy()` with the proc pointer as its
only argument to clean up.


## Examples (using SDL mutexes)

### Create

`proc thrd_proc = trunner_create(SDL_CreateMutex(), SDL_LockMutex, SDL_UnlockMutex, SDL_DestroyMutex);`

### Add to Process Queue

`run_on_thread(thrd_proc, (void*)my_data, my_data_func, my_callback);`

### Run Processor

`trunner_process(thrd_proc);`

### Clean up

`trunner_destroy(thrd_proc);`


## Bugs

Probably. If you find and fix any, just shoot me a pull request
on github (github.com/kutani/thread-runner).


## License

Public domain. See LICENSE.
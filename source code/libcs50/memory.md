# Memory module

The `memory.c` file provides some replacements for  `malloc`, `calloc`, and `free`.  As in Lab 3, these variants keep track of the number of allocation and free operations, and can report on those numbers, which can ease debugging and testing.
They are only truly useful if you use their functions consistently throughout your code.

These functions introduce a new concept: `assert`.  The traditional notion of an `assert` function is to "assert that a given condition is true, and if not, to print an error message and exit the program."  In this case, I wanted a convenient way to test the pointers returning from `malloc` (and related calls); if NULL, which is an unexpected and rare occurrence, I want to print an error message and exit. 

> Such behavior is not really appropriate for general-purpose use, because crashing (exiting) the program is not behavior many systems or users will accept - but it will do for our purposes.

Here are two of the function prototypes (for details see `memory.h`):

```c
void *assertp(void *p, const char *message);
void *count_malloc_assert(const size_t size, const char *message);
```

The first is used by the second; indeed, the first line of `count_malloc_assert` is 

```
  void *ptr = assertp(malloc(size), message);
```

Notice that it acts like a pass-through function, when all is well.  The pointer coming from `malloc` is returned by `assertp` and is saved in the variable.  At that point it is assured to be non-NULL.  When `assertp` receives a NULL pointer, it prints a message to `stderr` and exits:

```c
void *
assertp(void *p, const char *message)
{
  if (p == NULL) {
    fprintf(stderr, "MALLOC FAILED: %s\n", message);
    exit (99);
  }
  return p;
}
```

The nice thing about these functions is that you can use `count_malloc_assert()` and know that it will either return a valid pointer, or not return at all.  This drastically simplifies error handling - because your program punts on the error and exits.  (Long-term, a better solution would let the application receive and recover from the error.)

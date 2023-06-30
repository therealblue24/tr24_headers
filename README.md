# therealblue24's C headers
single-file public domain stb-style C headers for C (and sometimes C++)

<a name="tr24_libs"></a>
library    | lastest version | category | Lines of Code | description | use for
--------------------- | ---- | -------- | --- | ----------------|-----------------------------------------------------
**[tr24_smartptr.h](tr24_smartptr.h)** | 1.03 | pointers  | 420 | smart pointers in C using witchcraft | C (C++ compat)
**[tr24_mutex.h](tr24_mutex.h)**       | 0.02 | threading | 61  | simple mutex implementation in C     | C/C++
**[tr24_async.h](tr24_async.h)**       | 0.02 | async in c | 220 | async futures and promises in C | C/C++
**[tr24_valid_ptr.h](tr24_valid_ptr.h)** | 0.01 | pointers | 69 | runtime pointer valididation | C

Total lines of code: **770**

# How to Use
Get the header, and then insert code like this:
```c
#define TR24_INSERTHEADERNAMEHERE_IMPL
#include "tr24_insertheadernamehere.h"
```
do the #define statement *once*. Exclude the #define statement if you want
to include the header without implementation. You can also define the macro `TR24_IMPL` if you want to implement for every header.

> Note: Lines are measured using the command `cloc`.
## OS Support
library    | Linux | MacOS | Windows | Caveats |
--------------------- | ---- | -------- | --- | ----------------|
**[tr24_smartptr.h](tr24_smartptr.h)** | Yes | Yes | No | Windows does not work as it does not have __sync_bool_and_compare_swap. Else, Pure *GNU* C.
**[tr24_mutex.h](tr24_mutex.h)** | Yes | Yes | Untested | Pure C, should work
**[tr24_async.h](tr24_async.h)** | Yes | Yes | No       | Uses pthreads. Unix only.
**[tr24_valid_ptr.h](tr24_valid_ptr)** | Yes | Yes | No | unistd! UNIX syscalls! unix only.

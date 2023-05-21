# therealblue24's C headers
single-file public domain stb-style C headers for C (and sometimes C++)

<a name="tr24_libs"></a>
library    | lastest version | category | Lines of Code | description | use for
--------------------- | ---- | -------- | --- | ----------------|-----------------------------------------------------
**[tr24_smartptr.h](tr24_smartptr.h)** | 1.02 | pointers  | 386 | smart pointers in C using witchcraft            | C
**[tr24_mutex.h](tr24_mutex.h)**       | 0.02 | threading | 61  | bare bones and simple mutex implementation in C | C/C++

Total lines of code: **447**

# How to Use
Get the header, and then insert code like this:
```c
#define TR24_INSERTHEADERNAMEHERE_IMPL
#include "tr24_insertheadernamehere.h"
```
do the #define statement *once*. Exclude the #define statement if you want
to include the header without implementation. You can also define the macro `TR24_IMPL` if you want to implement for every header.

> Note: Lines are measured using the command `cloc`.

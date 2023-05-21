# therealblue24's C headers
single-file public domain stb-style C headers for C (and sometimes C++)

<a name="tr24_libs"></a>
library    | lastest version | category | Lines of Code | description
--------------------- | ---- | -------- | --- | -------------------------------
**[tr24_smartptr.h](tr24_smartptr.h)** | 1.00 | pointers | 361 | smart pointers in C using witchcraft 
**[tr24_mutex.h](tr24_mutex.h)** | 0.01 | threading | 49 | bare bones and simple mutex implementation in C

Total lines of code: **410**

# How to Use
Get the header, and then insert code like this:
```c
#define TR24_INSERTHEADERNAMEHERE_IMPL
#include "tr24_insertheadernamehere.h"
```
do the #define statement *once*. Exclude the #define statement if you want
to include the header without implementation. You can also define the macro `TR24_IMPL` if you want to implement for every header.

> Note: Lines are measured using the command `cloc`.

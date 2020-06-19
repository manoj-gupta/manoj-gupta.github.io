---
layout: post
title:  "CGO - How to use C shared library code"
date:   2020-06-19 16:00:00 +0530
categories: 
  - Linux
  - Golang
---

I was working with CGO code and realized the need of keeping the C code in a shared library and using it from Go code. Here is a brief run down of how to go about it.

# C Shared Library Overview

Libraries are wonderful as it provides allows to share the existing compiled code by programmes. Here, we look at how to build C shared library, which I am going to use in Go program using CGO.

Create a folder $WS/libs/shared_test, where you can export WS to your working directory path.

Create three files needed to build shared library

__mmap_test.h__

```
#ifndef _MMAP_TEST_H_
#define _MMAP_TEST_H_

extern void mmap_main();

#endif  // _MMAP_TEST_H_
```

__mmap_test.c__

```
#include <stdio.h>

#include "mmap_test.h"

void mmap_main()
{
    puts("mmap_main: Inside shared library");
    return;
}
```

__main.c__

```
#include <stdio.h>

#include "mmap_test.h"

int main(void)
{
    puts("Driver Program - Testing shared library");
    mmap_main();
    return 0;
}
```

Here, *mmap_test.h* provides the library interface and *mmap_test.c* provides library function implementation. *main.c* is the driver program using *libmmap* library.

## Compiling the library

**Step 1:** Compile with position independent code (pic)

```
$ gcc -c -Wall -Werror -fpic mmap_test.c
```

This will create an object file ```mmap_test.o```

**Step 2:** Create shared library from object file

```
gcc -shared -o libmmap.so mmap_test.o
```

**Step 3:** Linking with the shared library

```
gcc -L$WS/libs/shared_test -Wall -o test_mmap main.c -lmmap
```

Note that you have to specify the library path using **-L** option, otherwise linker does not know where to find libmmap specified with **-l** option above (GCC assumes that all libraries start with lib and end with .so).

**Step 4:** Specify LD_LIBRARY_PATH to specify libary path at runtime

The output file created test_mmap can not be run just like that. The path where *libmmap.so* is stored need to be specified using LD_LIBRARY_PATH, unless it is copied to standard path.

```
export LD_LIBRARY_PATH=$WS/libs/shared_test:$LD_LIBRARY_PATH
./test_mmap
Driver Program - Testing shared library
mmap_main: Inside shared library
```

**Step 5** Automate using *Makefile*

```
# Makefile template for a shared library in C

CC = gcc  # C compiler
CFLAGS = -fPIC -Wall -Wextra -O2 -g  # C flags
LDFLAGS = -shared   # linking flags
RM = rm -f   # rm command
TARGET_LIB = ../libmmap.so  # target lib

SRCS = mmap_test.c # source files
OBJS = $(SRCS:.c=.o)

.PHONY: all
all: ${TARGET_LIB}

$(TARGET_LIB): $(OBJS)
	$(CC) ${LDFLAGS} -o $@ $^

$(SRCS:.c=.d):%.d:%.c
	$(CC) $(CFLAGS) -MM $< >$@

include $(SRCS:.c=.d)

.PHONY: clean
clean:
	-${RM} ${TARGET_LIB} ${OBJS} $(SRCS:.c=.d)
```

# Using Shared library using CGo

Let's see, how we can use *libmmap.so* from Go program using CGO.

Create a new file in same or different folder

__main.go__

```
package main

/*
#cgo CFLAGS: -I<change with WS>/libs/mmap_test/
#cgo LDFLAGS: -L<change with WS>/libs/ -lmmap

#include <stdio.h>

#include "mmap_test.h"

*/
import "C"

import (
	"fmt"
)

func main() {
	fmt.Println("Golang: shared library test")
	C.mmap_main()
}
```

Build and run program

```
$ go build main.go
$ ./main
Golang: shared library test
mmap_main: Inside shared library
```

# CGO: How to pass function pointer to C

Starting from Go 1.6 cgo has new rules [Reference](https://github.com/golang/go/issues/12416).

These rules are checked during the runtime, and if violated program crashes. So, it is not possible to pass a pointer to C code, if the memory to which it is pointing stores a Go function pointer. Fortunately, it is possible to woraround it by registering the function pointer and storing it in C code, probably using a mapping with index. Here, I show a basic example of storing single Go function pointer in C.

Add the following lines in 

__mmap_test.h__

```
typedef void (*Callback)(int arg1, void *arg2);
void setCallback(Callback func);
```

__mmap_test.c__

```
#include <string.h>

Callback g_fn;

void setCallback(Callback func) {
    g_fn = func;
}
```

In mmap_main() .. to test the callback

```
    if (g_fn != NULL) {
        g_fn(25, g_fn);
    }

```

Rebuild shared library

```
$ make clean; make
```

__main.c__

```
void InitCallback();
extern void test_callback(int arg1, void *arg2);

void test_callback(int arg1, void *arg2) {
    printf("%s called\n", __func__);
    printf("arg1: %d arg2: %p\n", arg1, arg2);
}

void InitCallback() {
    setCallback(test_callback);
}
```

In main() .. initialize the callback

```
    InitCallback();

```

Build and run program

```
$ gcc -L$WS/libs/shared_test -Wall -o test_mmap main.c -lmmap
$ ./test_mmap
Driver Program - Testing shared library
mmap_main: Inside shared library
test_callback called
arg1: 25 arg2: 0x558d9d2345ea
```

__main.go__

In CGO section

```
static inline void test_callback(int arg1, void *arg2) {
    printf("%s called\n", __func__);
    printf("arg1: %d arg2: %p\n", arg1, arg2);
}

static inline void InitCallback() {
    setCallback(test_callback);
}
```

In Go Code, main() function

```
	C.InitCallback()
```

Build and run program

```
$ go build main.go
$ ./main
Golang: shared library test
mmap_main: Inside shared library
test_callback called
arg1:25 arg2: 0x492640
```
# References

1. [Shared libraries with GCC on Linux](https://www.cprogramming.com/tutorial/shared-libraries-linux-gcc.html)
2. [Stack Overflow](https://stackoverflow.com/questions/37157379/passing-function-pointer-to-the-c-code-using-cgo#:~:text=Starting%20from%20Go%201.6%20cgo,not%20contain%20any%20Go%20pointers.&text=These%20rules%20are%20checked%20during,and%20if%20violated%20program%20crashes)
3. [Makefile Template for a Shared Library in C](https://www.topbug.net/blog/2019/10/28/makefile-template-for-a-shared-library-in-c-with-explanations/)
---
title: "Golang Programs - Static vs Dynamic Linking"
categories:
  - Golang
tags:
  - golang
last_modified_at: 20120-08-09T14:25:52-05:00
---

# Discovery

It is well known that Golang creates static binaries by default. But is that so?

I was taken by surprise to find out all the Golang binaries that I was using were using dynamic linking. This was easily verified by using *file* command on binary

```
$ file test.gobin
file test.gobin:      ELF 64-bit LSB executable, x86-64, version 1 (SYSV), dynamically linked (uses shared libs), not stripped
```

To find out the libaries being linked, use *ldd* command
```
$ ldd test.gobin
        linux-vdso.so.1 =>  (0x00007ffd4b7ed000)
        libpthread.so.0 => /lib64/libpthread.so.0 (0x00000032d6e00000)
        libc.so.6 => /lib64/libc.so.6 (0x00000032d6600000)
        /lib64/ld-linux-x86-64.so.2 (0x0000560e965ba000)
```
After searching on Google and going through many useful links, I could find out the reason to be usage of CGO. I was using CGO indirectly just by including the package "net".

As of this writing, there are two packages which uses CGO.
* net 
* os/user 

## References
* [Statically compiling Go programs](https://www.arp242.net/static-go.html)


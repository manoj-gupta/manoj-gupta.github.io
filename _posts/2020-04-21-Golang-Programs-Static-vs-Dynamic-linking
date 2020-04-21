---
layout: post
title:  "Golang Programs - Static vs Dynamic Linking"
date:   2020-04-21 14:00:00 +0530
categories: 
  - Golang
---
# Discovery

It is well known that Golang creates static binaries by default. But is that so?

I was taken by surprise to find out all the Golang binaries that we were using were dynamic. We were always trying to find out option to enable dynamic linking and here we have all the binaries having dynamic linking. This was easily verified by using *file* command on binary

```
$ file test.gobin
file test.gobin:      ELF 64-bit LSB executable, x86-64, version 1 (SYSV), dynamically linked (uses shared libs), not stripped
```

To further looking into the libaries being linked, use ldd command
```
$ ldd test.gobin
        linux-vdso.so.1 =>  (0x00007ffd4b7ed000)
        libpthread.so.0 => /lib64/libpthread.so.0 (0x00000032d6e00000)
        libc.so.6 => /lib64/libc.so.6 (0x00000032d6600000)
        /lib64/ld-linux-x86-64.so.2 (0x0000560e965ba000)
```
After searching on Google and going through many useful link, I could find out the reason was found to be usage of CGO, which I was using indirectly just by including the package "net".

As of this writing, there are two packages which uses CGO.
* net 
* os/user 

## References
* [Statically compiling Go programs](https://www.arp242.net/static-go.html)


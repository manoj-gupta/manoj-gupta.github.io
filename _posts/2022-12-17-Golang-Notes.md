---
title: "Golang Notes"
categories:
  - Golang
tags:
  - golang
last_modified_at: 2022-12-18T14:25:52-05:00
---

Following are my notes after reading resources mentioned in **Reference** section. This will serve as a single point of reference for me.

# Text and HTML Templates
A template is a string or file containing one or more portions enclosed in double braces, {{...}}, called *actions*. Most of the string is printed literally, but each action contains an expression in the template language for
* Printing values
* Selecting struct fields
* Calling functions and methods
* Expressing control flow such as *if-else* statements 
* Range loops
* Instantiating other templates

Good article explaing templates is at [Gopher Academy Blog](https://blog.gopheracademy.com/advent-2017/using-go-templates/)

# Concurrenent Programming

Golang enables two types of concurrent programming: `goroutines` and `channels`.

If `goroutines` are the activities of a concurrent `go` program, `channels` are the connections between them.

## goroutines
Each concurrently excuting activity is gcalled a `goroutine`.

New goroutines are created by `go` statement.
```
    f()     // call f(); wait for it to return
    go f()  // create a new goroutine that calls f(); don't wait
```

Other than by returning from main or exiting the program, there is no programmatic way for one `goroutine` to stop another.

## channels
A channel is a mechanism that lets one `goroutine` send values to another `goroutine`. 

To create a channel, we use built-in `make` function.
```
    ch := make(chan int)        // `ch` has type `chan int`
```

A channel is a reference to data structure created by `make`. Two channels of the same type may be compared using ==. The comparison is true if both are references to the same channel data structure. A channel may also be compared to `nil`.

`channel` support three operations: `send`, `receive` and `close`
```
    ch <- x     // send statement
    x = <-ch    // receive expression in assignment statment
    <-ch        // receive statement, result is discarded
    close(ch)   // close operation
```

A `send` statement transmits a value from one goroutine, through the channel, to another goroutine executing a corresponding `receive` expression.

A `close` operation sets a flag indicating no more values can be sent to this channel; subsequent attempts to send will panic. Receive operations on a closed channel yield the values that have been sent until no more values are left; any receive operations thereafter complete immediately and yield the zero value of the channel’s element type.

### Unbuffered and Buffered Channels
A channel created with a simple call to make is called an `unbuffered` channel, but make accepts an optional second argument, an integer called the channel’s capacity. If the capacity is non-zero, make creates a `buffered` channel.

```
    ch = make(chan int)     // unbuffered channel
    ch = make(chan int, 0)  // unbuffered channel
    ch = make(chan int, 3)  // buffered channel with capacity 3
```

A send operation on an **unbuffered channel** blocks the sending `goroutine` until another `goroutine` executes a corresponding receive on the same channel, at which point the value is transmitted and both `goroutines` may continue. Conversely, if the receive operation was attempted first, the receiving `goroutine` is blocked until another goroutine performs a send on the same channel.

A **buffered channel** has a queue of elements. The queue’s maximum size is determined when it is created, by the capacity argument to `make`.

### Unidirectional Channel Types
To document channel usage intent and prevent misuse, the Go type system provides `unidirectional` channel types that expose only one or the other of the `send` and `receive` operations.
```
    chan<- int   // send-only channel of int, allows sends only
    <-chan int   // receive-only channel of int, allows receives only
```
Violations of this discipline are detected at compile time.

Since the `close` operation asserts that no more sends will occur on a channel, only the sending goroutine is in a position to call it, and for this reason it is a compile-time error to attempt to close a receive-only channel.

## Multiplexing with select

General form of a `select` statement is shown below. Like a `switch` statement, it has a number of cases and an optional default. Each case specifies a *communication* (a send or receive operation on some channel) and an associated block of statements.
```
    select {
    case <-ch1:
    // ...
    case x := <-ch2:
    // ...use x...
    case ch3 <- y:
    // ...
    default:
    // ...
    }
```

A select waits until a communication for some case is ready to proceed. It then performs that communication and executes the case’s associated statements; the other communications do not happen. A select with no cases, `select{}`, waits forever.

## Cancellation

There is no way for one goroutine to terminate another directly, since that would leave all its shared variables in undefined states. But what if we need to cancel two goroutines, or an arbitrary number?

For cancellation, we need is a reliable mechanism to broadcast an event over a channel so that many
goroutines can see it as it occurs and can later see that it has occurred. 

As we know, after a channel has been closed and drained of all sent values, subsequent receive operations proceed immediately, yielding zero values. We can exploit this to create a broadcast mechanism: don’t send a value on the channel, `close` it.

To to this, we create a cancellation channel on which no values are ever sent, but whose closure indicates that it is time for the program to stop what it is doing. All `goroutines` which need to be communicated of an event does `receive` on this channel. The `goroutine` which need to broadcast event simply `close` this channel.  

# Golang Programs - Static vs Dynamic Linking

It is well known that Golang creates static binaries by default. I was surprised to find out all the Golang binaries that I was using were using dynamic linking. This was easily verified by using *file* command on binary

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
The reason is the usage of CGO indirectly just by including the package "net".

There are two packages which uses CGO.
* net 
* os/user 


# References

* The Go Programming Language, Alan Donovan, B. Kernighan
* [Gopher Academy Blog](https://blog.gopheracademy.com)
* [Internals of Go channels](https://shubhagr.medium.com/internals-of-go-channels-cf5eb15858fc)
* [Statically compiling Go programs](https://www.arp242.net/static-go.html)


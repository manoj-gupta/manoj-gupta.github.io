---
title: "Golang Notes"
categories:
  - Golang
tags:
  - golang
last_modified_at: 2022-12-18T14:25:52-05:00
---

Golang is an open-source, compiled, and statically typed programming language designed by Google. It is built to be simple, high-performing, readable, and efficient.

* TOC
{:toc}

# Golang Types

Data types are categorized into four categories as shown below.

![image-center]({{ '/images/golang/golang_types.png' | absolute_url }}){: .align-center}


# Concurrent programming

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

# Type, Interface and Reflection

Understanding *Type*, *Interface* and *Reflection* should be done together because *interface* is a special *type* and *reflection* is built on *types*.

Golang is **statically typed** language. Every variable has a static type, that is, exactly one type known and fixed at compile time: `int`, `float32`, `*UserType`, `[]byte` etc.

## Interfaces
One important category of type is `interface` types, which represent **fixed sets of methods**. Any type that *implements* these methods is said to *implement the interface*.

Interfaces are **statically typed**: a variable of interface type always has the same static type, and even though at run time the value stored in the interface variable may change type, that value will always satisfy the interface. An extremely important example of an interface type is the **empty interface** `interface{}` which represents the empty set of methods and is satisfied by any value.

A variable of interface type stores a pair: the **concrete value** assigned to the variable, and that **value’s type descriptor**. To be more precise, the value is the underlying concrete data item that implements the interface and the type describes the full type of that item.

Let's defined an interface `shape`, which has two methods `Area` and `Perimeter` that accepts no arguments and return `float64` value.

```
// Shape is an interface
type Shape interface {
    Area() float64
    Perimeter() float64
}
```

Since the `interface` is a type just like a `struct`, we can create a variable of its type. Let's create a variable `s` of type interface `Shape` and print it's type and value.
```
var s shape
fmt.Printf("Type of s is %T\n", s)
fmt.Printf("Value of s is %v\n", s)
```

It has **zero value** and **nil type** as variable `s` is just declard of type `Shape` but not assigned any value.
```
Type of s is <nil>
Value of s is <nil>
```

Any type that implements these methods (with exact method signatures) will also implement `Shape` interface. Let's define a new type `Rectangle` which implements `Shape` interface
```
// Rectangle is a type which implements `Shape` interface
type Rectangle struct {
	length  float64
	breadth float64
}

// Area of Rectangle
func (r Rectangle) Area() float64 {
	return r.length * r.breadth
}

// Perimeter of Rectangle
func (r Rectangle) Perimeter() float64 {
	return 2 * (r.length + r.breadth)
}
```
Now, create of variable of type `Rectangle` and assing it to `s` and print type and value of `s`.
```
var s Shape
s = Rectangle{1, 2}
fmt.Printf("Type of s is %T\n", s)
fmt.Printf("Value of s is %v\n", s)
```

We can see that, dynamic type of `s` is now `main.Rectangle` and dynamic value of s is the value of the struct Rect which is `{1 2}`.
```
Type of s is main.Rectangle
Value of s is {1 2}
```

Two **interface variables are equal** if both of these variable holds the same *dynamic type* and *dynamic value*. Let's define one more variable `r` of `Rectangle` type and having same value `{1, 2}`

```
var s Shape
s = Rectangle{1, 2}
r := Rectangle{1, 2}
fmt.Printf("Type of s is %T\n", s)
fmt.Printf("Value of s is %v\n", s)
fmt.Println("s == r is ", s == r)
```

They are equal as shown below
```
Type of s is main.Rectangle
Value of s is {1 2}
s == r is  true
```

## Reflection

**interface value => reflection object**

*Reflection* is a mechanism to examine the *type* and *value* pair stored inside an interface variable. The [reflect package](https://go.dev/pkg/reflect/) provides two types: [Type](https://go.dev/pkg/reflect/#Type) and [Value](https://go.dev/pkg/reflect/#Value), which provide access to the content of an interface variable. Also, two functions: `reflect.TypeOf` and `reflect.ValueOf`, retrieve `reflect.Type` and `reflect.Value` pieces out of an interface value.

Important methods in `reflect.Type` and `reflect.Value`:
* `reflect.Value` has a `Type` method that returns the `Type` of a `reflect.Value`.
* Both `reflect.Type` and `reflect.Value` have a `Kind` method that returns a constant indicating what sort of item is stored.

**reflection object => interface value**

Like physical reflection, reflection in Go generates its own inverse.

`Interface` method returns interface value from `reflect.Value`. This method packs the *type* and *value* information back into an interface representation and returns the result:
```
// Interface returns v's value as an interface{}.
func (v Value) Interface() interface{}
```

**The value must be settable t0 modify a reflection object**

*Settability* is a property of a reflection `Value`, and not all reflection `Values` have it. It’s the property that a reflection object can modify the **actual storage** that was used to create the reflection object. Settability is determined by whether the reflection object holds the original item.

The `CanSet` method of Value reports the settability of a `Value`.

### Using reflection to modify the fields of a structure

We create the reflection object with the **address** of the struct because we’ll want to modify it later. Then we set `typeOfT` to its type and iterate over the fields using method calls. Note that we extract the names of the fields from the struct type, but the fields themselves are regular `reflect.Value` objects.
```
type Rectangle struct {
	Length  int
	Breadth float64
}

t := Rectangle{5, 6.6}
s := reflect.ValueOf(&t).Elem()
typeOfT := s.Type()
for i := 0; i < s.NumField(); i++ {
    f := s.Field(i)
    fmt.Printf("%d: %s %s = %v\n", i,
        typeOfT.Field(i).Name, f.Type(), f.Interface())
}
```

Output:
```
0: Length int = 5
1: Breadth float64 = 6.6
```

**Note:** The field names of `Rectangle` are upper case (exported) because only exported fields of a struct are settable.

Because `s` contains a settable reflection object, we can modify the fields of the structure.
```
s.Field(0).SetInt(77)
s.Field(1).SetFloat(99.8)
```

Output:
```
fmt.Println("t is now", t)
```

If we modified the program so that `s` was created from `t`, not `&t`, the calls to `SetInt` and `SetFloat` would fail as the fields of `t` would not be settable.

# JSON

JSON is an encoding of JavaScript values—strings, numbers, booleans, arrays, and objects —as Unicode text. It’s an efficient yet readable representation for the basic data types of arrays, slices, structs, and maps.

Go has excellent support for encoding and decoding JavaScript Object Notation (JSON) format, provided by the standard library package `encoding/json`. 

* Converting a Go data structure to JSON is called `marshaling`. Marshaling is done by `json.Marshal`. Marshal produces a byte slice containing a very long string with no white space. For human consumption, a variant called `json.MarshalIndent` produces neatly indented output.
* The inverse operation to `marshaling`, decoding JSON and populating a Go data structure, is called `unmarshaling`, and it is done by `json.Unmarshal`.
* Another useful function is `json.Indent`. It can be used to produced indented output from an existing JSON text. An example usage is shown below.

```
func main() {
	input := []byte(`{"login": "sample-id","id": 12345678}`)
	fmt.Println(input)
	fmt.Println(string(prettify(input)))
}

func prettify(input []byte) []byte {
	var prettyJSON bytes.Buffer
	error := json.Indent(&prettyJSON, input, "", "\t")
	if error != nil {
		fmt.Println("JSON parse error: ", error)
		return []byte{}
	}
	return prettyJSON.Bytes()
}
```

# Text and HTML Templates

A template is a string or file containing one or more portions enclosed in double braces, `{` `{`...`}` `}`, called *actions*. Most of the string is printed literally, but each action contains an expression in the template language for
* Printing values
* Selecting struct fields
* Calling functions and methods
* Expressing control flow such as *if-else* statements 
* Range loops
* Instantiating other templates

Good article explaing templates is at [Gopher Academy Blog](https://blog.gopheracademy.com/advent-2017/using-go-templates/)
# Miscellaneous

## Static vs Dynamic Linking

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

## Number literal prefixes

[Go 1.13](https://go.dev/doc/go1.13) allowed number literal prefixes, which allowed **digit seperators** among other things. The digits of any number literal may now be separated (grouped) using underscores, such as in `1_000_000`, `0b_1010_0110`, or `3.1415_9265`. An underscore may appear between any two digits or the literal prefix and the first digit.

# References

* The Go Programming Language, Alan Donovan, B. Kernighan
* [Gopher Academy Blog](https://blog.gopheracademy.com)
* [Go Data Structures: Interfaces](https://research.swtch.com/interfaces)
* [The Go Blog](https://go.dev/blog/laws-of-reflection)



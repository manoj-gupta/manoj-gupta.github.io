---
layout: post
title:  "Golang Testing"
date:   2020-04-05 8:00:00 +0530
categories: Golang
---

# Overview
Testing a.k.a. automated testing, is the practice of writing small programs that check that the code under test behaves as expected for certain inputs, which are either carefully chosen to exercise certain features or randomised to ensure broad coverage.

Go’s approach relies on one command **go test**, and set of conventions for writing test functions that **go test** can run. The lightweight mechanism is effective for pure testing, and it extends naturally to benchmarks and systematic examples for documentation.

# The go test Tool
The **go test** subcommand is a test driver for Go packages that are organized using certain conventions. In a package directory, files whose name ends with **_test.go** are not part of packet built by **go build** but are a part of it when built by **go test**.

Within test files, three types of functions are treated differently:
* **tests**: A test function whose name begins with *Test*, exercise program logic for correct behavior.
* **benchmarks**: A benchmark function whose name begins with *Benchmark*, measures the performance of some operation.
* **examples**: An example function whose name begins with *Example*, provides machine-checked documentation.

The *go test* scans the *_test.go* files for these special functions, generates a temporary *main package* that calls them all in the proper way, builds and runs it, reports the results and then cleans up.

# Test Functions
Each test file must import the testing package. Test functions have the following signature:
```
Func TestName(t *testing.T) {
                           // …
}
```
Test function names must begin with **Test**; the optional suffix Name must being with a capital letter. The **t** parameter provides methods for reporting test failures and logging additional information.

# Example
Following is an example of validating palindrome using a function *IsPalindrome* that reports if the string reads the same forward and backwards.

__golang/src/palindrome/palindrome.go__

```
package palindrome
 
import "unicode"

// IsPalindrome ... reports whether s reads the same forward and backward
// Unicode implementation
func IsPalindrome(s string) bool {
    var letters []rune
    for _, r := range s {
        if unicode.IsLetter(r) {
            letters = append(letters, unicode.ToLower(r))
        }
    }
    for i := range letters {
        if letters[i] != letters[len(letters)-i-1] {
            return false
        }
    } 
    return true
}
```
In the same directory, file *palindrome_test.go* contains functions to test the functionality. It shows example usage of *test* and *benchmark* functions.  

*TestPalindromeTabular* shows a table-drive methodology, which is very common in Go. It is easy to add new entries as needed. Special care should be taken to print enough information to identify the failed case. In case of result mismatch, *t.Errorf* is used to display additional information.

__golang/src/palindrome/palindrome_test.go__
```
package palindrome
 
import "testing"
 
func TestPalindromeTabular(t *testing.T) {
    var tests = []struct {
        input string
        want  bool
    }{
        {"", true},
        {"a", true},
        {"aa", true},
        {"ab", false},
        {"kayak", true},
        {"detartrated", true},
        {"palindrome", false},
    }
 
    for idx, test := range tests {
        got := IsPalindrome(test.input)
        if got != test.want {
            t.Errorf("Test%d: input:%q got:%v not same as want:%v", idx, test.input, got, test.want)
        }
    }
}
 
func TestFrenchPalindrome(t *testing.T) {
    if !IsPalindrome("été") {
        t.Error(`IsPalindrome("été") = false`)
    }
}
 
func TestCanalPalindrome(t *testing.T) {
    input := "A man, a plan, a canal: Panama"
    if !IsPalindrome(input) {
        t.Errorf(`IsPalindrome(%q) = false`, input)
    }
}
 
func TestCanalNotPalindrome(t *testing.T) {
    input := "A man a plan, a canal: anama"
    if IsPalindrome(input) {
        t.Errorf(`IsPalindrome(%q) = false`, input)
    }
}
 
func BenchmarkIsPalindrome(b *testing.B) {
    for i := 0; i < b.N; i++ {
        IsPalindrome("A man, a plan, a canal: Panama")
    }
}
```

Simplest way to run these test is to execute
```go test```

The **-v** flag prints the name and execution time of each test in the package 
```go test -v```

The **-run** flag takes regular expression as argument to run only those tests whose name matches the regular expression.
```go test -run=”French|Canal”```

# Coverage
The degree to which a test suite exercise the package under test is called the test’s coverage. Statement coverage is the simplest and most widely used heuristic. 

Go **cover** tool, which is integrated into *go test*, is used to measure coverage and help identify gaps in the tests. Following command can be used to find the coverage:
```
go test -cover
```

Following command is an enhanced version of *go test -cover*. It renders a HTML page that visualises line-by-line coverage of each affected *.go* file. The first half ```go test -coverprofile=cover.out``` runs ```go test -cover``` under the hood and saves the output to the file *cover.out*. The second half ```go tool cover -html=cover.out``` generates a HTML page in a new browser window, based on the contents of *cover.out*.
```
go test -coverprofile=cover.out && go tool cover -html=cover.out
```

# Benchmark Functions
Benchmarking is the practice of measuring the performance of a program on a fixed workload. A **Benchmark** function looks like a *test* function but with the *Benchmark* prefix and a *testing.B* parameter, which provides performance measurement related additional functions.

To run benchmark functions:
```
go test -bench=.
```

The **-benchmem** command-line flag will include memory allocation statistics in the report
```
go test -bench=. -benchmem
```

# Tips

__Command line arguments__
One of the interesing problem is to pass comamnd line arguments to the code under test. This can be done easily by setting *os.args* variable as shown below:

__golang/src/args/args.go__
```
package main

import (
	"flag"
	"fmt"
)

func main() {
	passArguments()
}

func passArguments() string {
	user := flag.String("user", "root", "Username for the server")
	flag.Parse()
	fmt.Printf("Username is %q\n", *user)

	userString := *user
	return userString
}
```

__golang/src/args/args_test.go__
```
package main

import (
	"os"
	"testing"
)

func TestArgs(t *testing.T) {
	expected := "one"

	// save and restore global variable os.Args
	oldArgs := os.Args
	defer func() { os.Args = oldArgs }()

	os.Args = []string{"args", "-user=one"}

	actual := passArguments()

	if actual != expected {
		t.Errorf("Test failed, expected: '%s', got:  '%s'", expected, actual)
	}
}
```

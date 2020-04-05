---
layout: post
title:  "Golang Testing"
date:   2020-04-05 8:00:00 +0530
categories: Golang
---

# Overview
Testing a.k.a. automated testing, is the practice of writing small programs that check that the code under test behaves as expected for certain inputs, which are either carefully chosen to exercise certain features or randomised to ensure broad coverage.

Go’s approach relies on one command *go test*, and set of conventions for writing test functions that *go test* can run. The lightweight mechanism is effective for pure testing, and it extends naturally to benchmarks and systematic examples for documentation.

# The go test Tool
The *go test* subcommand is a test driver for Go packages that are organized using certain conventions. In a package directory, files whose name ends with *_test.go* are not part of packet built by *go build* but are a part of it when built by *go test*.



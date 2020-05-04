---
layout: post
title:  "Golang Mocking"
date:   2020-05-04 8:00:00 +0530
categories: Golang
---

# Overview
Golang has a built-in testing framework provided by the testing package (go test), described my earlier post [Golang Teting] (https://manoj-gupta.github.io/golang/2020/04/05/Golang-Testing.html). It makes writing unit test cases very easy, where all interface are writting in the code under test. But how do you write more complicated tests where third-party library or interaction with external entity (e.g. kernel file system, socket, http server) is needed.

This can be achieved by mocking the interface using [GoMock](https://github.com/golang/mock), which integrates well with Go's built-in testing package.

# Installation
Once you have installed Go, install the mockgen tool.

To get the latest released version use:

```
GO111MODULE=on go get github.com/golang/mock/mockgen@latest
```

For older method without Go module using **"go get"**

```
go get github.com/golang/mock/gomock
go get github.com/golang/mock/mockgen
```

Verify that the ```mockgen``` binary is installed by running

```
$GOPATH/bin/mockgen
```

This should print usage information.

# Usage


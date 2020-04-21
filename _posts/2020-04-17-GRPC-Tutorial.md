layout: post
title:  "GRPC Tutorial"
date:   2020-04-17 11:00:00 +0530
categories: Golang
---

# Overview
Google RPC

# Google Protocol Buffers
Google Protocol buffer

## Installation
To install protobuf

In order to generated code from protocol buffers definition files, do the following:

* Download and install protoc compiler from official [website] (https://github.com/google/protobuf).

For Golang users on OSX
From the [download section] (https://github.com/protocolbuffers/protobuf/releases) take protoc-$VERSION-osx.zip (e.g. protoc-3.11.4-osx-x86_64.zip), unzip it and follow the instruction in readme.txt


* Add the location of protoc binary file into PATH environment variable so that you can invoke protoc compiler from any location.
* Install the protoc plugin for your language. For Go, run the go get command to install the protoc plugin for Go:

```
go get -u github.com/golang/protobuf/proto
go get -u github.com/golang/protobuf/protoc-gen-go
```

# GRPC

## Installation

# Resources
* [GRPC Official Website - Go Quick Start](https://www.grpc.io/docs/quickstart/go/)

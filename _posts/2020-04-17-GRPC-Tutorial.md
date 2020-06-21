---
layout: post
title:  "GRPC Tutorial"
date:   2020-04-17 11:00:00 +0530
categories: Golang
---

# Overview
I will be using Golang in this post. Let's install the software needed. There are equivalent commands to try on Raspberry-PI.

# Multipass

Create a new instance ```grpc``` and open a shell into this instance using multipass as follows:

```
$ multipass launch -n grpc
$ multipass shell grpc
...
...
ubuntu@grpc:~$
```

# Install Golang

Download latest tar for your platform from official site

```
wget https://dl.google.com/go/go1.14.4.linux-amd64.tar.gz (Ubuntu x86-64)
wget https://dl.google.com/go/go1.14.3.linux-armv6l.tar.gz (Raspberry PI)
```

Untar the tar file to ```/usr/local```

```
sudo tar -C /usr/local -xzf go1.14.4.linux-amd64.tar.gz (Ubuntu x86-64)
sudo tar -C /usr/local -xzf go1.14.3.linux-armv6l.tar.gz (Raspberry PI)
rm go1.14.4.linux-amd64.tar.gz (Ubuntu x86-64)
rm go1.14.3.linux-armv6l.tar.gz (Raspberry PI)
```

Set Golang paths in ```~/.profile```

```
export PATH=$PATH:/usr/local/go/bin
export GOPATH=$HOME/golang
```

Build directory to work

```
mkdir golang
mkdir golang/src
```

Logout and login back to shell to verify Go is installed.

```
ubuntu@grpc:~$ go version
go version go1.14.4 linux/amd64
```

# Google Protocol Buffers
Google Protocol buffer

## Installation
To install protobuf

In order to generated code from protocol buffers definition files, do the following:

* Download and install protoc compiler from official [website](https://github.com/google/protobuf). From the [download section](https://github.com/protocolbuffers/protobuf/releases) take ```protoc-$VERSION-osx.zip```. For example:

1. **For Golang users on OSX** : ```protoc-3.11.4-osx-x86_64.zip```
2. **For Golang users on Linux**: ```protoc-3.12.3-linux-x86_64.zip```

```
ubuntu@grpc:~$ wget https://github.com/protocolbuffers/protobuf/releases/download/v3.12.3/protoc-3.12.3-linux-x86_64.zip
ubuntu@grpc:~$ PROTOC_ZIP=protoc-3.12.3-linux-x86_64.zip
ubuntu@grpc:~$ sudo unzip -o $PROTOC_ZIP -d /usr/local bin/protoc
ubuntu@grpc:~$ sudo unzip -o $PROTOC_ZIP -d /usr/local 'include/*'
ubuntu@grpc:~$ rm -f $PROTOC_ZIP
```

* Add the location of protoc binary file into PATH environment variable so that you can invoke protoc compiler from any location. Also, you may have to add execute permission for all.

```
ubuntu@grpc:~$ export PATH=$PATH:/usr/local/bin/
ubuntu@grpc:~$ sudo chmod a+x /usr/local/bin/protoc
```

* Install the protoc plugin for your language. For Go, run the go get command to install the protoc plugin for Go:

```
go get -u github.com/golang/protobuf/proto
go get -u github.com/golang/protobuf/protoc-gen-go
```

Make sure that $GOPATH/bin is on your environment path so that you can use the protoc tool.

```
export PATH=$PATH:$GOPATH/bin
```

# GRPC

## Introduction

gRPC is a modern, open source remote procedure call (RPC) framework that can run anywhere.

RPC has been there from long time, used for something that we use within distributed systems that allow us to communicate between applications. More specifically, it allows us to expose methods within our application that we want other applications to be able to invoke.

It is similar to REST API that exposes functionality within app to other apps using a HTTP connection. Though REST and gRPC are similar, there are some basic differences as well:

* gRPC uses HTTP/2 vs. REST which uses HTTP 1.1
* gRPC uses the protocol buffer data format vs. JSON data format that typically used within REST APIs
* With gRPC you can use HTTP/2 capabilities 
  * server-side streaming
  * client-side streaming 
  * bidirectional-streaming

## Building GRPC server in Golang

**Step 1:** To being, here is a simple server Go program that listens on TCP Port (e.g. 9000) for incoming connections.

__gprc-demo/server.go__

```
package main

import (
	"log"
	"net"
)

func main() {
	_, err := net.Listen("tcp", ":9000")
	if err != nil {
		log.Fatalf("failed to listen: %v", err)
	}
}
```

**Step 2:** Import *grpc* package from *golang.org* to create a gRPC server and start serving. 

__gprc-demo/server.go__

```
package main

import (
	"log"
	"net"

	"google.golang.org/grpc"
)

func main() {

	lis, err := net.Listen("tcp", ":9000")
	if err != nil {
		log.Fatalf("failed to listen: %v", err)
	}

	grpcServer := grpc.NewServer()

	if err := grpcServer.Serve(lis); err != nil {
		log.Fatalf("failed to serve: %s", err)
	}
}
```

Note, you have to get *grpc* package if not already present.

```
ubuntu@grpc: go get google.golang.org/grpc
```

**Step 3:** Add Functionality

Let's add a chat service to this server. Service is defined in a **chat.proto** file using protobuf, exposing single service *ChatService* to be called by gRPC client.

__gprc-demo/chat.proto__

```
syntax = "proto3";
package chat;

message Message {
  string body = 1;
}

service ChatService {
  rpc SayHello(Message) returns (Message) {}
}
```

**Step 4:** Generate Go specific gRPC code for service using the **protoc** tool

```
ubuntu@grpc:~/golang/src/gprc-demo$ mkdir chat
ubuntu@grpc:~/golang/src/gprc-demo$ protoc --go_out=plugins=grpc:chat chat.proto

ubuntu@grpc:~/golang/src/gprc-demo$ ls -R
.:
chat  chat.proto  server.go

./chat:
chat.pb.go
```

**Step 5:** Use the generated code in *server.go* as follows:

* Implement Chat interface

```
type ChatServer struct {
}

func (s *ChatServer) SayHello(ctx context.Context, in *chat.Message) (*chat.Message, error) {
	log.Printf("Receive message body from client: %s", in.Body)
	return &chat.Message{Body: "Hello From the Server!"}, nil
}
```

* Use the chat service with *grpcSever*

```
	// register chat service with gRPCServer
	chat.RegisterChatServiceServer(grpcServer, &ChatServer{})
```

Final server code is:

__gprc-demo/server.go__

```
package main

import (
	"fmt"
	"log"
	"net"

	"golang.org/x/net/context"

	"google.golang.org/grpc"
	"gprc-demo/chat"
)

type ChatServer struct {
}

func (s *ChatServer) SayHello(ctx context.Context, in *chat.Message) (*chat.Message, error) {
	log.Printf("Receive message body from client: %s", in.Body)
	return &chat.Message{Body: "Hello From the Server!"}, nil
}

func main() {

	fmt.Println("Go GRPC Tutorial!")

	lis, err := net.Listen("tcp", ":9000")
	if err != nil {
		log.Fatalf("failed to listen: %v", err)
	}

	grpcServer := grpc.NewServer()

	// register chat service with gRPCServer
	chat.RegisterChatServiceServer(grpcServer, &ChatServer{})

	if err := grpcServer.Serve(lis); err != nil {
		log.Fatalf("failed to serve: %s", err)
	}
}
```

**Step 6:** Implement the client

After testing that server is up and running, let's write the client code.

__gprc-demo/client.go__

```
package main

import (
	"log"

	"golang.org/x/net/context"
	"google.golang.org/grpc"

	"gprc-demo/chat"
)

func main() {

	var conn *grpc.ClientConn
	conn, err := grpc.Dial(":9000", grpc.WithInsecure())
	if err != nil {
		log.Fatalf("did not connect: %s", err)
	}
	defer conn.Close()

	c := chat.NewChatServiceClient(conn)

	response, err := c.SayHello(context.Background(), &chat.Message{Body: "Hello From Client!"})
	if err != nil {
		log.Fatalf("Error when calling SayHello: %s", err)
	}
	log.Printf("Response from server: %s", response.Body)

}
```

**Step 8:** Let's have fun

Open two terminals and run server in one terminal followed by client in another window. Outputs should look like this:

__Server__

```
ubuntu@grpc:~/golang/src/gprc-demo$ go run server.go
Go GRPC Tutorial!
2020/06/21 13:20:17 Receive message body from client: Hello From Client!
```

__Client__

```
ubuntu@grpc:~/golang/src/gprc-demo$ go run client.go
2020/06/21 13:20:17 Response from server: Hello From the Server!
```

We now have a working gRPC server and client example, which can be extended to your imagination.

That's all for now :-)

# Resources
* [GRPC Official Website - Go Quick Start](https://www.grpc.io/docs/quickstart/go/)
* [Go Official Website - Getting Started](https://golang.org/doc/install)
* [Go gRPC Beginners Tutorial](https://tutorialedge.net/golang/go-grpc-beginners-tutorial/)
---
title: "Running Memcached on Ubuntu"
categories:
  - Distributed Computing
tags:
  - distributed computing
  - memcached
last_modified_at: 20120-08-09T14:25:52-05:00
---

[Memcached](https://memcached.org/about) is an in-memory key-value, distributed memory object caching system, generic in nature, but originally intended for use in speeding up dynamic web applications by alleviating database load.

Memcached allows you to take memory from parts of your system where you have more than you need and make it accessible to areas where you have less than you need.

Indepth description of memcached is available at [Distributed Caching with Memcached](https://www.linuxjournal.com/article/7451)

## Installation

Installation is very simple for Ubuntu

```
apt-get install memcached
```

For other options, please refer to [official instructions](https://github.com/memcached/memcached/wiki/Install).

To confirm if Memcached is installed or not, you need to run the command given below. This command shows that Memcached is running on the default port **11211**.

```
$ps aux | grep memcached
```

To run Memcached server on a different port, execute the command given below. This command starts the server on the TCP port *11111* and listens on the UDP port *11111* as a daemon process.

```
$memcached -p 11111 -U 11111 -u user -d
```

You can run multiple instances of Memcached server through a single installation.

# Connecting to Memcached

To connect to a Memcached server, you need to use the telnet command on HOST and PORT names.

**Syntax**

The basic syntax of Memcached telnet command is as shown below –

```
$telnet HOST PORT
```

Here, HOST and PORT are machine IP and port number respectively, on which the Memcached server is executing.

**Example**

The following example shows how to connect to a Memcached server and execute a simple set and get command. Assume that the Memcached server is running on host 127.0.0.1 and port 11211.

```
$telnet 127.0.0.1 11211
Trying 127.0.0.1...
Connected to 127.0.0.1.
Escape character is '^]'.
// now store some data and get it from memcached server
set mydb 0 900 9
memcached
STORED
get mydb
VALUE mydb 0 9
memcached
END
```

**Connection from Go Application**

Use the following code to connect to memcached server from Go application.

**git/golang/src/ecompute/ec.go**

```
package main

import (
    "fmt"

    "github.com/bradfitz/gomemcache/memcache"
)

func main() {
    // Connect to local memcache instance at default port
    mc := memcache.New("127.0.0.1:11211")

    // Set values
    mc.Set(&memcache.Item{Key: "key1", Value: []byte("first")})
    mc.Set(&memcache.Item{Key: "key2", Value: []byte("last")})

    // Get value
    val, err := mc.Get("key1")

    if err != nil {
        fmt.Println(err)
        return
    }

    fmt.Println("Get Usage:")
    fmt.Printf("%s => %s", "key1", val.Value)
    fmt.Println("\n")

    // Get multiple values
    idx, err := mc.GetMulti([]string{"key1", "key2"})

    if err != nil {
        fmt.Println(err)
        return
    }

    // It is important to note here that "range" iterates in a random order
    fmt.Println("MultiGet Usage:")
    for k, v := range idx {
        fmt.Printf("%s => %s\n", k, v.Value)
    }
}
```

**Cross compiling to run on ubuntu**

```
$ GOOS=linux GOARCH=amd64 go build -o ec ec.go
```

**Output**

On compiling and executing the program, you get to see the following output 
```
ubuntu@node3:~/ecompute$ ./ec
Get Usage:
key1 => first

MultiGet Usage:
key1 => first
key2 => last
```

**Memcached cluster with multiple machines**

Above code example is using localhost to connect to memcached. Generally you would like to connect to a cluster of machines to make use of multiple machines.

This can be achieved easily by changing the comment for new memcache instance as follows:

```
// Connect to multiple machines in a cluster
mc := memcache.New("192.168.64.3:11211", "192.168.64.4:11211", "192.168.64.5:11211")
```

Note that you have to make sure that memcached is running on these IP address.

For Memcached services running on Ubuntu or Debian servers, you can adjust the service parameters by editing the */etc/memcached.conf* file with vi, for instance:

```
sudo vi /etc/memcached.conf
```

By default, Ubuntu and Debian bind Memcached to the local interface 127.0.0.1. Change this option on each machine to the server private IP address. 

```
. . .
-l memcached_servers_private_IP
. . .

```

It can be verified in -l option 

```
ubuntu@node1:~/ecompute$ ps -ef | grep memcache
memcache 16702     1  0 14:53 ?        00:00:00 /usr/bin/memcached -m 64 -p 11211 -u memcache -l 192.168.64.3 -P /var/run/memcached/memcached.pid
```
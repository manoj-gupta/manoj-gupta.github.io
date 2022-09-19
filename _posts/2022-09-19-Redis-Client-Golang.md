---
title: "Redis Client In Golang"
# image: 
#   path: /images/KafkaTopic.png
#   thumbnail: /images/KafkaCluster.png
#   caption: "Kafka Cluster"
categories:
  - Redis
tags:
  - redis
  - golang
  - distributed computing
---

**RE**mote **DI**ctionary **S**ervice ([redis](https://redis.io/)) is an in-memory key-value **data structure store** used as a cache/database. Generally, it used as a cache in front of another persistent database to speed up application performance.

Redis provides in-built support for various data structures such as
* strings
* hashes
* lists
* sets
* sorted sets with range queries
* bitmaps
* hyperloglogs
* geospatial indexes
* streams

# Architecture

An overview of redis architecture is available at [architecturenotes](https://architecturenotes.co/redis/)

# Install on Ubuntu

Refer to official redis [docs](https://redis.io/docs/getting-started/installation/install-redis-on-linux/)


# Golang Client

Redis office site list various redis clients [here](https://redis.io/docs/clients/). I decided to try out [Redigo](https://github.com/gomodule/redigo)

## Create a RedisPool

Create a redispool object using following code

```
package redispool

import (
	"fmt"
	"os"
	"time"

	"github.com/gomodule/redigo/redis"
)

// default redis TCP port
const defaultRedisPort = 6379

// RedisServer ... redis server to connect to
var RedisServer = func() string {
	// $REDIS_SERVER takes precedence
	server := os.Getenv("REDIS_SERVER")
	if server == "" {
		// otherwise use the localhost:6379
		server = fmt.Sprintf("localhost:%d", defaultRedisPort)
	}
	return server
}()

// RedisPool ... redis connection pool
var RedisPool = redis.Pool{
	// check the health of an idle connection before the connection is used again
	TestOnBorrow: testIdleConn,
	MaxIdle:      2,               // Maximum number of idle connections in the pool
	IdleTimeout:  2 * time.Minute, // Close connections after remaining idle for this duration.
	Dial:         dialRedis,
}

func testIdleConn(c redis.Conn, _ time.Time) error {
	fmt.Println("Ping connection")
	reply, err := c.Do("Ping")
	fmt.Printf("TestOnBorrow :: Redis reply %v\n", reply)
	return err
}

func dialRedis() (redis.Conn, error) {
	c, err := redis.Dial("tcp", RedisServer)
	if err != nil {
		fmt.Printf("Dial (%s) failed: %v\n", RedisServer, err)
		return nil, err
	}

	fmt.Printf("opened redis connection to %s\n", RedisServer)
	return c, nil
}
```

## Check Connection to redis server
Use the following code to check connection

```
package main

import (
	"fmt"

	"github.com/manoj-gupta/cloud/redispool"
)

func main() {
	conn := redispool.RedisPool.Get()
	defer func() {
		fmt.Printf("closing redis connection to %s\n", redispool.RedisServer)
		conn.Close()
	}()
}
```

Running the program should generate output
```
$ go run redispool/app/main.go 
opened redis connection to localhost:6379
closing redis connection to localhost:6379
```


## Test GET/SET
Add the following code in `main.go` to see how `GET` and `SET` works for basic data types
```
	if _, err := conn.Do("SET", stringKey, "Hello Redis"); err != nil {
		fmt.Printf("Can not update %s err:%v\n", stringKey, err)
		os.Exit(1)
	}

	if val, err := redis.String(conn.Do("GET", stringKey)); err != nil {
		fmt.Printf("Can not read %s err:%v\n", stringKey, err)
		os.Exit(1)
	} else {
		fmt.Printf("READ Success --> key:%s val:%s\n", stringKey, val)
	}
```

Running the program should generate the output

```
$ go run redispool/app/main.go 
opened redis connection to localhost:6379
READ Success --> key:redis-string-key val:Hello Redis
closing redis connection to localhost:6379
```
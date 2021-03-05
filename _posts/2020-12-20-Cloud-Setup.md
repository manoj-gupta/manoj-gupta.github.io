---
title: "Cloud Setup on Macbook"
categories:
  - Cloud
tags:
  - cloud
---

Setting up cloud development machine can be challenging, especially for new comers. Here is what I did to setup various components on Macbook.

# Install Homebrew

OSX does not come with brew package. Install brew using the [official instructions](https://medium.com/r/?url=https%3A%2F%2Fbrew.sh%2F%23install).

# Install and start Redis

It is very easy to install redis using homebrew.

```
$ brew install redis
```

After installation, following message will be seen:

```
==> redis
To have launchd start redis now and restart at login:
brew services start redis
Or, if you don't want/need a background service you can just run:
redis-server /usr/local/etc/redis.conf
```

## Launch Redis on computer start

```
$ ln -sfv /usr/local/opt/redis/*.plist ~/Library/LaunchAgents
```

## Start and Stop Redis server using launchctl

```
$ launchctl load ~/Library/LaunchAgents/homebrew.mxcl.redis.plist
$ launchctl unload ~/Library/LaunchAgents/homebrew.mxcl.redis.plist
```

## Uninstall Redis and configuration file

```
$ brew uninstall redis
$ rm ~/Library/LaunchAgents/homebrew.mxcl.redis.plist
```

## Redis package info

```
$ brew info redis
```

## Redis server ping test

```
redis-cli ping
```

If "PONG" is printed on terminal, you are good to go!

# Install and start MongoDB

Follow official Instructions at [mongodb website](https://medium.com/r/?url=https%3A%2F%2Fdocs.mongodb.com%2Fmanual%2Ftutorial%2Finstall-mongodb-on-os-x%2F).

Here are the steps:

* Ensure to have installed Xcode.
* Tap the official MongoDB homebrew Tap.

```
brew tap mongodb/brew
```

* Install mongodb using brew

```
brew install mongodb-community@4.2
```

You should see instructions to start mongod

```
To have launchd start mongodb/brew/mongodb-community now and restart at login:
  brew services start mongodb/brew/mongodb-community
Or, if you don't want/need a background service you can just run:
  mongod --config /usr/local/etc/mongod.conf
```

Following are the paths created:

* the [configuration file](https://medium.com/r/?url=https%3A%2F%2Fdocs.mongodb.com%2Fmanual%2Freference%2Fconfiguration-options%2F) (/usr/local/etc/mongod.conf)
* the [log directory path](https://medium.com/r/?url=https%3A%2F%2Fdocs.mongodb.com%2Fmanual%2Freference%2Fconfiguration-options%2F%23systemLog.path) (/usr/local/var/log/mongodb)
* the [data directory path](https://medium.com/r/?url=https%3A%2F%2Fdocs.mongodb.com%2Fmanual%2Freference%2Fconfiguration-options%2F%23storage.dbPath) (/usr/local/var/mongodb)

## Running mongod

To run MongoDB (i.e. the mongod process) as a macOS service, issue the following:

```
brew services start mongodb-community@4.2
```

To stop a `mongod` running as a macOS service, use the following command:

```
brew services stop mongodb-community@4.2
```

To run MongoDB (i.e. the `mongod` process) manually as a background process, issue the following:

```
mongod --config /usr/local/etc/mongod.conf --fork
```

To stop a `mongod` running as a background process, connect to the `mongod` from the **mongo** shell, and issue the shutdown command as needed.

To verify that MongoDB is running, search for `mongod` in your running processes:

```
ps aux | grep -v grep | grep mongod
```

You can also view the log file to see the current status of your `mongod` process: `/usr/local/var/log/mongodb/mongo.log`.

## Connect and Use MongoDB

To begin using MongoDB, connect a `mongo` shell to the running instance. From a new terminal, issue the following:

```
mongo
```

## Deploying replica set on localhost

A replica set is a cluster that consists of at least two `mongod` instances that replicate data amongst one another. Such setup increases redundancy and ensures high availability of the database. 

**This setup should be used only for testing or development environment.**

We will configure **three member** replica set that provide enough redundancy to survive most network partitions and other system failures. These sets also have sufficient capacity for many distributed read operations. Replica sets should always have an odd number of members. This ensures that elections will proceed smoothly.

Following are the steps to follow:

1. Create the data directories for each member.

```
mkdir -p $HOME/mongodb/rs0-0  $HOME/mongodb/rs0-1 $HOME/mongodb/rs0-2
```

2. Start your `mongod` instances in their own terminal.

First member:

```
mongod --replSet rs0 --port 27017 --bind_ip localhost --dbpath $HOME/mongodb/rs0-0  --oplogSize 128
```

Second member:

```
mongod --replSet rs0 --port 27018 --bind_ip localhost --dbpath $HOME/mongodb/rs0-1  --oplogSize 128
```

Third member:

```
mongod --replSet rs0 --port 27019 --bind_ip localhost --dbpath $HOME/mongodb/rs0-2 --oplogSize 128
```

3. Connect to one of your `mongod` instances through the `mongo` shell. Indicate the instance by specifying port number,

```
mongo --port 27017
```

4. In the mongo shell, use `rs.initiate()` to initiate the replica set.

First create replica set configuration object in `mongo` shell environment as follows:

```
rsconf = {
  _id: "rs0",
  members: [
    {
     _id: 0,
     host: "localhost:27017"
    },
    {
     _id: 1,
     host: "localhost:27018"
    },
    {
     _id: 2,
     host: "localhost:27019"
    }
   ]
}
```

Pass the `rsconf` file to `rs.initiate()` as follows:

```
rs.initiate( rsconf )
```

5. Display the current replica configuration.

```
rs.conf()
```

6. Check the status of replica set at any time with the rs.status() operation.

```
rs.status()
```

### Script

* [Link](https://medium.com/@OndrejKvasnovsky/mongodb-replica-set-on-local-macos-f5fc383b3fd6)

## MongoDB Change Streams

There is a new feature in MongoDB 3.6 called [Change Stream](https://medium.com/r/?url=https%3A%2F%2Fdocs.mongodb.com%2Fmanual%2FchangeStreams%2F), which enable applications to stream real-time data changes by leveraging MongoDB's underlying replication capabilities. MongoDB needs to be started with replica set in order to use *change stream*.

Change streams can notify the application of all writes to documents (including deletes) and provide access to all available information as changes occur, without polling that can introduce delays and incur higher overhead.

## Tools

* https://github.com/ludocode/msgpack-tools

```
redis-cli - raw get "exec-engine-v2:overall:location:32341" | /usr/local/bin/msgpack2json -d
```

# Install Cassandra

Apache Cassandra is an open source distributed database management system based upon a decentralized and shared nothing architecture. Nodes in the Cassandra cluster are functionally identical servers. Data is distributed and replicated across each autonomous node and with no single point of failure. Cassandra cluster handles vast amounts of data and scales linearly with the inclusion of additional nodes.

## Installation

Java 7 or 8 is a prerequisite for Cassandra so make sure to download the [Java Development Kit](https://medium.com/r/?url=http%3A%2F%2Fwww.oracle.com%2Ftechnetwork%2Fjava%2Fjavase%2Fdownloads%2Findex.html).

Create a directory to keep Cassandra and get cassandra 3.11.10 in this directory.

```
▶ mkdir -p ~/opt/packages/cassandra/
▶ cd ~/opt/packages/cassandra/
▶ curl -O https://mirrors.estointernet.in/apache/cassandra/3.11.10/apache-cassandra-3.11.10-bin.tar.gz
▶ tar zxvf apache-cassandra-3.11.10-bin.tar.gz
```

Create a symbolic link to the Cassandra 3.11.10 directory. This will keep is from having to change environment variables later on.

```
ln -s ~/opt/packages/cassandra/apache-cassandra-3.11.10 ~/opt/cassandra
```

Add the following to shell profile file (e.g. `.zshrc`)

```
if [ -d "$HOME/opt/cassandra" ]; then
    export PATH="$PATH:$HOME/opt/cassandra/bin"
fi
```

Verify Cassandra installation

```
▶ cassandra -v
3.11.10
```

Start Cassandra server in non-daemon mode, which will allow us to see output to the terminal.

```
▶ cassandra -f
```

## Cassandra Query Language

With the Cassandra server running, open a new terminal window and access the Cassandra Query Language shell by typing:

```
▶ cqlsh
Connected to Test Cluster at 127.0.0.1:9042.
[cqlsh 5.0.1 | Cassandra 3.11.10 | CQL spec 3.4.4 | Native protocol v4]
Use HELP for help.
cqlsh> DESCRIBE KEYSPACES
system_traces  system_schema  system_auth  system  system_distributed
cqlsh>
```

Create a Keyspace, which is a container for our application data. The keyspace requires that the replication strategy and replication factor be specified.

```
cqlsh> CREATE KEYSPACE test
   ...     WITH REPLICATION = {
   ...         'class': 'SimpleStrategy',
   ...         'replication_factor': 1
   ...     };
cqlsh> DESCRIBE KEYSPACES
system_schema  system_auth  system  system_distributed  test  system_traces
```

Switch keyspace and create a table in new keyspace

```
USE test;
CREATE TABLE books (
    id INT PRIMARY KEY,
    name TEXT,
    author TEXT
);
```

Insert data into the partition with a single row:

```
INSERT INTO books (id, name, author) VALUES (1, 'The Tragedy of Hamlet', 'William Shakespeare');
```

Query the data just entered

```
cqlsh:test> SELECT * FROM books WHERE id=1;
id | author              | name
----+---------------------+-----------------------
  1 | William Shakespeare | The Tragedy of Hamlet
(1 rows)
```

## Issues seen due to jenv settings

In case you seen something like this

```
▶ cassandra -f
  Unable to find java executable. Check JAVA_HOME and PATH environment variables.
```

`jenv` versions showed the current version was system. I am not sure why that version was not proper. I set the global version to 1.8.0.202 and everything is working as expected.

```
▶ jenv global 1.8.0.202
▶ jenv versions
  system
  1.8
* 1.8.0.202 (set by /Users/manojg/.jenv/version)
  oracle64-1.8.0.202
```

# References
* [MongoDB Manual](https://medium.com/r/?url=https%3A%2F%2Fdocs.mongodb.com%2Fmanual%2Ftutorial%2Fdeploy-replica-set-for-testing%2F)
* [An Introduction to Change Streams](https://medium.com/r/?url=https%3A%2F%2Fwww.mongodb.com%2Fblog%2Fpost%2Fan-introduction-to-change-streams)
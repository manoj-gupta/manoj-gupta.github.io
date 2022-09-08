---
title: "Kafka Producer and Consumer In Golang"
# image: 
#   path: /images/KafkaTopic.png
#   thumbnail: /images/KafkaCluster.png
#   caption: "Kafka Cluster"
categories:
  - Kafka
tags:
  - Kafka
---

Apache Kafka is a **distributed publish-subscribe messaging system** that is designed to be fast, scalable, and durable. Kafka stores streams of records (messages) in topics. Each record consists of *a key, a value, and a timestamp*. Producers write data to topics and consumers read from topics.

# Running Kafka Cluster

For demostration, Kafka cluster is needed. I have installed it on x86 machine running Ubuntu 22.04.1 LTS.

## Install Java

First, you need to install Java in order to run the Kafka executables. Visit official download page to download Debian file on your system. Also, you can use the below command to download the Debian file using the command line.

```
$ wget https://download.oracle.com/java/17/latest/jdk-17_linux-x64_bin.deb
```

Once you download the Debian file, install it on your system using command below:

```
sudo apt install ./jdk-17_linux-x64_bin.deb 
```

The above command installs Oracle Java 17 on your system. All the files will be placed under `/usr/lib/jvm` directory. Now set an alternative reference to newly installed Java on your system.

```
sudo update-alternatives --install /usr/bin/java java /usr/lib/jvm/jdk-17/bin/java 100
```

## Install Kafka

* Download Kafka's binary from [official download page](https://kafka.apache.org/downloads).
* Extract it to a directory using following command ```tar -xvf kafka_2.12–2.5.0.tgz``` Calling this directory as ```KAFKA_DIR``` for future reference.
* Check it is running using ```bin/kafka-topics.sh```
* Add the path to ```PATH``` environment variable by editing ```.bashrc``` file

## Running


**Start Zookeeper**

Zookeeper is a distributed key-value store commonly used to store server state for coordination, which is required by Kafka to run.

To start Zookeeper issue the following command from ```KAFKA_DIR```


```
bin/zookeeper-server-start.sh config/zookeeper.properties
```

**Start Kafka brokers**

Kafka brokers are where our data is stored and distributed. Similar to Zookeeper, there are two files to start and configure the broker servers.

* ```bin/kafka-server-start.sh```: which is used to start the broker server.
* ```config/server.properties```: which provides the default broker configuration

Let’s start up 3 brokers to have distributed system. There are several options but three properties need to be unique for each server
```
# The id of the broker. This must be set to a unique integer for each broker.
broker.id=0

# The address the socket server listens on. It will get the value returned from 
listeners=PLAINTEXT://:9092

# A comma separated list of directories under which to store log files
log.dirs=/tmp/kafka-logs
```

Copy the config/server.properties file and make 3 files for each server instance:
```
cp config/server.properties config/server.1.properties
cp config/server.properties config/server.2.properties
cp config/server.properties config/server.3.properties
```

Change above mentioned properties for each file so that they are all unique.

**server.1.properties**
```
broker.id=1
listeners=PLAINTEXT://:9093
log.dirs=/tmp/kafka-logs-1
```

**server.2.properties**
```
broker.id=2
listeners=PLAINTEXT://:9094
log.dirs=/tmp/kafka-logs-2
```

**server.3.properties**
```
broker.id=3
listeners=PLAINTEXT://:9095
log.dirs=/tmp/kafka-logs-3
```

Create the required log directories.
```
mkdir /tmp/kafka-logs-1 /tmp/kafka-logs-2 /tmp/kafka-logs-3
```

Start the broker instances using below commands on different terminal sessions.
```
bin/kafka-server-start.sh config/server.1.properties
```

```
bin/kafka-server-start.sh config/server.2.properties
```

```
bin/kafka-server-start.sh config/server.3.properties
```

You should see a startup message when the brokers start successfully.
```
[2022-09-08 15:11:10,987] INFO [KafkaServer id=3] started (kafka.server.KafkaServer)
```

## Create sample topic

We need to create a topic to which the data before adding data to Kafka. To do this, run the command:

```
$ bin/kafka-topics.sh --create --topic sample-kafka-topic --bootstrap-server localhost:9093 --partitions 3 --replication-factor 2
```

Once you create the topic, you should see a confirmation message:
```
Created topic sample-kafka-topic.
```

You can verify it using the command:
```
bin/kafka-topics.sh --list --bootstrap-server localhost:9093
```

It should return the topic just created
```
sample-kafka-topic
```

# Implementation

## Kafka Producer

Following function writes a sample message every second into Kafka Cluster.
```
// the topic and broker addresses
const (
	topic          = "message-log"
	broker1Address = "localhost:9093"
	broker2Address = "localhost:9094"
	broker3Address = "localhost:9095"
)

func Produce(ctx context.Context) {
	count := 0

	l := log.New(os.Stdout, "Kafka producer: ", 0)
	// intialize the writer with the broker addresses, and the topic
	w := kafka.NewWriter(kafka.WriterConfig{
		Brokers: []string{broker1Address, broker2Address, broker3Address},
		Topic:   topic,
		Logger:  l, // assign the logger to the writer
	})

	for {
		// message is key:value pair. Key determines the partition to publish data
		err := w.WriteMessages(ctx, kafka.Message{
			Key:   []byte(strconv.Itoa(count)),
			Value: []byte("Sample message" + strconv.Itoa(count)),
		})

		if err != nil {
			panic("Error: write message " + err.Error())
		}

		// log a confirmation once the message is written
		fmt.Println("written: ", count)
		count++

		// sleep for a second
		time.Sleep(time.Second)
	}
}
```

# Kafka Consumer

Following function consumes messages from the Kafka cluster.

```
func Consume(ctx context.Context) {
	l := log.New(os.Stdout, "Kafka consumer: ", 0)

	// groupID - identifies the consumer to receiving duplicates
	r := kafka.NewReader(kafka.ReaderConfig{
		Brokers: []string{broker1Address, broker2Address, broker3Address},
		Topic:   topic,
		GroupID: "sample-group",
		Logger:  l, // assign the logger to the reader
	})

	for {
		// the `ReadMessage` blocks until we receive the next event
		msg, err := r.ReadMessage(ctx)
		if err != nil {
			panic("could not read message " + err.Error())
		}
		// after receiving the message, log its value
		fmt.Println("received: ", string(msg.Value))
	}
}
```

Following function calls above functions to demonstrate functionality.

```
func main() {
	ctx := context.Background()
	// produce messages in a new go routine as
	// both producer and consumer functions are blocking
	go kafkapc.Producer(ctx)
	kafkapc.Consumer(ctx)
}
```

By running this code, following output will be seen:

```
Kafka producer: writing 1 messages to sample-kafka-topic (partition: 0)
written:  0
Kafka consumer: committed offsets for group sample-group: 
	topic: sample-kafka-topic
		partition 0: 1
received:  Sample message0
Kafka producer: writing 1 messages to sample-kafka-topic (partition: 1)
Kafka consumer: committed offsets for group sample-group: 
	topic: sample-kafka-topic
		partition 1: 1
received:  Sample message1
written:  1
Kafka producer: writing 1 messages to sample-kafka-topic (partition: 2)
Kafka consumer: committed offsets for group sample-group: 
	topic: sample-kafka-topic
		partition 2: 1
received:  Sample message2
written:  2
Kafka producer: writing 1 messages to sample-kafka-topic (partition: 0)
written:  3
Kafka consumer: committed offsets for group sample-group: 
	topic: sample-kafka-topic
		partition 0: 2
received:  Sample message3
....
....
```

To tune Kafka client, you can use options list  [here](https://kafka.apache.org/documentation.html#configuration). 
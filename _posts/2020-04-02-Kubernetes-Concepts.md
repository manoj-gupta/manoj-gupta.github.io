---
title: "Kubernetes Concepts"
categories:
  - Kubernetes
tags:
  - open source
  - kubernetes
last_modified_at: 20120-08-09T14:25:52-05:00
---

Kubernetes (K8s) is an open-source system for automating deployment, scaling, and management of containerized applications, as described at [Kubernetes](https://kubernetes.io/) website.

A Kubernetes cluster consists of a set of worker machines, called nodes, that run containerized applications. Every cluster has at least one worker node. To understand the system, few basic things needs to be understood.

# Master Node
The master node is responsible for the management of Kubernetes cluster. Master node consists of following components:
* **API Server** is the entry points for all the REST commands used to control the cluster.
* **etcd** is a simple, distributed, consistent key-value store. It’s mainly used for shared configuration and service discovery. 
* **Scheduler** has the information regarding resources available on the members of the cluster, as well as the ones required for the configured service to run and hence is able to decide where to deploy a specific service. 
* **controller-manager** is a daemon embedding different kinds of controllers inside the master node. 

# Worker Node
The pods are run on worker nodes, which consists of following components:
* **Docker** runs on each of the worker nodes, and runs the configured pods. It takes care of downloading the images and starting the containers.
* **kubelet** gets the configuration of a pod from the apiserver and ensures that the described containers are up and running.
* **kube-proxy** acts as a network proxy and a load balancer for a service on a single worker node. It takes care of the network routing for TCP and UDP packets.
* **kubectl** is a command line tool to communicate with the API service and send commands to the master node.

# Pods
A **pod** is the lowest unit of an application in Kubernetes. A pod is not same as container but can be made up of multiple containers and has following properties:
* A single IP address
* Share localhost
* A shared IPC space
* A shared network port range
* Shared volumes

The containers in a pod talk to each other via local host, whereas pod-to-pod communication is done via services. A pod is a single entity, and if it fails, it cannot restart itself.

# ReplicaSets
**Replica Sets** ensures a certain number of pods are always running. Replica Set allows you to define the number of pods that need to be running at all times and this number could be “1”. If a pod crashes, it will be recreated to get back to the desired state. For this reason, replica sets are preferred over a simple pod because they provide high availability.

# Services and Labels
**Services** tie our pods together and provide a front end resource to access. This allows for the traffic to be load balanced for failures. A service allows Kubernetes to set a single DNS record for the pods.

**Labels** are just a key value pair, or tag, that provides metadata on our objects. A Kubernetes Service can select the pods it is supposed to abstract through a label selector. 

With the service resource type, you would usually define a selector like the example below:
```
apiVersion: v1
kind: Service
metadata:
  name: my-service
spec:
  selector:
    app: MyApp
```

# Deployment
The **deployment** resource type sits above a replica set and manage the replica objects. Replica sets are all or nothing. If you need to do an upgrade, you need to replace the replica set. This action will cause downtime to your application.

Deployments give us the functionality to do upgrades without downtime. As you do in a replica set, you specify the number of pods you would like to run. Once you trigger an update a deployment will do a rolling upgrade on the pods, all while making sure the upgrade is successful on the pod before moving to the next one.

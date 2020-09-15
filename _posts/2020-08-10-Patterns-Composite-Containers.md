---
title: "Patterns for Composite Containers"
categories:
  - Kafka
tags:
  - Kafka
  - Kafka Connect
  - Kafka Streams
---

Building an application from modular containers means thinking about symbiotic groups of containers that cooperate to provide a service, not one container per service. In *Kubernetes*, the embodiment of this modular container service is a Pod. A **Pod** is a group of containers that share resources like file systems, kernel namespaces and an IP address. The *Pod* is the atomic unit of scheduling in a *Kubernetes* cluster, precisely because the symbiotic nature of the containers in the *Pod* require that they be co-scheduled onto the same machine, and the only way to reliably achieve this is by making container groups atomic scheduling units.

When you start thinking in terms of *Pods*, there are naturally some general patterns of modular application development that re-occur multiple times.

# Sidecar containers

Sidecar containers extend and enhance the "**main**" container, they take existing containers and make them better. As an example, consider a container that runs the *Nginx* web server. Add a different container that syncs the file system with a *git* repository, share the file system between the containers and you have built git push-to-deploy. But you've done it in a modular manner where the *git* synchronizer can be built by a different team, and can be reused across many different web servers (Apache, Python, Tomcat, etc). Because of this modularity, you only have to write and test your *git* synchronizer once and reuse it across numerous apps. And if someone else writes it, you don't even need to do that.

![image-center]({{ '/images/SidecarContainerPattern.png' | absolute_url }}){: .align-center}

# Ambassador containers

Ambassador containers proxy a local connection to the world. As an example, consider a *Redis* cluster with read-replicas and a single write master. You can create a Pod that groups your main application with a *Redis* ambassador container. The ambassador is a proxy is responsible for splitting reads and writes and sending them on to the appropriate servers. Because these two containers share a network namespace, they share an IP address and your application can open a connection on "localhost" and find the proxy without any service discovery. As far as your main application is concerned, it is simply connecting to a *Redis* server on localhost. This is powerful, not just because of separation of concerns and the fact that different teams can easily own the components, but also because in the development environment, you can simply skip the proxy and connect directly to a *Redis* server that is running on localhost.

![image-center]({{ '/images/AmbassadorContainerPattern.png' | absolute_url }}){: .align-center}

# Adapter containers

Adapter containers standardize and normalize output. Consider the task of monitoring N different applications. Each application may be built with a different way of exporting monitoring data. (e.g. JMX, StatsD, application specific statistics) but every monitoring system expects a consistent and uniform data model for the monitoring data it collects. By using the adapter pattern of composite containers, you can transform the heterogeneous monitoring data from different systems into a single unified representation by creating Pods that groups the application containers with adapters that know how to do the transformation. Again because these Pods share namespaces and file systems, the coordination of these two containers is simple and straightforward.

![image-center]({{ '/images/AdapterContainerPattern.png' | absolute_url }}){: .align-center}

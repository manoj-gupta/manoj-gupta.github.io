---
layout: post
title:  "Working with K3s using Multipass"
date:   2020-04-01 16:00:00 +0530
categories: Kubernetes
---

One of the easiest way to try out k3s is by using [multipass](https://multipass.run/), a tool that allows you to spin up Ubuntu VMs in seconds on MAC, Linux or Windows machine.

## Multipass
# Download
You can download the binary for your platform from  [multipass](https://multipass.run/) website. It list the available command by just executing it.

```
user@HOSTNAME$ multipass
Usage: multipass [options] <command>
Create, control and connect to Ubuntu instances.

This is a command line utility for multipass, a
service that manages Ubuntu instances.

Options:
  -h, --help     Display this help
  -v, --verbose  Increase logging verbosity, repeat up to three times for more
                 detail

Available commands:
  delete    Delete instances
  exec      Run a command on an instance
  find      Display available images to create instances from
  get       Get a configuration setting
  help      Display help about a command
  info      Display information about instances
  launch    Create and start an Ubuntu instance
  list      List all available instances
  mount     Mount a local directory in the instance
  purge     Purge all deleted instances permanently
  recover   Recover deleted instances
  restart   Restart instances
  set       Set a configuration setting
  shell     Open a shell on a running instance
  start     Start instances
  stop      Stop running instances
  suspend   Suspend running instances
  transfer  Transfer files between the host and instances
  umount    Unmount a directory from an instance
  version   Show version details
```

# Create ubuntu VMs
Create VMs with default configuration of 1GB RAM and one CPU.

```
user@HOSTNAME$ multipass launch -n node1
user@HOSTNAME$ multipass launch -n node2
user@HOSTNAME$ multipass launch -n node3
```

# Useful Commands

```
user@HOSTNAME$ multipass list
Name                    State             IPv4             Image
node1                   Running           192.168.64.3     Ubuntu 18.04 LTS
node2                   Running           192.168.64.4     Ubuntu 18.04 LTS
node3                   Running           192.168.64.5     Ubuntu 18.04 LTS
```

```
user@HOSTNAME$ multipass exec node1 -- uname -a
Linux node1 4.15.0-88-generic #88-Ubuntu SMP Tue Feb 11 20:11:34 UTC 2020 x86_64 x86_64 x86_64 GNU/Linux
```

## Setting up k3s cluster

# Download
You can download the binary for your platform from [k3s](https://github.com/rancher/k3s/releases/tag/v1.17.2+k3s1) website.

# Copy k3s binary to nodes
The recommended way to share data between your host and the instance is the **mount** command:

user@HOSTNAME$ multipass mount $HOME node1

From this point on `/Users/manoj` will be available inside the instance. Use **umount** to unmount it again and you can change the target by passing it after the instance name:

```
user@HOSTNAME$ multipass info node1
...
....
Mounts:         /Users/manoj => /Users/manoj
                    UID map: 502:default
                    GID map: 20:default
```

Shell into the node instance and copy the binary

```
user@HOSTNAME$ multipass shell node1
ubuntu@node1:~/dl$ cp /Users/manoj/k3s .
ubuntu@node1:~/dl$ ls
k3s
ubuntu@node1:~/dl$ chmod +x k3s
ubuntu@node1:~/dl$ ./k3s
NAME:
   k3s - Kubernetes, but small and simple

USAGE:
   k3s [global options] command [command options] [arguments...]
...
...
```

For convenience, copy `k3s` to `/usr/local/bin`

When you want to unmount node1

```
user@HOSTNAME$ multipass umount node1
```

# Bringing up master and workers

Role of various nodes:
* server: node1
* workers: node2, node3

Run server on node1 using 

```
ubuntu@node1:~$ sudo k3s server &
```

Get the token from master

```
ubuntu@node1:~$ sudo cat /var/lib/rancher/k3s/server/node-token
xxxxxxe68bb141xxxx7dbd9xxxxa2caxxxxf94054043e11cxxxx4c160f8a1exxxxx::server:xx941xxxxfc1cxxxxx6ddde87b3xxxxx
```

```
Join node2 and node3 to cluster

ubuntu@node2:~$ export K3S_URL="https://192.168.64.3:6443"
ubuntu@node2:~$ export K3S_TOKEN="xxxxxxe68bb141xxxx7dbd9xxxxa2caxxxxf94054043e11cxxxx4c160f8a1exxxxx::server:xx941xxxxfc1cxxxxx6ddde87b3xxxxx"

ubuntu@node2:sudo k3s agent --server ${K3S_URL} --token ${K3S_TOKEN} &
```

On master node

```
ubuntu@node1:~$ sudo k3s kubectl get nodes
NAME    STATUS   ROLES    AGE     VERSION
node1   Ready    master   19m     v1.17.2+k3s1
node3   Ready    <none>   3m35s   v1.17.2+k3s1
node2   Ready    <none>   7m48s   v1.17.2+k3s1
```

## Running Nginx Pods
To launch a pod using the container image nginx and exposing a HTTP API on port 80, execute:

```
user@HOSTNAME$ kubectl run mynginx --image=nginx --replicas=3 --port=80
kubectl run --generator=deployment/apps.v1 is DEPRECATED and will be removed in a future version. Use kubectl run --generator=run-pod/v1 or kubectl create instead.
deployment.apps/mynginx created
```

# Listing Nginx Pods
You can now see that the pod is running:

```
user@HOSTNAME$ kubectl get po
NAME                       READY   STATUS    RESTARTS   AGE
mynginx-7f79686c94-dp9j8   1/1     Running   0          86s
mynginx-7f79686c94-bxn5n   1/1     Running   0          86s
mynginx-7f79686c94-8rbq4   1/1     Running   0          86s
```

# Exposing the Deployment

Create a Service object that exposes the deployment:

```
user@HOSTNAME$ kubectl expose deployment mynginx --port 80
service/mynginx exposed
```

# Verifying the endpoints controller for Pods

The below command verifies if endpoints controller has found the correct Pods for your Service:

```
user@HOSTNAME$ kubectl get endpoints mynginx
NAME      ENDPOINTS                                 AGE
mynginx   10.42.0.10:80,10.42.1.5:80,10.42.2.5:80   17m
```

# Testing if Nginx application is up & running:

On each of the nodes, you can try

```
ubuntu@node1:~$ curl 10.42.1.5:80
<!DOCTYPE html>
<html>
<head>
<title>Welcome to nginx!</title>
<style>
    body {
        width: 35em;
        margin: 0 auto;
        font-family: Tahoma, Verdana, Arial, sans-serif;
    }
</style>
</head>
<body>
<h1>Welcome to nginx!</h1>
<p>If you see this page, the nginx web server is successfully installed and
working. Further configuration is required.</p>

<p>For online documentation and support please refer to
<a href="http://nginx.org/">nginx.org</a>.<br/>
Commercial support is available at
<a href="http://nginx.com/">nginx.com</a>.</p>

<p><em>Thank you for using nginx.</em></p>
</body>
</html>
```

# Cleaning up

```
kubectl delete --all pods
pod "mynginx-84b8d48d44-9ghrl" deleted
pod "mynginx-84b8d48d44-bczsv" deleted
pod "mynginx-84b8d48d44-qqk9p" deleted
```
---
title: "Using Golang modules with CircleCI"
categories:
  - Golang
tags:
  - golang
  - cicd
---

An overview of using Golang modules with CircleCI, which is a continuous integration and delivery platform.

# Golang Modules

Golang modules is a Golang dependency management system. It makes dependency version information explicit and easier to manage. It lets you work from any directory and not just from **GOPATH**. It allows to install specific version(s) of a dependency package to avoid breaking changes. The go.mod file list the dependency of the project so that all dependencies need not be distributed with the package.

A module is like a package that you can share with other people. Hence, it has to be a Git or any other **VCS** repository.

# CircleCI

CircleCI is a continuous integration and delivery platform to automate software builds, tests, and deployments.

Nice workflow from [CircleCI webpage](https://circleci.com/docs/2.0/about-circleci/)

![image-center]({{ '/images/circleci.png' | absolute_url }}){: .align-center}

# Example of using Golang modules with CircleCI

## Project structure

* Create an empty directory anywhere but inside **GOPATH** (e.g.`toolkit`) to host the module which will contain some packages as well.
* Create a Git repository for this module (e.g. `github.com/manoj-gupta/toolkit`)
* Initialize the Git repository with the github URL (`git init; git remote add origin github.com/manoj-gupta/toolkit`)
* Initialize Go Module inside this directory using `go mod init <import-path>` (e.g. `go mod init github.com/manoj-gupta/toolkit`)
* Create directory for package (e.g. `algo`) and add code in .go files

```
manoj@MACBOOK:~/gomodule$mkdir toolkit
manoj@MACBOOK:~/gomodule$cd toolkit/
manoj@MACBOOK:~/gomodule/toolkit$echo "# toolkit" >> README.md
manoj@MACBOOK:~/gomodule/toolkit$git init
Initialized empty Git repository in /Users/manojg3/gomodule/toolkit/.git/
manoj@MACBOOK~/gomodule/toolkit$git remote add origin https://github.com/manoj-gupta/toolkit.git
manoj@MACBOOK:~/gomodule/toolkit$git remote -v
origin https://github.com/manoj-gupta/toolkit.git (fetch)
origin https://github.com/manoj-gupta/toolkit.git (push)
manoj@MACBOOK:~/gomodule/toolkit$go mod init github.com/manoj-gupta/toolkit
go: creating new go.mod: module github.com/manoj-gupta/toolkit
manoj@MACBOOK:~/gomodule/toolkit$cat go.mod
module github.com/manoj-gupta/toolkit
go 1.13
manoj@MACBOOK:~/gomodule/toolkit$mkdir algo
manoj@MACBOOK:~/gomodule/toolkit$cd ..
manoj@MACBOOK:~/gomodule$mkdir main
manoj@MACBOOK:~/gomodule$cd main
manoj@MACBOOK:~/gomodule/main$go mod init main
go: creating new go.mod: module main
manoj@MACBOOK:~/gomodule/main$cat go.mod
module main
go 1.13
```

* Create commit and push to Github

```
manoj@MACBOOK:~/gomodule/toolkit$git add .
manoj@MACBOOK:~/gomodule/toolkit$git commit -m "Initial Commit"
manoj@MACBOOK:~/gomodule/toolkit$git push -u origin master
```

* Create `main` module for testing purpose using the command `go mod init main` which will `go.mod` file in `main` folder.
* Create `app.go` file in `main` folder and import the module to test (e.g. `github.com/manoj-gupta/toolkit/algo`)

![image-center]({{ '/images/toolkit_algo.png' | absolute_url }}){: .align-center}

* You should see output as follows:

```
manoj@MACBOOK:~/gomodule/main$go run app.go 
manoj@MACBOOK:~/gomodule/main$cd ../main/
manoj@MACBOOK:~/gomodule/main$go run app.go 
[4 12]
manoj@MACBOOK:~/gomodule/main$cat go.mod 
module main
go 1.13
require github.com/manoj-gupta/toolkit v0.0.0-20200918082747-a0172a0611d7
```

# Connect to CircleCI

You can execute the tests for Go in CircleCI by adding the  `config.yml` file to the project and configuring the Github repository in CircleCI (Refer [Language Guide: Go](https://circleci.com/docs/2.0/language-go/)).

Next, login to your CircleCI account and “*Setup Project*” against your repo in “*Projects*” section. Make sure to choose existing “Use Existing Config”. It should not start building followed by success.
To better understand what your pipeline is doing, follow the steps provided at [Digging Into Your First Pipeline](https://circleci.com/docs/2.0/getting-started/#digging-into-your-first-pipeline).

# Code repository

* [Github Repo](https://github.com/manoj-gupta/toolkit)

# Resources:

* [Using Go Modules](https://blog.golang.org/using-go-modules)
* [CircleCI: Continuous Integration and Delivery](https://circleci.com/)
* [Anatomy of modules in Go](https://medium.com/rungo/anatomy-of-modules-in-go-c8274d215c16)
* [Language Guide: Go](https://circleci.com/docs/2.0/language-go/)




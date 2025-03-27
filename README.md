<p align="center">
      <img src="https://i.ibb.co/vdydkBk/cpp-img.jpg" alt="cpp_template_project" width="300">
</p>

<p align="center">
    <img src="https://img.shields.io/appveyor/build/gruntjs/grunt" alt="build">
    <img src="https://img.shields.io/badge/Version-0.1-blue" alt="Version">
    <img src="https://img.shields.io/badge/License-GPL--3.0-orange" alt="License">
</p>

---

## About

The goal of this project is to use the quicksort algorithm and multithreading to sort data faster than the standard library sorting algorithm.

---

## Requirements
* C++ version: [`C++20`](https://en.cppreference.com/w/cpp/17)
* Build system: [`CMake`](https://cmake.org/)
* C++ compiler: `g++`
* Code coverage report: [`lcov`](http://ltp.sourceforge.net/coverage/lcov.php)
* Package manager: [`Conan`](https://conan.io/downloads) or
* Test framework: [`gtest`](https://github.com/google/googletest)

---

## Run programm in docker

* Install docker and pull image of ubuntu: `./scripts/docker_setup.sh` (if you already have docker, then just do `docker pull ubuntu:22.04`)

* Build app in docker: `./scripts/docker_build.sh`

* Run unit tests in docker: `./scripts/docker_test.sh`


## Run programm locally

* Install dependencies: `./scripts/install_dep.sh`
* Build project: `./scripts/build.sh`
* Run unit tests: `./scripts/test.sh`

- To create your own class use script: ./scripts/class.sh ClassName ./path
---
## Developers

- [Renat](https://github.com/khatymov)

---

## License
Current project is distributed under the GPL-3.0 license
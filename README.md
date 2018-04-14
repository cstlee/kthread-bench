# kthread-bench
Kernel thread performance benchmark

## Dependacies

* g++
* linux headers
* python
* matplotlib (python libary)
* python-pip (to install matplotlib)
* cmake (used by docopt library)

## Build Instructions

0. Clone the repository.

    ```
    git clone https://github.com/cstlee/kthread-bench.git
    ```

1. Download and Build Libraries.
    ```
    cd kthread-bench

    # Download Submodules
    git submodule update --init --recursive
    
    # Build PerfUtils
    pushd lib/PerfUtils
    make
    popd
    
    # Build docopt.cpp
    pushd lib/docopt.cpp
    cmake .
    make
    popd
    ```

2. Build Benchmark

    ```
    make
    ```

## Vagrant
A [Vagrant](https://www.vagrantup.com) configuration is provided which defines a Debian VM which includes all the necessary build dependancies.  If Vagrant is installed, use the following commands from the project root directory to build and enter the VM.

```
vagrant up
vagrant ssh
```

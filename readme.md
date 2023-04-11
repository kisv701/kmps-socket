# Kims Multi-Platform Socket (kmps)

This repo is intended to be a header only wrapper for sockets on Linux and Windows systems.

# Example app
This repo contains an example app for using the kmps_socket api.

## Build for windows
```
$ mkdir build
$ cd build
$ cmake -DCMAKE_TOOLCHAIN_FILE=../cross-compile-windows-cmake/windows_toolchain.cmake ..
$ make
```

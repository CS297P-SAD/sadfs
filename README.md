# sadfs
A simple distributed file system.

### Build
To build the project, run:
```
$ mkdir build
$ cd build
$ cmake ..
$ make
```

### Run
To run any of the daemons, run:
```
$ PATH=$PWD/bin bin/{sadmd|sadcd|sadfsd}-bootstrap
```

To see how to override config parameters, run:
```
$ ./bin/{sadmd|sadcd|sadfsd}-bootstrap {-h|--help}
```
### Dependencies
To run sadfsd on Linux/BSD, install https://github.com/libfuse/libfuse

To run sadfsd on macOS, try https://github.com/osxfuse/osxfuse

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

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

To debug with GDB, replace `cmake ..` above with:
```
$ cmake -DCMAKE_CONFIG_TYPE=Debug ..
```

To avoid typing this each time `cmake` is run:
```
$ alias cmake='cmake -DCMAKE_CONFIG_TYPE=Debug'
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

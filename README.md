## ldpaio

ldpaio is an open-source library that integrates PAIO with the the LD_PRELOAD library.


## Getting started with ldpaio

Tutorial to install and test ldpaio.

1. <b>Set up ldpaio</b>
```shell
$ cd ldpaio; mkdir build; cd build;
$ cmake -DCMAKE_INSTALL_PREFIX:PATH=. ..
$ make install -j
$ export PATH_LDPAIO=$PWD
```

2. <b>Setup microbenchmark</b>
```shell
$ cd ldpaio/tests
$ g++ libc_calls_experiment.cpp -o libc_calls_test
$ cd ..
```

3. <b>Run microbenchmark</b>
```shell
$ cd ldpaio
$ LD_PRELOAD=$PATH_LDPAIO/libldpaio.so ./tests/libc_calls_test
```

## Features
...

## Contents
...

## System Requirements
...

## Dependencies
...

## Limitations
...

## Calls currently supported
...

## License
...

## Contributors
* Ricardo Macedo, INESC TEC & U.Minho

## Acknowledgments
...

## Contact
...

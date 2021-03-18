## padll

padll is an open-source library that integrates PAIO with the the LD_PRELOAD library.


## Getting started with padll

Tutorial to install and test padll.

1. <b>Set up padll</b>
```shell
$ cd padll; mkdir build; cd build;
$ cmake -DCMAKE_INSTALL_PREFIX:PATH=. ..
$ make install -j
$ export PATH_PADLL=$PWD
```

2. <b>Setup microbenchmark</b>
```shell
$ cd padll/tests
$ g++ libc_calls_experiment.cpp -o libc_calls_test
$ cd ..
```

3. <b>Run microbenchmark</b>
```shell
$ cd padll
$ LD_PRELOAD=$PATH_PADLL/libpadll.so ./tests/libc_calls_test
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

## PADLL 

padll is an open-source library that integrates PAIO with the the LD_PRELOAD library.
PADLL stands for <b><ins>P</ins></b>rogrammable and <b><ins>A</ins></b>daptable I/O for <b><ins>D</ins></b>ynamically <b><ins>L</ins></b>oaded <b><ins>L</ins></b>ibraries.


## Getting started with padll

Tutorial to install and test padll.

1. <b>Set up padll</b>
```shell
$ cd padll; mkdir build; cd build;
$ cmake ..
$ cmake --build .
$ export PATH_PADLL=$PWD
```

2. <b>Setup microbenchmark</b>
```shell
$ cd padll/tests/posix
$ g++ simple_test.cpp -o simple_test
$ cd ..
```

3. <b>Run microbenchmark</b>
```shell
$ cd padll
$ LD_PRELOAD=$PATH_PADLL/libpadll.so ./tests/posix/simple_test
```

## Features
...

## Contents
...

## System Requirements
...

## Dependencies
* `C++17`
* `g++ 9.3.0`
* `cmake 3.19.0-rc1`
* library dependencies are handled in `CMakeLists.txt`

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

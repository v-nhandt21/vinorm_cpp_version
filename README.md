ICU Version: 64.2
HOST: x86_64-pc-linux-gnu
CC Compiler: gcc
CXX Compiler: g++


# For Ubuntu 18.04

  - Start at folder **VOS_Update_Frontend**
  - Move to src
```sh
$ cd src
```
  - Set LD_LIBRARY_PATH to "lib" folder in current folder
```sh
$ export LD_LIBRARY_PATH=../lib
```
  - Run make main to compile
```sh
$ make main
```
  - Reading text from "input.txt" and print to "output.txt" (create new file input.txt to input, the input.txt must have content)
```sh
$ touch input.txt
$ ./main
```


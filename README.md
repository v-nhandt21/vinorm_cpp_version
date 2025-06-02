ICU Version: 64.2
HOST: x86_64-pc-linux-gnu
CC Compiler: gcc
CXX Compiler: g++


# For Ubuntu 18.04

  - Start at folder **Vinorm_cpp_version**
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

Some example and option for normalization

- Input
```
Có phải tháng 12/2020 đã có vaccine phòng ngừa Covid-19 xmz ?
```
- Output
```sh 
./main
Có phải tháng mười hai năm hai nghìn không trăm hai mươi đã có vaccine phòng ngừa Covid mười chín ích mờ giét .
```
- Lowercase output
```sh 
./main -lower
có phải tháng mười hai năm hai nghìn không trăm hai mươi đã có vaccine phòng ngừa covid mười chín ích mờ giét .
```
- Do not handle unknown words, discard word undefine and do not contain vowel, do not spell word with vowel
```sh 
./main -unknown
Có phải tháng mười hai năm hai nghìn không trăm hai mươi đã có vaccine phòng ngừa Covid mười chín xmz
```
- Keep, do not replace punctuation with dot and coma
```sh 
./main -punc
Có phải tháng mười hai năm hai nghìn không trăm hai mươi đã có vaccine phòng ngừa Covid mười chín ích mờ giét ?
```
- Just get normalization wit Regex, not using Dictionary Checking
```sh 
./main -rule
Có phải tháng mười hai năm hai nghìn không trăm hai mươi đã có vaccine phòng ngừa Covid mười chín xmz ?
```
To access old version, check src(1.0.7), this version handle with undefine words in address code form.

### Acknowledge

This project was conducted by

- Le Tan Dang Tam

- Do Tri Nhan

During our time as collaborators at AILab, HCMUS, under the supervision of Professor Vu Hai Quan.
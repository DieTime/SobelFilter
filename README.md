# Sobel filter

Multithread Sobel filter implementation on C language.

### About:
1. Working only with `PGM - portable graymap format` image
2. Support multithreading using `-J` flag

### Flags:
- `-S` Path to source PGM image `required!`
- `-O` Path to output PGM image `required!`
- `-J` Threads count `optional`

### Usage:
```bash
git clone https://github.com/DieTime/SobelFilter.git
cd SobelFilter/

cmake -S . -B ./cmake-build 
cd ./cmake-build && cmake --build .

./sobel-filter -S <path/to/source.pgm> -O <path/to/output.pgm> -J <threads>
```
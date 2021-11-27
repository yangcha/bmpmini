# BMP reader and writer

A very simple and small C++11 header only BMP reader and writer. It supports 24bit color BMP and 8bit grayscale BMP image.


For grayscale image, 8bit BMP is one third of 24bit BMP in file size. It will save the disk space.

## How to build

```
make build
cd build
cmake ..
``` 

## How to use

It is a single header only library, just include it in your code. There is no dependency on external library. 

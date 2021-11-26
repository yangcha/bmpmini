
#include <iostream>
#include "bmpmini.hpp"

int main(int argc, char* argv[])
{
    using namespace image;
    BMPMini bmp;
    try {
        bmp.read("../samples/lena_color.bmp");
    }
    catch (const std::exception& e) {
        std::cout << e.what() << std::endl;
        return -1;
    }
    bmp.write("lena_gray.bmp");
    return 0;
}

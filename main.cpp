
#include <iostream>
#include "bmpmini.hpp"

int main(int argc, char* argv[])
{
    using namespace image;
    BMPMini bmp;
    try {
        bmp.read("../samples/lena_color.bmp");
        bmp.write("lena_color_copy.bmp");
        auto img = bmp.get();
        img.channels = 1;
        std::vector<uint8_t> gray(img.width * img.height);
        BGR* color = reinterpret_cast<BGR*>(img.data);
        for (size_t i = 0; i < gray.size(); i++) {
            gray[i] = 0.299 * color[i].r + 0.587 * color[i].g + 0.114 * color[i].b;
        }
        img.data = gray.data();
        bmp.write(img, "lena_gray.bmp");
    }
    catch (const std::exception& e) {
        std::cout << e.what() << std::endl;
        return -1;
    }
   
    return 0;
}

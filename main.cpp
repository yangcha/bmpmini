
#include <iostream>
#include <algorithm>
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
        struct BGR {
            uint8_t b{ 0 };
            uint8_t g{ 0 };
            uint8_t r{ 0 };
        };
        BGR* color = reinterpret_cast<BGR*>(img.data);
        std::transform(color, color + gray.size(), gray.begin(),
            [] (const BGR& c) { return (uint8_t)std::min(std::max(0.299 * c.r + 0.587 * c.g + 0.114 * c.b, 0.0), 255.0); });
        img.data = gray.data();
        bmp.write(img, "lena_gray.bmp");
    }
    catch (const std::exception& e) {
        std::cout << e.what() << std::endl;
        return -1;
    }
   
    return 0;
}

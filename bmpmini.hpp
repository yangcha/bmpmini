#ifndef _BMP_CPP_H_
#define _BMP_CPP_H_

#include <vector>
#include <array>
#include <fstream>

namespace image {
#pragma pack(push, 1)
	struct BMPHeader {
		// BMP file header
		uint16_t magic_number{ 0x4D42 }; // magic number: BM
		uint32_t file_size{ 0 };	// in bytes
		uint16_t reserved1{ 0 };	// Reserved
		uint16_t reserved2{ 0 };    // Reserved
		uint32_t offset_data{ 54 };  // Offset of byte where bitmap image data starts

		// BMP information header
		uint32_t header_size{ 40 };	// size of this header, in bytes (40)
		int32_t width{ 0 };         // width in pixels (signed integer)
		int32_t height{ 0 };        // height in pixels (signed integer, positive- bottom-up, negative- top-down)
		uint16_t planes{ 1 };       // number of color planes (must be 1)
		uint16_t bit_per_pixel{ 24 };    // number of bits per pixel. Typical values are 1, 4, 8, 16, 24 and 32
		uint32_t compression_method{ 0 };  // compression method: 0 - uncompressed.
		uint32_t image_size{ 0 };          // size of the raw bitmap data in bytes, 0 - uncompressed images
		int32_t h_pixels_per_meter{ 2835 }; // horizontal resolution of the image
		int32_t v_pixels_per_meter{ 2835 }; // vertical resolution of the image
		uint32_t colors_in_palette{ 0 };    // number of colors in the color palette. Use 0 for the max number of colors allowed by bit_count
		uint32_t important_colors{ 0 };     // number of important colors used, or 0 when every color is important; generally ignored
	};
#pragma pack(pop)


	struct BGRA {
		BGRA() = default;
		BGRA(uint8_t _b, uint8_t _g, uint8_t _r, uint8_t _a) :
			b{ _b }, g{ _g }, r{ _r }, a{ _a } {}
		uint8_t b{ 0 };
		uint8_t g{ 0 };
		uint8_t r{ 0 };
		uint8_t a{ 0 };
	};

	struct ImageView {
		ImageView(int w, int h, int c, uint8_t* d) :
			width{ w }, height{ h }, channels{ c }, data{ d } {}
		int width{ 0 };
		int height{ 0 };
		int channels{ 0 };
		uint8_t* data{ nullptr };
	};

	class BMPMini
	{
	public:
		enum class CompressionMethod {
			BI_RGB,		// no compression
			BI_RLE8,	// RLE 8-bit/pixel
			BI_RLE4,	// RLE 4-bit/pixel
			BI_BITFIELDS	// RGB bit field masks
		};

		void read(const std::string& filename) {
			std::ifstream istrm(filename, std::ios::binary);
			if (!istrm) {
				throw std::runtime_error("Cannot open the input file: " + filename);
			}

			istrm.read(reinterpret_cast<char*>(&header), sizeof BMPHeader);
			istrm.seekg(header.offset_data, std::ios::beg);

			int row_size = ((header.width * header.bit_per_pixel + 31) / 32) * 4;
			int image_size = header.height * row_size;
			std::vector< uint8_t> data(image_size);
			istrm.read(reinterpret_cast<char*>(data.data()), data.size());

			pixel_data.reserve((header.width * header.height * header.bit_per_pixel) / 8);
			for (int i = 0; i < header.height; i++) {
				auto k = header.height - 1 - i;
				auto ptr = reinterpret_cast<uint8_t*>(data.data()) + k * row_size;
				pixel_data.insert(pixel_data.end(), ptr, ptr+ (header.width* header.bit_per_pixel)/8);
			}
		}

		void write(int width, int height, int channels, const uint8_t* data,
			const std::string& filename) {
			header.width = width;
			header.height = height;
			header.bit_per_pixel = channels * 8;
			int row_size = ((header.width * header.bit_per_pixel + 31) / 32) * 4;
			header.image_size = header.height * row_size;
			header.file_size = header.offset_data + header.image_size;

			std::vector<uint8_t> image_data(header.image_size);
			for (int i = 0; i < header.height; i++) {
				auto k = header.height - 1 - i;
				auto ptr = &data[k * width * channels];
				std::copy(ptr, ptr + width * channels, &image_data[i*row_size]);
			}

			std::ofstream ostrm{ filename, std::ios_base::binary };
			ostrm.write((const char*)&header, sizeof(header));
			ostrm.write((const char*)image_data.data(), image_data.size());
		}

		void write8(ImageView raw_image,
			const std::string& filename) {
			header.width = raw_image.width;
			header.height = raw_image.height;
			header.bit_per_pixel = 8;
			int row_size = ((header.width * header.bit_per_pixel + 31) / 32) * 4;
			header.image_size = header.height * row_size;
			header.colors_in_palette = 0;
			std::vector<BGRA> color_palette;
			for (unsigned i = 0; i < 256; i++) {
				color_palette.emplace_back(i, i, i, 0);
			}
			header.file_size = header.offset_data + color_palette.size() + header.image_size;
			header.offset_data = 54+color_palette.size();
			std::vector<uint8_t> image_data(header.image_size);
			for (int i = 0; i < header.height; i++) {
				auto k = header.height - 1 - i;
				auto ptr = &raw_image.data[k * raw_image.width * raw_image.channels];
				for (int j = 0; j < raw_image.width; j++) {
					image_data[i * row_size + j] = ptr[j * raw_image.channels];
				}
			}

			std::ofstream ostrm{ filename, std::ios_base::binary };
			ostrm.write((const char*)&header, sizeof(header));
			ostrm.write((const char*)color_palette.data(), color_palette.size()*4);
			ostrm.write((const char*)image_data.data(), image_data.size());
		}

		void write(const std::string& filename) {
			write8({ header.width, header.height, header.bit_per_pixel / 8, pixel_data.data() }, filename);
		}

	private:
		BMPHeader header;
		std::vector<uint8_t> pixel_data;
	};

}
#endif

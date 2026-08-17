#include "arma3il/image_decoder.h"

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_STATIC
#include "stb_image.h"

#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "stb_image_resize2.h"



namespace arma3il {
    int nextPowerOfTwo(int value) {
        int power = 1;
        while (power < value) power <<= 1;
        return power;
    }

    std::shared_ptr<DecodedImage> ImageDecoder::decodeFile(const std::string& filePath) {
        int w = 0, h = 0, channels = 0;
        unsigned char* pixels = stbi_load(filePath.c_str(), &w, &h, &channels, 4);
        if (!pixels) return nullptr;

        const int targetW = nextPowerOfTwo(w);
        const int targetH = nextPowerOfTwo(h);

        auto image = std::make_shared<DecodedImage>();
        image->width = targetW;
        image->height = targetH;
        image->origWidth = w;
        image->origHeight = h;
        image->rgba.resize(static_cast<size_t>(targetW) * targetH * 4);

        // sRGB (pas "_linear") : les JPEG standards sont encodés en sRGB, il faut
        // que le resize en tienne compte pour ne pas fausser les couleurs/le contraste.
        stbir_resize_uint8_srgb(
            pixels, w, h, 0,
            image->rgba.data(), targetW, targetH, 0,
            STBIR_RGBA
        );

        stbi_image_free(pixels);
        return image;
    }
} // namespace arma3il
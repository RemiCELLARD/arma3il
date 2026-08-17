#pragma once
#include <memory>
#include <string>
#include "arma3il/texture_cache.h"

namespace arma3il {

    /**
     * @class ImageDecoder
     * @brief Decodes image files on disk into raw RGBA8 pixel buffers.
     *
     * Wraps stb_image (JPEG/PNG/etc. decoding) and stb_image_resize2 (sRGB-aware
     * resizing). Arma 3's procedural texture system requires power-of-two
     * dimensions, so the decoded image is always resized up to the next
     * power-of-two width and height; the original (pre-resize) dimensions are
     * preserved separately in DecodedImage for use in UI layout calculations.
     */
    class ImageDecoder {
    public:
        /**
         * @brief Decodes an image file into a power-of-two RGBA8 buffer.
         *
         * Reads the file at filePath, decodes it via stb_image (format is
         * auto-detected from file content, not from its extension), then
         * resizes it up to the next power-of-two dimensions using sRGB-correct
         * resampling.
         *
         * @param filePath Path to the image file on disk.
         * @return A populated DecodedImage on success, or nullptr if the file
         *         could not be read or decoded.
         */
        static std::shared_ptr<DecodedImage> decodeFile(const std::string& filePath);
    };

} // namespace arma3il
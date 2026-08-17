#pragma once
#include <string>
#include <vector>
#include <cstdint>
#include <mutex>
#include <memory>
#include <unordered_map>

namespace arma3il {

    /**
     * @struct DecodedImage
     * @brief A fully decoded image, ready to be served to RVExtensionFillTextureSource.
     */
    struct DecodedImage {
        int width = 0;        ///< Power-of-two width of the pixel buffer, in pixels.
        int height = 0;        ///< Power-of-two height of the pixel buffer, in pixels.
        int origWidth = 0;     ///< Original (pre-resize) width of the source image, in pixels.
        int origHeight = 0;    ///< Original (pre-resize) height of the source image, in pixels.
        std::vector<uint8_t> rgba; ///< Pixel data, 4 bytes per pixel (R,G,B,A), row-major, top-to-bottom, width*height*4 bytes total.
    };

    /**
     * @class TextureCache
     * @brief Thread-safe, in-memory store of decoded images, keyed by uniqueName.
     *
     * RVExtensionFillTextureSource is called by the engine on an arbitrary
     * (rendering-related) thread whenever it needs pixel data for a texture
     * previously referenced from SQF via an
     * `#(rgb,W,H,mips)extension("arma3il","uniqueName")` string. This cache
     * holds the decoded pixel buffers so that call can be answered
     * synchronously and instantly, without touching disk or network.
     *
     * Entries are never evicted automatically: callers are responsible for
     * calling remove()/clear() (e.g. when a displayed image dialog is closed)
     * to avoid unbounded memory growth over a long play session.
     */
    class TextureCache {
    public:
        /**
         * @brief Returns the process-wide singleton instance.
         * @return Reference to the single TextureCache instance.
         */
        static TextureCache& instance();

        /**
         * @brief Stores (or replaces) a decoded image under the given key.
         *
         * @param uniqueName Key under which the image will be retrievable
         *                    (must match the uniqueName used in the SQF
         *                    procedural texture string).
         * @param image      Decoded image to store.
         */
        void store(const std::string& uniqueName, std::shared_ptr<DecodedImage> image);

        /**
         * @brief Retrieves a previously stored decoded image.
         *
         * @param uniqueName Key the image was stored under.
         * @return Shared pointer to the DecodedImage, or nullptr if not found.
         */
        std::shared_ptr<DecodedImage> get(const std::string& uniqueName) const;

        /**
         * @brief Removes a single entry from the cache, freeing its memory.
         *
         * @param uniqueName Key of the entry to remove.
         */
        void remove(const std::string& uniqueName);

        /**
         * @brief Removes all entries from the cache, freeing all associated memory.
         */
        void clear();

    private:
        mutable std::mutex m_mutex; ///< Guards access to m_images.
        std::unordered_map<std::string, std::shared_ptr<DecodedImage>> m_images; ///< Decoded images keyed by uniqueName.
    };

} // namespace arma3il
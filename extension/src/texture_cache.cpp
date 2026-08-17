#include "arma3il/texture_cache.h"

namespace arma3il {

    TextureCache& TextureCache::instance() {
        static TextureCache cache;
        return cache;
    }

    void TextureCache::store(const std::string& uniqueName, std::shared_ptr<DecodedImage> image) {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_images[uniqueName] = std::move(image);
    }

    std::shared_ptr<DecodedImage> TextureCache::get(const std::string& uniqueName) const {
        std::lock_guard<std::mutex> lock(m_mutex);
        auto it = m_images.find(uniqueName);
        return it != m_images.end() ? it->second : nullptr;
    }

    void TextureCache::remove(const std::string& uniqueName) {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_images.erase(uniqueName);
    }

    void TextureCache::clear() {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_images.clear();
    }

} // namespace arma3il
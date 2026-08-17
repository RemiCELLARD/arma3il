#pragma once
#include <string>

namespace arma3il {

    /**
     * @class HashUtil
     * @brief Deterministic, non-cryptographic hashing utility.
     *
     * Used to turn an arbitrary string (typically an image URL) into a stable
     * identifier composed exclusively of [0-9a-f] characters — safe to use as a
     * filename, as a uniqueName for RVExtensionFillTextureSource, and as a
     * TextureCache key, without any additional sanitization.
     *
     * This is not intended for security-sensitive use cases (no collision
     * resistance guarantees beyond what FNV-1a naturally provides); it exists
     * purely to derive stable, filesystem/classname-safe identifiers.
     */
    class HashUtil {
    public:
        /**
         * @brief Computes a 64-bit FNV-1a hash of the input and returns it as hex.
         *
         * The same input always produces the same output. Two different inputs
         * are extremely unlikely to collide for the volume of URLs this
         * extension is expected to handle.
         *
         * @param input Arbitrary string to hash (typically a URL).
         * @return A 16-character lowercase hexadecimal string.
         */
        static std::string hashToHex(const std::string& input);
    };

} // namespace arma3il
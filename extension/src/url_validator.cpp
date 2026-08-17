#include "arma3il/url_validator.h"
#include <algorithm>
#include <array>
#include <fstream>
#include <cctype>

namespace arma3il {

    std::string UrlValidator::toLower(std::string s) {
        std::transform(s.begin(), s.end(), s.begin(),
            [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
        return s;
    }

    std::string UrlValidator::stripQueryString(const std::string& url) {
        auto pos = url.find_first_of("?#");
        return pos == std::string::npos ? url : url.substr(0, pos);
    }

    bool UrlValidator::hasAllowedExtension(const std::string& url) {
        static const std::array<std::string, 7> allowed = {
            ".jpg", ".jpeg", ".jpe", ".jfif", ".jif", ".png"
        };
        const std::string cleaned = toLower(stripQueryString(url));

        for (const auto& ext : allowed) {
            if (cleaned.size() >= ext.size() &&
                cleaned.compare(cleaned.size() - ext.size(), ext.size(), ext) == 0) {
                return true;
            }
        }
        return false;
    }

    bool UrlValidator::hasValidImageSignature(const std::string& localFilePath) {
        std::ifstream file(localFilePath, std::ios::binary);
        if (!file) return false;

        unsigned char header[8] = { 0 };
        file.read(reinterpret_cast<char*>(header), sizeof(header));
        if (file.gcount() < 3) return false;

        // JPEG : FF D8 FF
        if (header[0] == 0xFF && header[1] == 0xD8 && header[2] == 0xFF) {
            return true;
        }

        // PNG : 89 50 4E 47 0D 0A 1A 0A (8 octets)
        static const unsigned char pngMagic[8] = {
            0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A
        };
        if (file.gcount() == 8 && std::equal(std::begin(pngMagic), std::end(pngMagic), header)) {
            return true;
        }

        return false;
    }

} // namespace arma3il
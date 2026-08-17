#include "arma3il/url_validator.h"
#include <cstdio>
#include <fstream>
#include <filesystem>

extern int g_failures;
#define CHECK(cond) do { \
    if (!(cond)) { \
        std::printf("FAIL: %s (%s:%d)\n", #cond, __FILE__, __LINE__); \
        ++g_failures; \
    } \
} while (0)

namespace {

    // Creates a temporary file with a given binary header, for testing purposes
    // to test `hasValidImageSignature` without relying on an actual file on the disk.
    std::string makeTempFile(const std::vector<unsigned char>& bytes) {
        auto path = std::filesystem::temp_directory_path() / "arma3il_test_sig.bin";
        std::ofstream f(path, std::ios::binary);
        f.write(reinterpret_cast<const char*>(bytes.data()), bytes.size());
        f.close();
        return path.string();
    }

} // namespace

void run_url_validator_tests() {
    using arma3il::UrlValidator;

    // --- Permitted extensions ---
    CHECK(UrlValidator::hasAllowedExtension("https://x.com/a.jpg"));
    CHECK(UrlValidator::hasAllowedExtension("https://x.com/a.JPEG"));
    CHECK(UrlValidator::hasAllowedExtension("https://x.com/a.jpe"));
    CHECK(UrlValidator::hasAllowedExtension("https://x.com/a.jfif"));
    CHECK(UrlValidator::hasAllowedExtension("https://x.com/a.jif"));
    CHECK(UrlValidator::hasAllowedExtension("https://x.com/a.jpg?width=200&v=2"));

    // --- Rejected extensions ---
    CHECK(!UrlValidator::hasAllowedExtension("https://x.com/a.exe"));
    CHECK(!UrlValidator::hasAllowedExtension("https://x.com/a.png"));
    CHECK(!UrlValidator::hasAllowedExtension("https://x.com/a.paa"));
    CHECK(!UrlValidator::hasAllowedExtension("https://x.com/a"));

    // --- Real binary signature ---
    const std::vector<unsigned char> validJpeg = { 0xFF, 0xD8, 0xFF, 0xE0, 0x00, 0x10 };
    const std::vector<unsigned char> invalidHeader = { 0x89, 0x50, 0x4E, 0x47 }; // PNG

    const std::string validPath = makeTempFile(validJpeg);
    const std::string invalidPath = makeTempFile(invalidHeader);

    CHECK(UrlValidator::hasValidImageSignature(validPath));
    CHECK(!UrlValidator::hasValidImageSignature(invalidPath));
    CHECK(!UrlValidator::hasValidImageSignature("path_that_does_not_exist.jpg"));

    std::remove(validPath.c_str());
    std::remove(invalidPath.c_str());
}
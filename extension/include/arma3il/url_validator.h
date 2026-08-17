#pragma once
#include <string>

namespace arma3il {

    /**
     * @class UrlValidator
     * @brief Utility functions to validate an image URL and its downloaded content.
     *
     * Used to secure file downloads by checking both the extension declared in
     * the URL and the actual signature (magic bytes) of the downloaded file.
     * This avoids relying solely on the filename, which could otherwise be
     * misleading (e.g. a malicious server serving non-image content under a
     * ".jpg" name).
     */
    class UrlValidator {
    public:
        /**
         * @brief Checks whether the URL ends with an allowed image extension.
         *
         * Strips any query string, lowercases the result, then checks it
         * against the list of allowed image extensions (.jpg, .jpeg, .jpe,
         * .jfif, .jif, .png).
         *
         * @param url Full URL to analyze.
         * @return true if the extension is allowed, false otherwise.
         */
        static bool hasAllowedExtension(const std::string& url);

        /**
         * @brief Verifies the real signature (magic bytes) of a downloaded file.
         *
         * Confirms the file is genuinely an image, independent of its name or
         * declared extension, by reading its first bytes and comparing them
         * against known signatures for supported image formats (JPEG, PNG).
         *
         * @param localFilePath Path to the downloaded file on disk.
         * @return true if the signature matches a supported image format, false otherwise.
         */
        static bool hasValidImageSignature(const std::string& localFilePath);

    private:
        /**
         * @brief Converts a string to lowercase.
         * @param s String to convert.
         * @return The lowercase copy of the input string.
         */
        static std::string toLower(std::string s);

        /**
         * @brief Strips the query string (and fragment) from a URL.
         *
         * Example: "https://site.com/image.png?token=123" becomes
         * "https://site.com/image.png".
         *
         * @param url URL to clean.
         * @return The URL without its query string/fragment.
         */
        static std::string stripQueryString(const std::string& url);
    };

} // namespace arma3il
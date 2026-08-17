#pragma once
#include <string>
#include <functional>

namespace arma3il {

    /**
     * @class Downloader
     * @brief Performs asynchronous HTTPS downloads of image files via libcurl.
     *
     * Each download runs on a detached background thread so that RVExtensionArgs
     * can return immediately to the SQF caller without blocking the game.
     * The downloaded file is saved to a temporary local path (derived from a
     * hash of the URL) and is expected to be deleted by the caller once decoded
     * into memory — Downloader itself only guarantees the file exists and is a
     * plausible image while the callback runs.
     */
    class Downloader {
    public:
        /**
         * @brief Callback invoked once a download attempt has finished (success or failure).
         *
         * @param success   true if the file was downloaded and passed basic validation.
         * @param localPath Absolute path to the downloaded file on success; empty on failure.
         * @param error     Short machine-readable error code on failure; empty on success.
         */
        using ResultCallback = std::function<void(bool success, const std::string& localPath, const std::string& error)>;

        /**
         * @brief Starts an asynchronous download of the given URL.
         *
         * Spawns a detached background thread that performs the HTTPS request,
         * validates the response, and invokes cb() with the result. Enforces
         * HTTPS only, disables automatic redirects, and caps the response size
         * at 5 MB.
         *
         * @param url       Fully-qualified HTTPS URL to download.
         * @param outputDir Directory in which to write the temporary file. If
         *                  empty, a default directory under the user's
         *                  Documents folder is used (created if missing).
         * @param cb        Callback invoked exactly once, from the background
         *                  thread, once the attempt has finished.
         */
        static void downloadAsync(const std::string& url, const std::string& outputDir, ResultCallback cb);

    private:
        /**
         * @brief Builds the local file path used to store a download in progress.
         *
         * The filename is derived from a hash of the URL (see HashUtil), so it
         * is always filesystem-safe regardless of the URL's actual content.
         *
         * @param url       URL being downloaded (used to derive the filename).
         * @param outputDir Target directory; if empty, falls back to the default directory.
         * @return Absolute path where the file should be written.
         */
        static std::string buildLocalPath(const std::string& url, const std::string& outputDir);

        /**
         * @brief Returns (and creates if necessary) the default download directory.
         *
         * @return Absolute path to a writable directory under the user's Documents folder.
         */
        static std::string getDefaultDownloadDirectory();
    };

} // namespace arma3il
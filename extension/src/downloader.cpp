#include "arma3il/downloader.h"
#include "arma3il/url_validator.h"
#include "arma3il/hash_util.h"
#include "arma3il/logger.h"

#include <curl/curl.h>
#include <thread>
#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <sstream>

#if defined(_WIN32)
#include <windows.h>
#include <shlobj.h>
#endif

namespace fs = std::filesystem;

namespace arma3il {

    std::string Downloader::getDefaultDownloadDirectory() {
#if defined(_WIN32)
        PWSTR path = nullptr;
        std::string result;
        if (SUCCEEDED(SHGetKnownFolderPath(FOLDERID_Documents, 0, nullptr, &path))) {
            std::wstring wpath(path);
            CoTaskMemFree(path);
            result = std::string(wpath.begin(), wpath.end()) + "\\Arma 3\\arma3il_downloads";
        }
        else {
            result = "arma3il_downloads";
        }
#else
        const char* tmp = std::getenv("TMPDIR");
        std::string result = std::string(tmp ? tmp : "/tmp") + "/arma3il_downloads";
#endif
        std::error_code ec;
        fs::create_directories(result, ec);
        return result;
    }

    std::string Downloader::buildLocalPath(const std::string& url, const std::string& outputDir) {
        std::string dir = outputDir.empty() ? getDefaultDownloadDirectory() : outputDir;

        std::error_code ec;
        fs::create_directories(dir, ec);

        std::ostringstream oss;
        oss << dir << "\\" << HashUtil::hashToHex(url) << ".img";
        return oss.str();
    }

    namespace {
        size_t writeToFile(void* ptr, size_t size, size_t nmemb, void* stream) {
            return fwrite(ptr, size, nmemb, static_cast<FILE*>(stream));
        }
    }

    void Downloader::downloadAsync(const std::string& url, const std::string& outputDir, ResultCallback cb) {
        std::thread([url, outputDir, cb]() {
            const std::string localPath = buildLocalPath(url, outputDir);

            FILE* fp = nullptr;
#if defined(_WIN32)
            fopen_s(&fp, localPath.c_str(), "wb");
#else
            fp = fopen(localPath.c_str(), "wb");
#endif
            if (!fp) {
                Logger::error("Downloader: cannot be opened " + localPath);
                cb(false, "", "cannot_open_local_file");
                return;
            }

            CURL* curl = curl_easy_init();
            if (!curl) {
                fclose(fp);
                Logger::error("Downloader: curl_easy_init failed");
                cb(false, "", "curl_init_failed");
                return;
            }

            curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeToFile);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
            curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 0L);
            curl_easy_setopt(curl, CURLOPT_TIMEOUT, 15L);
            curl_easy_setopt(curl, CURLOPT_MAXFILESIZE, 5L * 1024 * 1024);
            curl_easy_setopt(curl, CURLOPT_PROTOCOLS_STR, "https");
            curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 1L);
            curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 2L);

            CURLcode res = curl_easy_perform(curl);
            long httpCode = 0;
            curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &httpCode);
            curl_easy_cleanup(curl);
            fclose(fp);

            if (res != CURLE_OK) {
                std::remove(localPath.c_str());
                const std::string errorMsg = std::string("curl_error_") + curl_easy_strerror(res);
                Logger::warn("Downloader: transport error for " + url + " (" + errorMsg + ")");
                cb(false, "", errorMsg);
                return;
            }

            if (httpCode < 200 || httpCode >= 300) {
                std::remove(localPath.c_str());
                const std::string errorMsg = "http_" + std::to_string(httpCode);
                Logger::warn("Downloader: unexpected HTTP status for " + url + " (" + errorMsg + ")");
                cb(false, "", errorMsg);
                return;
            }

            if (!UrlValidator::hasValidImageSignature(localPath)) {
                std::remove(localPath.c_str());
                Logger::warn("Downloader: invalid signature for " + url);
                cb(false, "", "invalid_file_signature");
                return;
            }

            cb(true, localPath, "");
            }).detach();
    }

} // namespace arma3il
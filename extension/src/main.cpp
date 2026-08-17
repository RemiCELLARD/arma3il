#include "arma3il/extension_api.h"
#include "arma3il/url_validator.h"
#include "arma3il/whitelist.h"
#include "arma3il/downloader.h"
#include "arma3il/callback_manager.h"
#include "arma3il/texture_cache.h"
#include "arma3il/image_decoder.h"
#include "arma3il/logger.h"
#include "arma3il/client_whitelist.h"
#include "arma3il/hash_util.h"
#include "arma3il/sqf_format.h"

#include <mutex>
#include <algorithm>
#include <cstring>
#include <cstdio>
#include <cstdint>
#include <cctype>
#include <string>
#include <vector>
#include <unordered_map>
#include <functional>

namespace {

    arma3il::Whitelist g_whitelist;
    arma3il::CallbackManager g_callbackMgr;
    std::once_flag g_clientWhitelistInitFlag;

    void ensureClientWhitelistLoaded() {
        std::call_once(g_clientWhitelistInitFlag, []() {
            arma3il::ClientWhitelist::instance().loadOrCreate();
            });
    }

    std::string unquoteSqfArg(const std::string& raw) {
        if (raw.size() >= 2 && raw.front() == '"' && raw.back() == '"') {
            const std::string inner = raw.substr(1, raw.size() - 2);
            std::string result;
            result.reserve(inner.size());
            for (size_t i = 0; i < inner.size(); ++i) {
                if (inner[i] == '"' && i + 1 < inner.size() && inner[i + 1] == '"') {
                    result += '"';
                    ++i;
                }
                else {
                    result += inner[i];
                }
            }
            return result;
        }
        return raw;
    }

    std::vector<std::string> splitArgs(const char** argv, int argc) {
        std::vector<std::string> result;
        result.reserve(static_cast<size_t>(argc));
        for (int i = 0; i < argc; ++i) result.emplace_back(unquoteSqfArg(argv[i]));
        return result;
    }

    void safeCopy(char* output, int outputSize, const std::string& text) {
#if defined(_WIN32)
        strncpy_s(output, static_cast<size_t>(outputSize), text.c_str(), _TRUNCATE);
#else
        std::strncpy(output, text.c_str(), static_cast<size_t>(outputSize) - 1);
        output[outputSize - 1] = '\0';
#endif
    }

    enum TextureFormat : uint32_t {
        PacAI88 = 1,
        PacARGB8888 = 5
    };

    /// Result of a command handler: the SQF array literal to return, plus the
    /// native RVExtensionArgs exit code (0 = success, 1 = error).
    struct CommandResult {
        std::string payload;
        int code;
    };

    using CommandHandler = std::function<CommandResult(const std::vector<std::string>&)>;

    // --- Individual command handlers -------------------------------------------------
    CommandResult handleWhitelistAdd(const std::vector<std::string>& args) {
        g_whitelist.addHosts(args);
        return { arma3il::SqfArrayBuilder().addString("ok").build(), 0 };
    }

    CommandResult handleWhitelistClear(const std::vector<std::string>&) {
        g_whitelist.clear();
        return { arma3il::SqfArrayBuilder().addString("ok").build(), 0 };
    }

    CommandResult handleWhitelistClientReload(const std::vector<std::string>&) {
        arma3il::ClientWhitelist::instance().loadOrCreate();
        return { arma3il::SqfArrayBuilder().addString("ok").build(), 0 };
    }

    CommandResult handleTextureCacheClear(const std::vector<std::string>& args) {
        if (!args.empty()) {
            arma3il::TextureCache::instance().remove(args[0]);
        }
        else {
            arma3il::TextureCache::instance().clear();
        }
        return { arma3il::SqfArrayBuilder().addString("ok").build(), 0 };
    }

    CommandResult handleDownload(const std::vector<std::string>& args) {
        if (args.empty()) {
            return {
                arma3il::SqfArrayBuilder().addString("error").addString("").addString("missing_args").build(),
                1
            };
        }

        const std::string& url = args[0];
        const std::string uniqueName = arma3il::HashUtil::hashToHex(url);

        if (!arma3il::UrlValidator::hasAllowedExtension(url)) {
            return {
                arma3il::SqfArrayBuilder().addString("error").addString(uniqueName).addString("invalid_extension").build(),
                1
            };
        }

        if (!g_whitelist.isAllowed(url)) {
            return {
                arma3il::SqfArrayBuilder().addString("error").addString(uniqueName).addString("not_whitelisted_mission").build(),
                1
            };
        }

        std::string host, scheme;
        arma3il::Whitelist::extractHost(url, host, scheme);
        std::transform(host.begin(), host.end(), host.begin(),
            [](unsigned char c) { return static_cast<char>(std::tolower(c)); });

        if (!arma3il::ClientWhitelist::instance().contains(host)) {
            return {
                arma3il::SqfArrayBuilder().addString("error").addString(uniqueName).addString("not_whitelisted_client").build(),
                1
            };
        }

        arma3il::Downloader::downloadAsync(url, "",
            [uniqueName](bool success, const std::string& localPath, const std::string& error) {
                if (!success) {
                    g_callbackMgr.fire("arma3il", "download",
                        arma3il::SqfArrayBuilder().addString("error").addString(uniqueName).addString(error).build());
                    return;
                }

                auto image = arma3il::ImageDecoder::decodeFile(localPath);
                std::remove(localPath.c_str());

                if (!image) {
                    arma3il::Logger::error("Decode failed for " + uniqueName);
                    g_callbackMgr.fire("arma3il", "download",
                        arma3il::SqfArrayBuilder().addString("error").addString(uniqueName).addString("decode_failed").build());
                    return;
                }

                arma3il::TextureCache::instance().store(uniqueName, image);
                arma3il::Logger::log("TextureCache store: uniqueName=" + uniqueName +
                    " width=" + std::to_string(image->width) + " height=" + std::to_string(image->height));

                const std::string payload = arma3il::SqfArrayBuilder()
                    .addString("done")
                    .addString(uniqueName)
                    .addNumber(image->width)
                    .addNumber(image->height)
                    .addNumber(image->origWidth)
                    .addNumber(image->origHeight)
                    .build();
                g_callbackMgr.fire("arma3il", "download", payload);
            });

        return {
            arma3il::SqfArrayBuilder().addString("loading").addString(uniqueName).build(),
            0
        };
    }

    // --- Dispatch table ---------------------------------------------------------------
    const std::unordered_map<std::string, CommandHandler>& commandTable() {
		static const std::unordered_map<std::string, CommandHandler> table = {
			{ "whitelist_add",           handleWhitelistAdd },
			{ "whitelist_clear",         handleWhitelistClear },
			{ "whitelist_client_reload", handleWhitelistClientReload },
			{ "texture_cache_clear",     handleTextureCacheClear },
			{ "download",                handleDownload },
		};
        return table;
    }

} // namespace

ARMA3IL_EXPORT void ARMA3IL_CALL RVExtensionVersion(char* output, int outputSize) {
    safeCopy(output, outputSize, "arma3il v1.0.0");
}

ARMA3IL_EXPORT void ARMA3IL_CALL RVExtension(char* output, int outputSize, const char* /*function*/) {
    safeCopy(output, outputSize, "use callExtension [\"function\", [args]] instead");
}

ARMA3IL_EXPORT int ARMA3IL_CALL RVExtensionArgs(
    char* output, int outputSize,
    const char* function, const char** argv, int argc)
{
    const std::string fn = function ? function : "";
    const std::vector<std::string> args = splitArgs(argv, argc);

    const auto& table = commandTable();
    const auto it = table.find(fn);
    if (it == table.end()) {
        safeCopy(output, outputSize, arma3il::SqfArrayBuilder().addString("error").addString("").addString("unknown_function").build());
        return 1;
    }

    const CommandResult result = it->second(args);
    safeCopy(output, outputSize, result.payload);
    return result.code;
}

ARMA3IL_EXPORT void ARMA3IL_CALL RVExtensionRegisterCallback(RVExtensionCallback fnc) {
    arma3il::Logger::warn("RVExtensionRegisterCallback called");
    ensureClientWhitelistLoaded();
    g_callbackMgr.setCallback(fnc);
}

ARMA3IL_EXPORT void ARMA3IL_CALL RVExtensionFillTextureSource(
    const char* uniqueName, uint16_t width, uint16_t height,
    uint16_t pitch, uint8_t /*mipLevel*/, TextureFormat format, void* buffer)
{
    if (format != PacARGB8888 || buffer == nullptr) return;

    uint8_t* base = static_cast<uint8_t*>(buffer);
    auto image = arma3il::TextureCache::instance().get(uniqueName ? uniqueName : "");

    const bool valid = image
        && image->width == static_cast<int>(width)
        && image->height == static_cast<int>(height);

    if (!valid) {
        arma3il::Logger::warn("FillTextureSource: texture not found or size mismatch for " +
            std::string(uniqueName ? uniqueName : "null"));
        for (uint16_t y = 0; y < height; ++y) {
            std::memset(base + static_cast<size_t>(y) * pitch, 0, static_cast<size_t>(width) * 4);
        }
        return;
    }

    for (uint16_t y = 0; y < height; ++y) {
        const uint8_t* srcRow = image->rgba.data() + static_cast<size_t>(y) * width * 4;
        uint32_t* dstRow = reinterpret_cast<uint32_t*>(base + static_cast<size_t>(y) * pitch);
        for (uint16_t x = 0; x < width; ++x) {
            const uint8_t r = srcRow[x * 4 + 0];
            const uint8_t g = srcRow[x * 4 + 1];
            const uint8_t b = srcRow[x * 4 + 2];
            const uint8_t a = srcRow[x * 4 + 3];
            dstRow[x] = (static_cast<uint32_t>(a) << 24) |
                (static_cast<uint32_t>(b) << 16) |
                (static_cast<uint32_t>(g) << 8) |
                r;
        }
    }
}

ARMA3IL_EXPORT bool ARMA3IL_CALL RVExtensionHasTextureSourceUpdate(const char* /*uniqueName*/) {
    return false;
}
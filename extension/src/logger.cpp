#include "arma3il/logger.h"

#if defined(_WIN32) && !defined(NDEBUG)
#include <windows.h>
#endif

namespace arma3il {
    void write(const char* level, const std::string& message) {
        #if defined(_WIN32) && !defined(NDEBUG)
            OutputDebugStringA(("[arma3il][" + std::string(level) + "] " + message + "\n").c_str());
        #else
            // Build Release : logging off
            (void)level;
            (void)message;
        #endif
    }

    void Logger::warn(const std::string& message) { write("WARN", message); }
    void Logger::error(const std::string& message) { write("ERROR", message); }
    void Logger::log(const std::string& message) { write("LOG", message); }
} // namespace arma3il
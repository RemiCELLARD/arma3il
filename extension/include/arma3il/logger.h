#pragma once
#include <string>

namespace arma3il {

    /**
     * @class Logger
     * @brief Minimal logging facility based on OutputDebugString.
     *
     * Since this code runs inside a DLL loaded by the game (no console
     * attached), log messages are written via OutputDebugStringA and can be
     * observed in real time with tools such as Sysinternals DebugView. Intended
     * for diagnosing issues during development, not for verbose per-call
     * logging in production.
     */
    class Logger {
    public:
        /**
         * @brief Logs an informational message (tagged "LOG").
         * @param message Message to log.
         */
        static void log(const std::string& message);

        /**
         * @brief Logs a warning message (tagged "WARN").
         * @param message Message to log.
         */
        static void warn(const std::string& message);

        /**
         * @brief Logs an error message (tagged "ERROR").
         * @param message Message to log.
         */
        static void error(const std::string& message);
    };

} // namespace arma3il
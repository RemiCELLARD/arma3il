#pragma once
#include <mutex>
#include <string>
#include "arma3il/extension_api.h"

namespace arma3il {

    /**
     * @class CallbackManager
     * @brief Thread-safe holder for the RVExtensionCallback registered by the engine.
     *
     * The RV Engine calls RVExtensionRegisterCallback exactly once when the
     * extension is loaded, handing over a function pointer the extension can
     * use at any later point (including from background threads) to push
     * asynchronous results back into SQF via the "ExtensionCallback" mission
     * event handler. This class stores that pointer safely and exposes a single
     * fire() method to invoke it.
     */
    class CallbackManager {
    public:
        /**
         * @brief Stores the callback pointer provided by the engine.
         *
         * Called once, from RVExtensionRegisterCallback. Thread-safe.
         *
         * @param cb Function pointer supplied by the RV Engine.
         */
        void setCallback(RVExtensionCallback cb);

        /**
         * @brief Invokes the registered callback, if any.
         *
         * Safe to call from any thread (e.g. the background thread performing
         * an HTTP download). If no callback has been registered yet, this is a
         * no-op rather than a crash.
         *
         * @param name     Extension name to report to SQF (e.g. "arma3il").
         * @param function Logical function name (e.g. "download").
         * @param data     Payload string delivered to the "ExtensionCallback" event handler.
         */
        void fire(const std::string& name, const std::string& function, const std::string& data);

    private:
        std::mutex m_mutex;               ///< Guards access to m_callback.
        RVExtensionCallback m_callback = nullptr; ///< Callback pointer registered by the engine, or nullptr if none yet.
    };

} // namespace arma3il
#include "arma3il/callback_manager.h"

namespace arma3il {

    void CallbackManager::setCallback(RVExtensionCallback cb) {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_callback = cb;
    }

    void CallbackManager::fire(const std::string& name, const std::string& function, const std::string& data) {
        std::lock_guard<std::mutex> lock(m_mutex);
        if (m_callback) {
            m_callback(name.c_str(), function.c_str(), data.c_str());
        }
    }

} // namespace arma3il
#include "arma3il/whitelist.h"
#include <algorithm>
#include <cctype>
#include <curl/curl.h>

namespace arma3il {

    std::string Whitelist::toLower(std::string s) {
        std::transform(s.begin(), s.end(), s.begin(),
            [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
        return s;
    }

    void Whitelist::addHost(const std::string& host) {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_hosts.insert(toLower(host));
    }

    void Whitelist::addHosts(const std::vector<std::string>& hosts) {
        std::lock_guard<std::mutex> lock(m_mutex);
        for (const auto& h : hosts) m_hosts.insert(toLower(h));
    }

    void Whitelist::removeHost(const std::string& host) {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_hosts.erase(toLower(host));
    }

    void Whitelist::clear() {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_hosts.clear();
    }

    std::vector<std::string> Whitelist::listHosts() const {
        std::lock_guard<std::mutex> lock(m_mutex);
        return std::vector<std::string>(m_hosts.begin(), m_hosts.end());
    }

    bool Whitelist::extractHost(const std::string& url, std::string& outHost, std::string& outScheme) {
        CURLU* h = curl_url();
        if (!h) return false;

        bool ok = false;
        if (curl_url_set(h, CURLUPART_URL, url.c_str(), 0) == CURLUE_OK) {
            char* hostPart = nullptr;
            char* schemePart = nullptr;
            if (curl_url_get(h, CURLUPART_HOST, &hostPart, 0) == CURLUE_OK &&
                curl_url_get(h, CURLUPART_SCHEME, &schemePart, 0) == CURLUE_OK) {
                outHost = hostPart;
                outScheme = schemePart;
                ok = true;
            }
            if (hostPart) curl_free(hostPart);
            if (schemePart) curl_free(schemePart);
        }

        curl_url_cleanup(h);
        return ok;
    }

    bool Whitelist::isAllowed(const std::string& url) const {
        std::string host, scheme;
        if (!extractHost(url, host, scheme)) return false;

        // On impose HTTPS : pas de téléchargement d'image en clair.
        if (toLower(scheme) != "https") return false;

        std::lock_guard<std::mutex> lock(m_mutex);
        return m_hosts.find(toLower(host)) != m_hosts.end();
    }

} // namespace arma3il
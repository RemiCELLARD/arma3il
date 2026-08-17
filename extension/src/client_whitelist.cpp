#include "arma3il/client_whitelist.h"
#include "arma3il/logger.h"

#include <fstream>
#include <algorithm>
#include <array>

#if defined(_WIN32)
#include <windows.h>
#endif

namespace arma3il {

    namespace {
        /* Default client list of Domain Name Whitelist. can be found and edit in txt file */
        constexpr std::array<const char*, 6> kDefaultHosts = {
            "i.goopics.net",
            "i.imgur.com",
            "i.ibb.co",
            "i.postimg.cc",
            "zupimages.net",
            "cdn.discordapp.com"
        };

#if defined(_WIN32)      
        /* 
        * Free function used solely as an address "anchor" to
        * identify the current module (DLL) via GetModuleHandleExA. A
        * non-static member function cannot be cast to a standard
        * function pointer using `reinterpret_cast` (as it implicitly encodes `this`) — a
        * free function can.
        */
        void AddressAnchor() {}
#endif

    } // namespace

    ClientWhitelist& ClientWhitelist::instance() {
        static ClientWhitelist wl;
        return wl;
    }

    std::string ClientWhitelist::toLower(std::string s) {
        std::transform(s.begin(), s.end(), s.begin(),
            [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
        return s;
    }

    std::string ClientWhitelist::getFilePath() const {
#if defined(_WIN32)
        HMODULE hModule = nullptr;
        GetModuleHandleExA(
            GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
            reinterpret_cast<LPCSTR>(&AddressAnchor),
            &hModule);

        char path[MAX_PATH] = { 0 };
        DWORD len = GetModuleFileNameA(hModule, path, MAX_PATH);
        if (len == 0 || len == MAX_PATH) {
            return "arma3il_whitelist.txt";
        }

        std::string fullPath(path, len);
        auto pos = fullPath.find_last_of("\\/");
        std::string dir = (pos == std::string::npos) ? "" : fullPath.substr(0, pos + 1);

        std::string result = dir + "arma3il_whitelist.txt";
        Logger::warn("ClientWhitelist: file localition solved = " + result);
        return result;
#else
        return "arma3il_whitelist.txt";
#endif
    }

    void ClientWhitelist::writeDefaultFile(const std::string& path) const {
        std::ofstream file(path, std::ios::trunc);
        if (!file) {
            Logger::error("ClientWhitelist: Can't create file " + path);
            return;
        }

        file << "# Arma3il client whitelist: one domain name per line.\n";
        file << "# Empty lines and lines beginning with # are ignored.\n";
        file << "# A domain is only permitted if it ALSO appears in the\n";
        file << "# whitelist added by the mission (whitelist_add): the two\n";
        file << "# lists are combined as an intersection.\n\n";

        for (const char* host : kDefaultHosts) {
            file << host << "\n";
        }
    }

    void ClientWhitelist::loadOrCreate() {
        const std::string path = getFilePath();

        std::ifstream file(path);
        if (!file) {
            writeDefaultFile(path);
            file.open(path);
            if (!file) {
                Logger::error("ClientWhitelist: File cannot be found even after creation : " + path);
                return;
            }
        }

        std::unordered_set<std::string> hosts;
        std::string line;
        while (std::getline(file, line)) {
            while (!line.empty() && (line.back() == '\r' || line.back() == '\n' || line.back() == ' ')) {
                line.pop_back();
            }
            size_t start = line.find_first_not_of(' ');
            if (start == std::string::npos) continue;
            line = line.substr(start);

            if (line.empty() || line.front() == '#') continue;

            hosts.insert(toLower(line));
        }

        std::lock_guard<std::mutex> lock(m_mutex);
        m_hosts = std::move(hosts);
        Logger::warn("ClientWhitelist: " + std::to_string(m_hosts.size()) + " field(s) responsible for since " + path);
    }

    bool ClientWhitelist::contains(const std::string& host) const {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_hosts.find(toLower(host)) != m_hosts.end();
    }

} // namespace arma3il
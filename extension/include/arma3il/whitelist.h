#pragma once
#include <string>
#include <mutex>
#include <unordered_set>
#include <vector>

namespace arma3il {

    /**
     * @class Whitelist
     * @brief Manages a mission-controlled whitelist of hosts authorized for downloads.
     *
     * This whitelist is populated exclusively via SQF calls to "whitelist_add"
     * during mission startup (client-side) — no local configuration file is
     * read for this particular whitelist. It represents what the mission
     * author considers safe, as opposed to ClientWhitelist, which represents
     * what the person running the game client considers safe. A download is
     * only permitted if the target host passes both checks.
     *
     * Validation enforces two criteria:
     *   - the host extracted from the URL must be present in this whitelist;
     *   - the URL scheme must be strictly `https`.
     *
     * All operations are thread-safe via an internal mutex.
     */
    class Whitelist {
    public:
        /**
         * @brief Default constructor.
         *
         * Initializes an empty whitelist.
         */
        Whitelist() = default;

        /**
         * @brief Adds a single host to the whitelist.
         *
         * The host is lowercased before insertion to ensure case-insensitive comparisons.
         *
         * @param host Hostname to add (e.g. "example.com").
         */
        void addHost(const std::string& host);

        /**
         * @brief Adds multiple hosts to the whitelist.
         *
         * Each host is lowercased before insertion.
         *
         * @param hosts List of hosts to add.
         */
        void addHosts(const std::vector<std::string>& hosts);

        /**
         * @brief Removes a host from the whitelist.
         *
         * @param host Hostname to remove.
         */
        void removeHost(const std::string& host);

        /**
         * @brief Clears the whitelist entirely.
         */
        void clear();

        /**
         * @brief Checks whether a URL is allowed.
         *
         * Extracts the host and scheme from the URL via libcurl, enforces the
         * `https` scheme, then checks whether the host is present in the
         * whitelist.
         *
         * @param url Full URL to analyze.
         * @return true if the URL is valid and its host is whitelisted, false otherwise.
         */
        bool isAllowed(const std::string& url) const;

        /**
         * @brief Returns the list of currently whitelisted hosts.
         *
         * @return A vector containing every host currently in the whitelist.
         */
        std::vector<std::string> listHosts() const;

        /**
         * @brief Extracts the host and scheme from a URL.
         *
         * Uses the `curl_url_*` API for robust URL parsing. Exposed as a public
         * static method so callers (e.g. main.cpp) can reuse the same
         * extraction logic when checking a host against ClientWhitelist.
         *
         * @param url       URL to analyze.
         * @param outHost   Output parameter receiving the extracted host.
         * @param outScheme Output parameter receiving the extracted scheme.
         * @return true if extraction succeeded, false otherwise.
         */
        static bool extractHost(const std::string& url, std::string& outHost, std::string& outScheme);

    private:
        /**
         * @brief Converts a string to lowercase.
         *
         * Used to normalize hosts before insertion or comparison.
         *
         * @param s String to convert.
         * @return The lowercase copy of the input string.
         */
        static std::string toLower(std::string s);

        mutable std::mutex m_mutex;              ///< Guards access to the host set.
        std::unordered_set<std::string> m_hosts; ///< Set of allowed hosts (lowercase).
    };

} // namespace arma3il
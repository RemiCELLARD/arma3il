#pragma once
#include <string>
#include <unordered_set>
#include <mutex>

namespace arma3il {

    /**
     * @class ClientWhitelist
     * @brief Client-side domain whitelist, stored in a plain text file next to the DLL.
     *
     * Unlike Whitelist (which is populated by the mission via whitelist_add),
     * this whitelist is controlled entirely by the person running the game
     * client. It is stored as one domain per line in "arma3il_whitelist.txt",
     * located next to the extension DLL. If the file does not exist, it is
     * created automatically with a small default list of hosts.
     *
     * A download is only allowed if the target host is present in BOTH this
     * client whitelist AND the mission whitelist (logical AND / intersection),
     * so neither party can unilaterally authorize a domain the other has not
     * approved.
     *
     * All operations are thread-safe.
     */
    class ClientWhitelist {
    public:
        /**
         * @brief Returns the process-wide singleton instance.
         *
         * @return Reference to the single ClientWhitelist instance.
         */
        static ClientWhitelist& instance();

        /**
         * @brief Loads the whitelist file, creating it with default content if absent.
         *
         * Idempotent: safe to call multiple times (e.g. to hot-reload the file
         * after a person has manually edited it) — each call fully replaces the
         * in-memory set of hosts with what is currently on disk.
         */
        void loadOrCreate();

        /**
         * @brief Checks whether a given host is present in the client whitelist.
         *
         * Comparison is case-insensitive.
         *
         * @param host Hostname to check (e.g. "cdn.example.com").
         * @return true if the host is present in the whitelist file, false otherwise.
         */
        bool contains(const std::string& host) const;

    private:
        /**
         * @brief Resolves the absolute path of "arma3il_whitelist.txt" next to this DLL.
         *
         * @return Absolute path to the whitelist file.
         */
        std::string getFilePath() const;

        /**
         * @brief Writes the default whitelist file with a header comment and default hosts.
         *
         * @param path Absolute path where the file should be created.
         */
        void writeDefaultFile(const std::string& path) const;

        /**
         * @brief Converts a string to lowercase.
         *
         * @param s String to convert.
         * @return The lowercase copy of the input string.
         */
        static std::string toLower(std::string s);

        mutable std::mutex m_mutex;                 ///< Guards access to m_hosts.
        std::unordered_set<std::string> m_hosts;    ///< Set of allowed hosts (lowercase), loaded from disk.
    };

} // namespace arma3il
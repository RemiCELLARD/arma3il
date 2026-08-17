#pragma once
#include <string>
#include <vector>

namespace arma3il {

    /**
     * @class SqfArrayBuilder
     * @brief Fluent builder producing a valid SQF array literal string.
     *
     * The RV Engine can only return a plain string from RVExtensionArgs, but
     * that string can itself be valid SQF array syntax (e.g. `["done","abc",512]`),
     * which SQF can decode with `parseSimpleArray` into a real array. This
     * avoids fragile manual splitting (splitString ":") on the SQF side, which
     * breaks on any value that itself contains the delimiter (e.g. Windows
     * paths containing ":").
     */
    class SqfArrayBuilder {
    public:       
        /**
         * @brief Appends a string element (quoted, with internal quotes escaped).
         *
         * @param value Value to add
         */
        SqfArrayBuilder& addString(const std::string& value);

        /**
         * @brief Appends a numeric element (unquoted, parsed by SQF as a number).
         *
         * @param value Value to add
         */
        SqfArrayBuilder& addNumber(long long value);

        /**
         * @brief Builds the final SQF array literal, e.g. ["done","abc123",512,512].
         *
         */
        std::string build() const;

    private:
        static std::string escapeString(const std::string& value);
        std::vector<std::string> m_parts;
    };

} // namespace arma3il
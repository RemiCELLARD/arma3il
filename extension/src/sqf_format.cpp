#include "arma3il/sqf_format.h"
#include <sstream>

namespace arma3il {

    std::string SqfArrayBuilder::escapeString(const std::string& value) {
        // SQF string literals escape an internal double-quote by doubling it
        // ("" ) — the exact same rule the engine itself uses when serializing
        // callExtension arguments, so this is consistent with unquoteSqfArg
        // on the receiving end.
        std::string result;
        result.reserve(value.size() + 2);
        for (char c : value) {
            if (c == '"') result += "\"\"";
            else result += c;
        }
        return result;
    }

    SqfArrayBuilder& SqfArrayBuilder::addString(const std::string& value) {
        m_parts.push_back("\"" + escapeString(value) + "\"");
        return *this;
    }

    SqfArrayBuilder& SqfArrayBuilder::addNumber(long long value) {
        m_parts.push_back(std::to_string(value));
        return *this;
    }

    std::string SqfArrayBuilder::build() const {
        std::ostringstream oss;
        oss << "[";
        for (size_t i = 0; i < m_parts.size(); ++i) {
            if (i > 0) oss << ",";
            oss << m_parts[i];
        }
        oss << "]";
        return oss.str();
    }

} // namespace arma3il
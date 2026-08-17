#include "arma3il/hash_util.h"
#include <cstdint>
#include <sstream>
#include <iomanip>

namespace arma3il {

    namespace {

        uint64_t fnv1a64(const std::string& input) {
            constexpr uint64_t kOffsetBasis = 14695981039346656037ULL;
            constexpr uint64_t kPrime = 1099511628211ULL;

            uint64_t hash = kOffsetBasis;
            for (unsigned char c : input) {
                hash ^= static_cast<uint64_t>(c);
                hash *= kPrime;
            }
            return hash;
        }

    } // namespace

    std::string HashUtil::hashToHex(const std::string& input) {
        const uint64_t hash = fnv1a64(input);

        std::ostringstream oss;
        oss << std::hex << std::setfill('0') << std::setw(16) << hash;
        return oss.str();
    }

} // namespace arma3il
#include "arma3il/whitelist.h"
#include <cstdio>

extern int g_failures;
#define CHECK(cond) do { \
    if (!(cond)) { \
        std::printf("FAIL: %s (%s:%d)\n", #cond, __FILE__, __LINE__); \
        ++g_failures; \
    } \
} while (0)

void run_whitelist_tests() {
    arma3il::Whitelist wl;

    CHECK(!wl.isAllowed("https://cdn.example.com/a.jpg")); // empty at the start

    wl.addHost("cdn.example.com");
    CHECK(wl.isAllowed("https://cdn.example.com/a.jpg"));
    CHECK(!wl.isAllowed("http://cdn.example.com/a.jpg")); // HTTP request rejected
    CHECK(!wl.isAllowed("https://evil.com/a.jpg")); // host not listed
    CHECK(!wl.isAllowed("https://cdn.example.com.evil.com/a.jpg")); // false positive for a substring

    wl.addHosts({ "one.com", "two.com" });
    CHECK(wl.isAllowed("https://one.com/x.jpg"));
    CHECK(wl.isAllowed("https://two.com/x.jpg"));

    wl.removeHost("one.com");
    CHECK(!wl.isAllowed("https://one.com/x.jpg"));
    CHECK(wl.isAllowed("https://two.com/x.jpg")); // the others remain unchanged

    wl.clear();
    CHECK(!wl.isAllowed("https://cdn.example.com/a.jpg"));
    CHECK(!wl.isAllowed("https://two.com/x.jpg"));
}
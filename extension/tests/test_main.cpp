#include <cstdio>

int g_failures = 0;

void run_url_validator_tests();
void run_whitelist_tests();

int main() {
    run_url_validator_tests();
    run_whitelist_tests();

    if (g_failures == 0) {
        std::printf("Good to go !\n");
        return 0;
    }
    std::printf("%d failed test(s).\n", g_failures);
    return 1;
}
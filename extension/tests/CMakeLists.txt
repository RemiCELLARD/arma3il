add_executable(arma3il_tests
    test_main.cpp
    test_url_validator.cpp
    test_whitelist.cpp
)
target_link_libraries(arma3il_tests PRIVATE arma3il_core)

add_test(NAME arma3il_tests COMMAND arma3il_tests)
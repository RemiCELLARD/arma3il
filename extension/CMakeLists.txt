cmake_minimum_required(VERSION 3.25)

if (POLICY CMP0091)
    cmake_policy(SET CMP0091 NEW)
endif()

project(arma3il LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(ARMA3IL_BUILD_TESTS "Build unit tests for the portable core" OFF)

# Ensures that Debug and Release are always available for a
# multi-configuration build system (Visual Studio) — has no effect for a
# single-configuration build system (Ninja), where CMAKE_BUILD_TYPE takes over.
if (CMAKE_GENERATOR MATCHES "Visual Studio")
    set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "" FORCE)
endif()

# Must be defined BEFORE the targets are created (add_library),
if (VCPKG_TARGET_TRIPLET MATCHES "static")
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()

find_package(CURL REQUIRED)

include(FetchContent)
FetchContent_Declare(
    stb
    GIT_REPOSITORY https://github.com/nothings/stb.git
    GIT_TAG master
)
FetchContent_MakeAvailable(stb)

add_library(arma3il_core OBJECT
    src/url_validator.cpp
    src/whitelist.cpp
    src/texture_cache.cpp
    src/image_decoder.cpp
    src/logger.cpp
    src/client_whitelist.cpp
    src/hash_util.cpp
    src/sqf_format.cpp
)
target_include_directories(arma3il_core PUBLIC include ${stb_SOURCE_DIR})
target_link_libraries(arma3il_core PUBLIC CURL::libcurl)

add_library(arma3il SHARED
    src/main.cpp
    src/downloader.cpp
    src/callback_manager.cpp
    $<TARGET_OBJECTS:arma3il_core>
)
target_include_directories(arma3il PRIVATE include)
target_link_libraries(arma3il PRIVATE CURL::libcurl)

if (WIN32)
    if (CMAKE_SIZEOF_VOID_P EQUAL 8)
        set_target_properties(arma3il PROPERTIES OUTPUT_NAME "arma3il_x64")
    else()
        set_target_properties(arma3il PROPERTIES OUTPUT_NAME "arma3il")
    endif()
endif()

if (ARMA3IL_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
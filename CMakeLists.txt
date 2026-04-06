cmake_minimum_required(VERSION 3.23)

# cmake -B build -DCMAKE_INSTALL_PREFIX=/usr/local
# cmake --build build
# cmake --install build

# gh-update-checker https://api.github.com/repos/nlohmann/json/releases/latest 3.11.2

project(gh_update_checker
        VERSION 1.0.8
        DESCRIPTION "CLI tool to check for GitHub release updates"
        HOMEPAGE_URL "https://github.com/Zheng-Bote/gh-update-checker"
        LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(FetchContent)
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

# ---------------------------------------------------------
# Dependencies via FetchContent (NOT exported!)
# ---------------------------------------------------------

FetchContent_Declare(
    nlohmann_json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.11.3
)
FetchContent_MakeAvailable(nlohmann_json)

# ---------------------------------------------------------
# System Dependencies
# ---------------------------------------------------------

find_package(CURL REQUIRED)


# ---------------------------------------------------------
# Header-only library
# ---------------------------------------------------------

add_library(gh_update_checker INTERFACE)

target_include_directories(gh_update_checker INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

target_sources(gh_update_checker INTERFACE
    FILE_SET HEADERS
    BASE_DIRS include
    FILES include/check_gh-update.hpp
)

# IMPORTANT: do NOT link external targets here
target_link_libraries(gh_update_checker INTERFACE
    CURL::libcurl
)

# ---------------------------------------------------------
# CLI tool
# ---------------------------------------------------------

add_executable(gh-update-checker cli/gh-update-checker.cpp)

target_link_libraries(gh-update-checker
    gh_update_checker
    nlohmann_json::nlohmann_json
    CURL::libcurl
)

install(
    TARGETS gh-update-checker
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# ---------------------------------------------------------
# Installation of library + config package
# ---------------------------------------------------------

install(
    TARGETS gh_update_checker
    EXPORT gh_update_checkerTargets
    FILE_SET HEADERS DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(
    EXPORT gh_update_checkerTargets
    FILE gh_update_checkerTargets.cmake
    NAMESPACE gh_update_checker::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/gh_update_checker
)

configure_package_config_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/gh_update_checkerConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/gh_update_checkerConfig.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/gh_update_checker
)

install(
    FILES ${CMAKE_CURRENT_BINARY_DIR}/gh_update_checkerConfig.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/gh_update_checker
)

# ---------------------------------------------------------
# Tests
# ---------------------------------------------------------

enable_testing()

add_executable(test_basic tests/test_basic.cpp)

target_link_libraries(test_basic
    gh_update_checker
    nlohmann_json::nlohmann_json
    CURL::libcurl
)

add_test(NAME basic_update_check COMMAND test_basic)

file(GENERATE OUTPUT "${CMAKE_BINARY_DIR}/libs.txt" 
     CONTENT "$<TARGET_PROPERTY:gh-update-checker,LINK_LIBRARIES>")
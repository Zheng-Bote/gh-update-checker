/*!
 * @file gh-update-checker.cpp
 * @brief Example CLI for GitHub release update checker library
 *
 * This file is intentionally placed under examples/ and is not part of the
 * default build. It demonstrates how to call the library from a command-line
 * executable.
 */

#include <iostream>
#include <string>

#include <check_gh-update.hpp>

int main(int argc, char** argv) {
    if (argc < 3) {
        std::cerr << "Usage: gh-update-checker <repo-api-url> <local-version>\n";
        std::cerr << "Example:\n";
        std::cerr << "  gh-update-checker "
                     "https://api.github.com/repos/nlohmann/json/releases/latest 3.11.2\n";
        return 1;
    }

    std::string repo = argv[1];
    std::string local = argv[2];

    try {
        auto info = ghupdate::check_github_update(repo, local);
        std::cout << "Local version:  " << local << "\n";
        std::cout << "Remote version: " << info.latestVersion << "\n";
        std::cout << "Update:         " << (info.hasUpdate ? "YES" : "NO") << "\n";
        return info.hasUpdate ? 2 : 0;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 3;
    }
}

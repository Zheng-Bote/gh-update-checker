/*!
 * @file gh-update-checker.cpp
 * @brief Example CLI for GitHub release update checker library (C++23 optimized)
 */

#include <print>
#include <string_view>
#include <string>
#include <cstdlib>

#include <check_gh-update.hpp>

int main(int argc, char** argv) {
    if (argc < 3) {
        std::println("Usage: gh-update-checker <repo-url|api-url> <local-version>");
        std::println("Example:");
        std::println("  gh-update-checker [github.com](https://github.com/zheng-bote/gh-update-checker) v1.0.8\n");
        std::println("Note: You may pass a normal GitHub URL or a GitHub API URL.");
        std::exit(0);
    }

    std::string_view repoUrl{argv[1]};
    std::string_view localVersion{argv[2]};

    try {
        const auto info = ghupdate::check_github_update(repoUrl, localVersion);

        std::println("Local version:   {}", localVersion);
        std::println("Remote version:  {}", info.latestVersion);
        std::println("Update needed:   {}", info.hasUpdate ? "YES" : "NO");

        // Exit codes:
        //   0 → no update
        //   2 → update available
        std::exit(info.hasUpdate ? 2 : 0);

    } catch (const std::exception &e) {
        std::println(stderr, "Error: {}", e.what());
        std::exit(3);
    }
}

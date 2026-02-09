/*!
 * @file gh-update-checker.cpp
 * @brief Command-line interface for GitHub release update checker
 *
 * This program checks if a newer version of a GitHub repository is available
 * by comparing the latest release tag with a provided local version.
 * It uses semantic versioning (SemVer) for version comparison.
 *
 * Usage:
 *  gh-update-checker <repo-url-or-api-url> <local-version>
 *
 * Arguments:
 *  - repo-url-or-api-url: GitHub repository URL or GitHub API release URL
 *    Examples:
 *      - https://github.com/nlohmann/json
 *      - https://api.github.com/repos/nlohmann/json/releases/latest
 *  - local-version: Local version string in SemVer format (e.g., "3.11.2", "v1.0")
 *
 * Exit Codes:
 *  - 0: Success - no update available (local version is current)
 *  - 1: Usage error - invalid number of arguments
 *  - 2: Success - update available (newer version found)
 *  - 3: Runtime error - network, API, or parsing error
 *
 * Output:
 *  - Prints comparison results to stdout
 *  - Prints error messages to stderr
 *
 * @example
 * ```bash
 * $ gh-update-checker https://github.com/nlohmann/json 3.11.2
 * Local version:  3.11.2
 * Remote version: 3.11.3
 * Update:         YES
 * $ echo $?
 * 2
 *
 * $ gh-update-checker https://github.com/nlohmann/json 3.11.3
 * Local version:  3.11.3
 * Remote version: 3.11.3
 * Update:         NO
 * $ echo $?
 * 0
 * ```
 *
 * @author Your Team
 * @version 1.0.0
 */

#include <iostream>
#include <string>
#include <check_gh-update.hpp>

/*!
 * @brief Main entry point for the GitHub update checker CLI
 *
 * Parses command-line arguments, performs the update check,
 * and reports the results.
 *
 * @param argc Number of command-line arguments
 * @param argv Command-line arguments array
 *
 * @return Exit code:
 *         - 0: No update available
 *         - 1: Invalid arguments (usage error)
 *         - 2: Update available
 *         - 3: Runtime error (network, API, parsing)
 *
 * @note Catches std::exception and reports error to stderr
 */
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
        // exit code 2 = update available
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 3;
    }
}

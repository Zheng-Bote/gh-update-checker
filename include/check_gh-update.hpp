/**
 * SPDX-FileComment: Main entry point
 * SPDX-FileType: SOURCE
 * SPDX-FileContributor: ZHENG Robert
 * SPDX-FileCopyrightText: 2026 ZHENG Robert
 * SPDX-License-Identifier: LGPL-3.0
 *
 * @file check_gh-update.hpp
 * @brief GitHub release checker utility with semantic versioning support
 *
 * This header provides functionality to check for the latest release of a GitHub
 * repository and compare it with a local version using semantic versioning (SemVer).
 * 
 * Features:
 *  - HTTP GET requests via cpp-httplib
 *  - JSON parsing with nlohmann/json
 *  - Semantic versioning (SemVer) parsing and comparison
 *  - Automatic GitHub URL to API URL conversion
 *  - Synchronous and asynchronous version checking
 *  - Exception-based error handling for invalid inputs
 *
 * @version 1.0.9
 * @date 2026-02-25
 * @author ZHENG Robert (robert@hase-zheng.net)
 * @copyright Copyright (c) 2026 ZHENG Robert
 *
 * @example
 * ```cpp
 * auto result = ghupdate::check_github_update(
 *     "https://github.com/nlohmann/json",
 *     "3.11.2"
 * );
 * if (result.hasUpdate) {
 *     std::println("Update available: {}", result.latestVersion);
 * }
 * ```
 */

#pragma once
#include <string>
#include <string_view>
#include <regex>
#include <stdexcept>
#include <future>
#include <httplib.h>
#include <nlohmann/json.hpp>

namespace ghupdate {

// ---------------------------------------------------------
// SemVer
// ---------------------------------------------------------

/*!
 * @struct SemVer
 * @brief Semantic versioning structure (major.minor.patch)
 *
 * Represents a semantic version following the pattern MAJOR.MINOR.PATCH.
 * Supports version strings with optional 'v' prefix (e.g., "v1.2.3" or "1.2.3").
 * 
 * @note Implements three-way comparison operator (operator<=>)
 *       for easy version comparison (>, <, ==, etc.)
 */
struct SemVer {
    int major = 0;  ///< Major version component
    int minor = 0;  ///< Minor version component
    int patch = 0;  ///< Patch version component

    /*!
     * @brief Parses a semantic version string
     *
     * Extracts major, minor, and patch components from a string.
     * Accepts formats: "1.2.3", "v1.2.3", "1.2"
     *
     * @param v Version string to parse
     * @return Parsed SemVer structure
     * @throws std::runtime_error if version format is invalid
     *
     * @example
     * ```cpp
     * auto ver = SemVer::parse("v3.11.2");  // major=3, minor=11, patch=2
     * auto ver = SemVer::parse("1.0");      // major=1, minor=0, patch=0
     * ```
     */
    static SemVer parse(std::string_view v) {
        std::regex re(R"(v?(\d+)\.(\d+)(?:\.(\d+))?)");
        std::cmatch m;

        if (!std::regex_search(v.begin(), v.end(), m, re))
            throw std::runtime_error("Invalid SemVer: " + std::string(v));

        SemVer sv;
        sv.major = std::stoi(m[1]);
        sv.minor = std::stoi(m[2]);
        sv.patch = m[3].matched ? std::stoi(m[3]) : 0;
        return sv;
    }

    /*!
     * @brief Three-way comparison operator for semantic version comparison
     * @return Comparison result (==, <, >)
     * @note Enables use with <, >, <=, >= operators
     */
    auto operator<=>(const SemVer&) const = default;
};

// ---------------------------------------------------------
// HTTP GET via cpp-httplib
// ---------------------------------------------------------

/*!
 * @brief Performs an HTTP GET request
 *
 * Sends an HTTP GET request to the specified URL using cpp-httplib
 * and returns the response body as a string.
 *
 * @param url The URL to request (std::string_view)
 * @return Response body as std::string
 * @throws std::runtime_error on invalid URL, transport, or HTTP status errors
 *
 * @note Sets User-Agent header to "C++23-gh-update-checker"
 */
inline std::string http_get(std::string_view url) {
    static const std::regex url_re(R"(https://([^/]+)(/.*)?)");
    std::cmatch match;
    if (!std::regex_match(url.begin(), url.end(), match, url_re)) {
        throw std::runtime_error("Only https:// URLs are supported: " + std::string(url));
    }

    const std::string host = match[1].str();
    const std::string path = match[2].matched ? match[2].str() : "/";

    #ifdef CPPHTTPLIB_OPENSSL_SUPPORT
    httplib::SSLClient client(host);
    client.set_follow_location(true);
    client.set_connection_timeout(10, 0);
    client.set_read_timeout(30, 0);
    client.set_write_timeout(30, 0);

    httplib::Headers headers{
        {"User-Agent", "C++23-gh-update-checker"},
        {"Accept", "application/vnd.github+json"}
    };

    const auto response = client.Get(path, headers);
    if (!response) {
        throw std::runtime_error("HTTP request failed");
    }

    if (response->status < 200 || response->status >= 300) {
        throw std::runtime_error("GitHub API returned HTTP status " + std::to_string(response->status));
    }

    return response->body;
    #else
    throw std::runtime_error("cpp-httplib was built without OpenSSL support (HTTPS required)");
    #endif
}

// ---------------------------------------------------------
// Automatic GitHub URL to API URL conversion
// ---------------------------------------------------------

/*!
 * @brief Converts GitHub repository URLs to GitHub API URLs
 *
 * Automatically detects and converts standard GitHub repository URLs
 * to their corresponding GitHub API endpoints. If the input is already
 * an API URL, it is returned unchanged.
 *
 * Supported formats:
 *  - Standard URLs: https://github.com/owner/repo
 *  - Standard URLs with .git: https://github.com/owner/repo.git
 *  - API URLs: https://api.github.com/repos/owner/repo/releases/latest
 *
 * @param url GitHub repository URL or API URL
 * @return GitHub API URL for fetching releases
 * @throws std::runtime_error if URL format is invalid
 *
 * @example
 * ```cpp
 * auto api_url = to_github_api_url("https://github.com/nlohmann/json");
 * // Returns: "https://api.github.com/repos/nlohmann/json/releases/latest"
 * ```
 */
inline std::string to_github_api_url(std::string_view url) {
    if (url.find("api.github.com") != std::string::npos)
        return std::string(url);

    std::regex re(R"(https://github\.com/([^/]+)/([^/]+))");
    std::cmatch m;

    if (!std::regex_search(url.begin(), url.end(), m, re))
        throw std::runtime_error("Invalid GitHub URL: " + std::string(url));

    std::string owner = m[1];
    std::string repo  = m[2];

    if (repo.ends_with(".git"))
        repo = repo.substr(0, repo.size() - 4);

    return "https://api.github.com/repos/" + owner + "/" + repo + "/releases/latest";
}

// ---------------------------------------------------------
// UpdateInfo
// ---------------------------------------------------------

/*!
 * @struct UpdateInfo
 * @brief Result of a GitHub update check
 *
 * Contains the outcome of comparing a local version with the latest
 * remote release on GitHub.
 */
struct UpdateInfo {
    bool hasUpdate;              ///< true if remote version > local version
    std::string latestVersion;   ///< Latest release tag/version from GitHub
};

// ---------------------------------------------------------
// Synchronous version checking function
// ---------------------------------------------------------

/*!
 * @brief Checks for updates on a GitHub repository (synchronous)
 *
 * Fetches the latest release information from GitHub, parses the version,
 * and compares it with the provided local version using semantic versioning.
 *
 * Workflow:
 *  1. Converts the input URL to a GitHub API endpoint if needed
 *  2. Performs HTTP GET request to retrieve release information
 *  3. Parses JSON response to extract the tag_name field
 *  4. Compares versions using SemVer comparison
 *
 * @param repoUrl GitHub repository URL or API URL
 *        Examples: 
 *        - "https://github.com/user/repo" 
 *        - "https://api.github.com/repos/user/repo/releases/latest"
 * @param localVersion Local version string (will be parsed as SemVer)
 *
 * @return UpdateInfo structure containing:
 *         - hasUpdate: true if remote > local version
 *         - latestVersion: Latest version string from GitHub
 *
 * @throws std::runtime_error on:
 *         - Invalid GitHub URL format
 *         - HTTP request failure
 *         - Invalid JSON response from GitHub API
 *         - Invalid version string format
 *
 * @example
 * ```cpp
 * try {
 *     auto result = ghupdate::check_github_update(
 *         "https://github.com/nlohmann/json",
 *         "3.11.2"
 *     );
 *     if (result.hasUpdate) {
 *         std::println("Update available: {}", result.latestVersion);
 *     }
 * } catch (const std::exception& e) {
 *     std::println("Error: {}", e.what());
 * }
 * ```
 */
inline UpdateInfo check_github_update(
    std::string_view repoUrl,
    std::string_view localVersion
) {
    std::string apiUrl = to_github_api_url(repoUrl);

    std::string jsonText = http_get(apiUrl);
    auto json = nlohmann::json::parse(jsonText);

    if (!json.contains("tag_name") || !json["tag_name"].is_string()) {
        if (json.contains("message") && json["message"].is_string()) {
            throw std::runtime_error("GitHub API error: " + json["message"].get<std::string>());
        }
        throw std::runtime_error("GitHub API returned no valid tag_name");
    }

    std::string latest = json["tag_name"].get<std::string>();

    SemVer local = SemVer::parse(localVersion);
    SemVer remote = SemVer::parse(latest);

    return { remote > local, latest };
}

// ---------------------------------------------------------
// Asynchronous version checking function
// ---------------------------------------------------------

/*!
 * @brief Checks for updates on a GitHub repository (asynchronous)
 *
 * Non-blocking wrapper around check_github_update that executes
 * the update check in a separate thread using std::async.
 *
 * @param repoUrl GitHub repository URL or API URL (ownership transferred)
 * @param localVersion Local version string (ownership transferred)
 *
 * @return std::future<UpdateInfo> that resolves to the update check result
 *
 * @throws std::runtime_error (via future) on same conditions as check_github_update
 *
 * @note Call future.get() to retrieve the result (blocks until ready).
 *       Exceptions thrown during execution will be re-thrown when calling 
 *       future.get().
 *
 * @example
 * ```cpp
 * auto future = ghupdate::check_github_update_async(
 *     "https://github.com/nlohmann/json",
 *     "3.11.2"
 * );
 * // ... do other work ...
 * auto result = future.get();  // Blocks until result is ready
 * if (result.hasUpdate) {
 *     std::println("Update: {}", result.latestVersion);
 * }
 * ```
 */
inline std::future<UpdateInfo> check_github_update_async(
    std::string repoUrl,
    std::string localVersion
) {
    return std::async(std::launch::async, [repoUrl, localVersion] {
        return check_github_update(repoUrl, localVersion);
    });
}

} // namespace ghupdate

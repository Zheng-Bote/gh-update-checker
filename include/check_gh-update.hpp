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
 * @version 1.1.1
 * @date 2026-05-16
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

struct SemVer {
    int major = 0;
    int minor = 0;
    int patch = 0;

    static SemVer parse(std::string_view v) {
        static const std::regex re(R"(v?(\d+)\.(\d+)(?:\.(\d+))?)");

        std::string s(v);
        std::match_results<std::string::const_iterator> m;

        if (!std::regex_match(s, m, re))
            throw std::runtime_error("Invalid SemVer format: " + std::string(v));

        SemVer sv;
        sv.major = std::stoi(m[1]);
        sv.minor = std::stoi(m[2]);
        sv.patch = m[3].matched ? std::stoi(m[3]) : 0;
        return sv;
    }

    auto operator<=>(const SemVer&) const = default;
};


// ---------------------------------------------------------
// HTTP GET via cpp-httplib
// ---------------------------------------------------------

inline std::string http_get(std::string_view url) {
    static const std::regex url_re(R"(https://([^/]+)(/.*)?)");

    std::string s(url);
    std::match_results<std::string::const_iterator> m;

    if (!std::regex_match(s, m, url_re)) {
        throw std::runtime_error("Only https:// URLs are supported: " + std::string(url));
    }

    const std::string host = m[1].str();
    const std::string path = m[2].matched ? m[2].str() : "/";

#ifndef CPPHTTPLIB_OPENSSL_SUPPORT
    throw std::runtime_error("cpp-httplib built without OpenSSL support (HTTPS required)");
#else
    httplib::SSLClient client(host);
    client.set_follow_location(true);
    client.set_connection_timeout(10, 0);
    client.set_read_timeout(30, 0);
    client.set_write_timeout(30, 0);

    httplib::Headers headers{
        {"User-Agent", "C++23-gh-update-checker"},
        {"Accept", "application/vnd.github+json"}
    };

    auto resp = client.Get(path, headers);
    if (!resp) {
        // Keine get_last_error()-Unterstützung in älteren httplib Releases
        throw std::runtime_error("HTTP request failed (no response)");
    }

    if (resp->status == 403 &&
        resp->body.find("rate limit") != std::string::npos)
    {
        throw std::runtime_error("GitHub API rate limit exceeded");
    }

    if (resp->status < 200 || resp->status >= 300) {
        throw std::runtime_error(
            "GitHub API HTTP status " + std::to_string(resp->status) +
            " - " + resp->body
        );
    }

    return resp->body;
#endif
}


// ---------------------------------------------------------
// GitHub URL → API URL
// ---------------------------------------------------------

inline std::string to_github_api_url(std::string_view url) {
    // API URL passt direkt
    if (url.find("api.github.com") != std::string::npos)
        return std::string(url);

    static const std::regex re(
        R"(https://github\.com/([^/]+)/([^/?#]+))"
    );

    std::string s(url);
    std::match_results<std::string::const_iterator> m;

    if (!std::regex_search(s, m, re)) {
        throw std::runtime_error("Invalid GitHub repository URL: " + std::string(url));
    }

    std::string owner = m[1].str();
    std::string repo  = m[2].str();

    if (repo.ends_with(".git"))
        repo.erase(repo.size() - 4);

    return "https://api.github.com/repos/" + owner + "/" + repo + "/releases/latest";
}


// ---------------------------------------------------------
// UpdateInfo
// ---------------------------------------------------------

struct UpdateInfo {
    bool hasUpdate;
    std::string latestVersion;
};


// ---------------------------------------------------------
// Synchronous update check
// ---------------------------------------------------------

inline UpdateInfo check_github_update(
    std::string_view repoUrl,
    std::string_view localVersion)
{
    const std::string apiUrl = to_github_api_url(repoUrl);
    const std::string body   = http_get(apiUrl);

    nlohmann::json json;
    try {
        json = nlohmann::json::parse(body);
    } catch (...) {
        throw std::runtime_error("Invalid JSON from GitHub API");
    }

    if (!json.contains("tag_name") || !json["tag_name"].is_string()) {
        if (json.contains("message"))
            throw std::runtime_error("GitHub API error: " + json["message"].get<std::string>());

        throw std::runtime_error("GitHub API returned no release tag_name");
    }

    std::string latest = json["tag_name"].get<std::string>();

    if (latest.starts_with("v"))
        latest.erase(0, 1);

    const SemVer localVer  = SemVer::parse(localVersion);
    const SemVer remoteVer = SemVer::parse(latest);

    return { remoteVer > localVer, latest };
}


// ---------------------------------------------------------
// Asynchronous update check
// ---------------------------------------------------------

inline std::future<UpdateInfo> check_github_update_async(
    std::string repoUrl,
    std::string localVersion)
{
    return std::async(
        std::launch::async | std::launch::deferred,
        [repoUrl = std::move(repoUrl),
         localVersion = std::move(localVersion)] {
            return check_github_update(repoUrl, localVersion);
        }
    );
}

} // namespace ghupdate

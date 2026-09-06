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
 * This header provides functionality to check for the latest release of a
 GitHub
 * repository and compare it with a local version using semantic versioning
 (SemVer).
 *
 * Features:
 *  - HTTP GET requests via cpp-httplib
 *  - JSON parsing with nlohmann/json
 *  - Semantic versioning (SemVer) parsing and comparison
 *  - Automatic GitHub URL to API URL conversion
 *  - Synchronous and asynchronous version checking
 *  - Exception-based error handling for invalid inputs
 *
 * @version 2.0.0
 * @date 2026-09-06

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
#include <future>
#include <memory>
#include <mutex>
#include <optional>
#include <regex>
#include <stdexcept>
#include <string>
#include <string_view>

#include <curl/curl.h>
#include <nlohmann/json.hpp>

namespace ghupdate {

// ---------------------------------------------------------
// Proxy
// ---------------------------------------------------------

/**
 * @brief Proxy configuration
 *
 * Supports format: user:password@host:port
 */
struct Proxy {
  std::string host;
  int port = 0;
  std::string user;
  std::string password;

  /**
   * @brief Parse proxy string in format user:password@host:port
   * @param proxyStr The proxy string to parse
   * @return Proxy object
   * @throws std::runtime_error if format is invalid
   */
  static Proxy parse(std::string_view proxyStr) {
    static const std::regex re(R"(^([^:]+):(.*)@(.+):(\d+)$)");
    std::string s(proxyStr);
    std::smatch m;
    if (!std::regex_match(s, m, re)) {
      throw std::runtime_error(
          "Invalid proxy format. Expected user:password@host:port");
    }
    Proxy p;
    p.user = m[1].str();
    p.password = m[2].str();
    p.host = m[3].str();
    p.port = std::stoi(m[4].str());
    return p;
  }
};

// ---------------------------------------------------------
// SemVer
// ---------------------------------------------------------
// ... rest of file (I'll provide the whole file or at least the relevant parts)

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

  auto operator<=>(const SemVer &) const = default;
};

// ---------------------------------------------------------
// HTTP GET via libcurl
// ---------------------------------------------------------

inline size_t WriteCallback(void *contents, size_t size, size_t nmemb,
                            void *userp) {
  auto *str = static_cast<std::string *>(userp);
  size_t realsize = size * nmemb;
  str->append(static_cast<char *>(contents), realsize);
  return realsize;
}

inline std::string
http_get(std::string_view url,
         std::optional<std::string_view> proxy = std::nullopt) {
  static const std::regex url_re(R"(https://([^/]+)(/.*)?)");

  std::string s(url);
  if (!std::regex_match(s, url_re)) {
    throw std::runtime_error("Only https:// URLs are supported: " +
                             std::string(url));
  }

  // Thread-safe one-time initialization for libcurl
  static std::once_flag init_flag;
  std::call_once(init_flag, []() { curl_global_init(CURL_GLOBAL_DEFAULT); });

  // RAII wrapper for CURL handle
  std::unique_ptr<CURL, decltype(&curl_easy_cleanup)> curl(curl_easy_init(),
                                                           curl_easy_cleanup);
  if (!curl) {
    throw std::runtime_error("Failed to initialize libcurl");
  }

  char error_buffer[CURL_ERROR_SIZE] = "";
  std::string response_string;

  curl_easy_setopt(curl.get(), CURLOPT_ERRORBUFFER, error_buffer);
  curl_easy_setopt(curl.get(), CURLOPT_URL, s.c_str());
  curl_easy_setopt(curl.get(), CURLOPT_WRITEFUNCTION, WriteCallback);
  curl_easy_setopt(curl.get(), CURLOPT_WRITEDATA, &response_string);
  curl_easy_setopt(curl.get(), CURLOPT_FOLLOWLOCATION, 1L);
  curl_easy_setopt(curl.get(), CURLOPT_CONNECTTIMEOUT, 10L);
  curl_easy_setopt(curl.get(), CURLOPT_TIMEOUT, 30L);

  // RAII wrapper for curl_slist
  std::unique_ptr<curl_slist, decltype(&curl_slist_free_all)> headers(
      nullptr, curl_slist_free_all);
  headers.reset(curl_slist_append(headers.release(),
                                  "User-Agent: C++23-gh-update-checker"));
  headers.reset(curl_slist_append(headers.release(),
                                  "Accept: application/vnd.github+json"));
  curl_easy_setopt(curl.get(), CURLOPT_HTTPHEADER, headers.get());

  if (proxy) {
    auto p = Proxy::parse(*proxy);
    std::string proxy_url = p.host + ":" + std::to_string(p.port);
    curl_easy_setopt(curl.get(), CURLOPT_PROXY, proxy_url.c_str());
    if (!p.user.empty() || !p.password.empty()) {
      std::string auth = p.user + ":" + p.password;
      curl_easy_setopt(curl.get(), CURLOPT_PROXYUSERPWD, auth.c_str());
    }
  }

  CURLcode res = curl_easy_perform(curl.get());

  if (res != CURLE_OK) {
    std::string err_msg =
        error_buffer[0] != '\0' ? error_buffer : curl_easy_strerror(res);
    throw std::runtime_error("HTTP request failed: " + err_msg);
  }

  long http_code = 0;
  curl_easy_getinfo(curl.get(), CURLINFO_RESPONSE_CODE, &http_code);

  if (http_code == 403 &&
      response_string.find("rate limit") != std::string::npos) {
    throw std::runtime_error("GitHub API rate limit exceeded");
  }

  if (http_code < 200 || http_code >= 300) {
    throw std::runtime_error("GitHub API HTTP status " +
                             std::to_string(http_code) + " - " +
                             response_string);
  }

  return response_string;
}

// ---------------------------------------------------------
// GitHub URL → API URL
// ---------------------------------------------------------

inline std::string to_github_api_url(std::string_view url) {
  // API URL passt direkt
  if (url.find("api.github.com") != std::string::npos)
    return std::string(url);

  static const std::regex re(R"(https://github\.com/([^/]+)/([^/?#]+))");

  std::string s(url);
  std::match_results<std::string::const_iterator> m;

  if (!std::regex_search(s, m, re)) {
    throw std::runtime_error("Invalid GitHub repository URL: " +
                             std::string(url));
  }

  std::string owner = m[1].str();
  std::string repo = m[2].str();

  if (repo.ends_with(".git"))
    repo.erase(repo.size() - 4);

  return "https://api.github.com/repos/" + owner + "/" + repo +
         "/releases/latest";
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

inline UpdateInfo
check_github_update(std::string_view repoUrl, std::string_view localVersion,
                    std::optional<std::string_view> proxy = std::nullopt) {
  const std::string apiUrl = to_github_api_url(repoUrl);
  const std::string body = http_get(apiUrl, proxy);

  nlohmann::json json;
  try {
    json = nlohmann::json::parse(body);
  } catch (...) {
    throw std::runtime_error("Invalid JSON from GitHub API");
  }

  if (!json.contains("tag_name") || !json["tag_name"].is_string()) {
    if (json.contains("message"))
      throw std::runtime_error("GitHub API error: " +
                               json["message"].get<std::string>());

    throw std::runtime_error("GitHub API returned no release tag_name");
  }

  std::string latest = json["tag_name"].get<std::string>();

  if (latest.starts_with("v"))
    latest.erase(0, 1);

  const SemVer localVer = SemVer::parse(localVersion);
  const SemVer remoteVer = SemVer::parse(latest);

  return {remoteVer > localVer, latest};
}

// ---------------------------------------------------------
// Asynchronous update check
// ---------------------------------------------------------

inline std::future<UpdateInfo>
check_github_update_async(std::string repoUrl, std::string localVersion,
                          std::optional<std::string> proxy = std::nullopt) {
  return std::async(std::launch::async | std::launch::deferred,
                    [repoUrl = std::move(repoUrl),
                     localVersion = std::move(localVersion),
                     proxy = std::move(proxy)] {
                      return check_github_update(repoUrl, localVersion, proxy);
                    });
}

} // namespace ghupdate

/*!
 * @file test_basic.cpp
 * @brief Integration tests for gh-update-checker synchronous and asynchronous operations
 *
 * Tests cover:
 *  - Synchronous update checking with real GitHub API
 *  - Asynchronous update checking
 *  - SemVer version parsing and comparison
 *  - Error handling for invalid inputs
 *
 * @note Tests require network connectivity to GitHub API
 */

#include <check_gh-update.hpp>
#include <iostream>
#include <cassert>
#include <chrono>

// Test counter for simple reporting
int tests_passed = 0;
int tests_failed = 0;

/*!
 * @brief Print test result to stdout
 *
 * @param test_name Name of the test
 * @param passed Whether the test passed
 */
void print_result(const char* test_name, bool passed) {
    if (passed) {
        std::cout << "✓ PASS: " << test_name << "\n";
        tests_passed++;
    } else {
        std::cout << "✗ FAIL: " << test_name << "\n";
        tests_failed++;
    }
}

/*!
 * @brief Test 1: SemVer parsing with valid versions
 */
void test_semver_parsing() {
    try {
        auto v1 = ghupdate::SemVer::parse("1.2.3");
        auto v2 = ghupdate::SemVer::parse("v1.2.3");
        auto v3 = ghupdate::SemVer::parse("2.0");

        bool pass = (v1.major == 1 && v1.minor == 2 && v1.patch == 3) &&
                    (v2.major == 1 && v2.minor == 2 && v2.patch == 3) &&
                    (v3.major == 2 && v3.minor == 0 && v3.patch == 0);

        print_result("SemVer parsing", pass);
    } catch (const std::exception& e) {
        std::cerr << "  Exception: " << e.what() << "\n";
        print_result("SemVer parsing", false);
    }
}

/*!
 * @brief Test 2: SemVer comparison operators
 */
void test_semver_comparison() {
    try {
        auto v1 = ghupdate::SemVer::parse("1.0.0");
        auto v2 = ghupdate::SemVer::parse("1.1.0");
        auto v3 = ghupdate::SemVer::parse("1.0.0");

        bool pass = (v2 > v1) && (v1 < v2) && (v1 == v3);

        print_result("SemVer comparison", pass);
    } catch (const std::exception& e) {
        std::cerr << "  Exception: " << e.what() << "\n";
        print_result("SemVer comparison", false);
    }
}

/*!
 * @brief Test 3: Synchronous update check with standard GitHub URL
 *
 * Performs a real network call to GitHub API for nlohmann/json
 */
void test_sync_update_check_standard_url() {
    try {
        std::cout << "  Running synchronous check (standard GitHub URL)...\n";

        auto result = ghupdate::check_github_update(
            "https://github.com/nlohmann/json",
            "0.0.1"
        );

        bool pass = !result.latestVersion.empty();

        std::cout << "  Latest version found: " << result.latestVersion << "\n";
        std::cout << "  Has update: " << (result.hasUpdate ? "YES" : "NO") << "\n";

        print_result("Sync update check (standard URL)", pass);
    } catch (const std::exception& e) {
        std::cerr << "  Exception: " << e.what() << "\n";
        print_result("Sync update check (standard URL)", false);
    }
}

/*!
 * @brief Test 4: Synchronous update check with API GitHub URL
 *
 * Performs a real network call using GitHub API URL format
 */
void test_sync_update_check_api_url() {
    try {
        std::cout << "  Running synchronous check (GitHub API URL)...\n";

        auto result = ghupdate::check_github_update(
            "https://api.github.com/repos/nlohmann/json/releases/latest",
            "0.0.1"
        );

        bool pass = !result.latestVersion.empty();

        std::cout << "  Latest version found: " << result.latestVersion << "\n";
        std::cout << "  Has update: " << (result.hasUpdate ? "YES" : "NO") << "\n";

        print_result("Sync update check (API URL)", pass);
    } catch (const std::exception& e) {
        std::cerr << "  Exception: " << e.what() << "\n";
        print_result("Sync update check (API URL)", false);
    }
}

/*!
 * @brief Test 5: Asynchronous update check
 *
 * Tests the async wrapper using std::async
 */
void test_async_update_check() {
    try {
        std::cout << "  Starting asynchronous check...\n";

        auto start = std::chrono::steady_clock::now();

        // Start async operation
        auto future = ghupdate::check_github_update_async(
            "https://github.com/nlohmann/json",
            "0.5.0"
        );

        std::cout << "  Async operation started, waiting for result...\n";

        // Wait for result
        auto result = future.get();

        auto end = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

        bool pass = !result.latestVersion.empty();

        std::cout << "  Async completed in " << duration.count() << "ms\n";
        std::cout << "  Latest version found: " << result.latestVersion << "\n";
        std::cout << "  Has update: " << (result.hasUpdate ? "YES" : "NO") << "\n";

        print_result("Async update check", pass);
    } catch (const std::exception& e) {
        std::cerr << "  Exception: " << e.what() << "\n";
        print_result("Async update check", false);
    }
}

/*!
 * @brief Test 6: Version comparison - no update needed
 *
 * Verifies that check correctly identifies when local version is current
 */
void test_no_update_needed() {
    try {
        std::cout << "  Checking with very recent local version...\n";

        auto result = ghupdate::check_github_update(
            "https://github.com/nlohmann/json",
            "999.0.0"  // Version higher than any real release
        );

        bool pass = !result.hasUpdate;

        std::cout << "  Remote version: " << result.latestVersion << "\n";
        std::cout << "  Has update: " << (result.hasUpdate ? "YES" : "NO") << "\n";

        print_result("No update needed (local > remote)", pass);
    } catch (const std::exception& e) {
        std::cerr << "  Exception: " << e.what() << "\n";
        print_result("No update needed (local > remote)", false);
    }
}

/*!
 * @brief Test 7: Error handling - invalid GitHub URL
 *
 * Verifies proper exception handling for malformed URLs
 */
void test_invalid_url() {
    try {
        auto result = ghupdate::check_github_update(
            "https://invalid-host.com/some/repo",
            "1.0.0"
        );

        print_result("Invalid URL error handling", false);  // Should not reach here
    } catch (const std::runtime_error& e) {
        std::cout << "  Expected error caught: " << e.what() << "\n";
        print_result("Invalid URL error handling", true);
    } catch (const std::exception& e) {
        std::cerr << "  Unexpected exception type: " << e.what() << "\n";
        print_result("Invalid URL error handling", false);
    }
}

/*!
 * @brief Test 8: Error handling - invalid version format
 *
 * Verifies proper exception handling for malformed version strings
 */
void test_invalid_version_format() {
    try {
        auto result = ghupdate::check_github_update(
            "https://github.com/nlohmann/json",
            "invalid-version"
        );

        print_result("Invalid version format error handling", false);  // Should not reach here
    } catch (const std::runtime_error& e) {
        std::cout << "  Expected error caught: " << e.what() << "\n";
        print_result("Invalid version format error handling", true);
    } catch (const std::exception& e) {
        std::cerr << "  Unexpected exception type: " << e.what() << "\n";
        print_result("Invalid version format error handling", false);
    }
}

/*!
 * @brief Print test summary statistics
 */
void print_summary() {
    const char* separator = "==================================================";
    std::cout << "\n" << separator << "\n";
    std::cout << "TEST SUMMARY\n";
    std::cout << separator << "\n";
    std::cout << "Passed: " << tests_passed << "\n";
    std::cout << "Failed: " << tests_failed << "\n";
    std::cout << "Total:  " << (tests_passed + tests_failed) << "\n";
    std::cout << separator << "\n";
}

/*!
 * @brief Main test entry point
 *
 * @return Exit code: 0 if all tests pass, 1 if any test fails
 */
int main() {
    std::cout << "========================================\n";
    std::cout << "gh-update-checker Test Suite\n";
    std::cout << "========================================\n\n";

    std::cout << "--- Unit Tests ---\n";
    test_semver_parsing();
    test_semver_comparison();

    std::cout << "\n--- Integration Tests (requires network) ---\n";
    test_sync_update_check_standard_url();
    std::cout << "\n";
    test_sync_update_check_api_url();
    std::cout << "\n";
    test_async_update_check();
    std::cout << "\n";
    test_no_update_needed();

    std::cout << "\n--- Error Handling Tests ---\n";
    test_invalid_url();
    test_invalid_version_format();

    print_summary();

    return tests_failed > 0 ? 1 : 0;
}

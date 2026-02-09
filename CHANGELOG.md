<!-- START doctoc generated TOC please keep comment here to allow auto update -->
<!-- DON'T EDIT THIS SECTION, INSTEAD RE-RUN doctoc TO UPDATE -->

**Table of Contents**

- [Changelog](#changelog)
  - [[Unreleased]](#unreleased)
    - [Added](#added)
    - [Changed](#changed)
    - [Fixed](#fixed)
  - [[1.0.4] - 2026-02-09](#104---2026-02-09)
    - [Fixed](#fixed-2)
    - [Changed](#changed-2)
    - [Added](#added-2)
  - [[1.0.1] - 2026-02-10](#101---2026-02-10)
    - [Fixed](#fixed-1)
    - [Changed](#changed-1)
    - [Added](#added-1)
  - [[1.0.0] - 2026-02-09](#100---2026-02-09)
    - [Added](#added-3)
    - [Documentation](#documentation)
    - [Testing](#testing)
    - [Build & Packaging](#build--packaging)
    - [Known Limitations](#known-limitations)
    - [Dependencies](#dependencies)
    - [Contributors](#contributors)
  - [Versioning Strategy](#versioning-strategy)
  - [Release Process](#release-process)
  - [Links](#links)
    - [Format Guide](#format-guide)

<!-- END doctoc generated TOC please keep comment here to allow auto update -->

# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

### Added

- Support for GitHub OAuth token authentication (optional)
- Custom timeout configuration for network requests
- Support for pre-release versions in semantic versioning parser
- Integration with GitHub Actions workflow examples
- Docker support for containerized CLI usage

### Changed

- Improved error messages with more diagnostic information
- Async version now uses `std::jthread` instead of `std::async` (C++20 compatibility)
- Enhanced documentation with additional examples and recipes

### Fixed

- Network timeout handling improvements
- Better error recovery for transient network failures

## [1.0.4] - 2026-02-09

### Fixed

- Fixed GitHub Actions workflow permissions for SBOM commit operations
- Corrected SBOM and CVE report handling in release job
- Added proper git credentials persistence in checkout steps
- Fixed compilation database filtering for cppcheck with FetchContent dependencies
- Resolved race condition in release job where SBOMs weren't available at execution time

### Changed

- Removed automatic git commit of SBOM files to repository (now only attached to releases)
- Improved release job workflow to properly wait for all prerequisite jobs (codeql, cppcheck, clang_tidy, cvecheck)
- Enhanced release asset collection with glob patterns for better flexibility
- Refactored CHANGELOG extraction for release notes

### Added

- Added `persist-credentials: true` to GitHub Actions checkout step for proper git operations
- Improved error handling in release asset preparation with fallback mechanisms
- Added `continue-on-error: true` to release creation for robustness
- Better logging and diagnostics in GitHub Actions workflow steps
- Automatic CHANGELOG section extraction for release body

## [1.0.1] - 2026-02-10

### Fixed

- Fixed empty CVE reports in GitHub Actions workflow
- Corrected SBOM file path handling in release job
- Resolved cppcheck report generation issues with proper output stream redirection
- Fixed clang-tidy file discovery when compile_commands.json is empty
- Improved error handling for invalid GitHub API responses with timeout considerations

### Changed

- Enhanced CVE scanning with dual-level reporting (Critical + All vulnerabilities)
- Improved debug output in GitHub Actions workflow for better troubleshooting
- Refined SBOM artifact handling and validation in release pipeline

### Added

- CVE summary text report (`cve-summary.txt`) for human-readable vulnerability status
- Diagnostic output for all security scanning tools with timestamps
- Automatic SBOM file validation before CVE scanning
- Separate critical (CVSS ≥ 9) and comprehensive (CVSS ≥ 0) CVE reports

## [1.0.0] - 2026-02-09

### Added

- Initial release of gh-update-checker library and CLI tool
- Synchronous update checking with `check_github_update()`
- Asynchronous update checking with `check_github_update_async()`
- Semantic versioning (SemVer) parser with full version comparison support
- Automatic GitHub URL normalization (standard URL → API URL conversion)
- Support for multiple URL formats:
  - Standard GitHub URLs: `https://github.com/owner/repo`
  - GitHub API URLs: `https://api.github.com/repos/owner/repo/releases/latest`
  - GitHub API URLs with .git suffix
- HTTP/HTTPS requests via libcurl with robust error handling
- JSON response parsing via nlohmann/json
- Header-only library design for easy integration
- CMake build system with FetchContent dependency management
- Command-line interface with configurable exit codes:
  - Exit code 0: No update available
  - Exit code 1: Usage error
  - Exit code 2: Update available
  - Exit code 3: Runtime/network error
- Comprehensive test suite with 8 integration and unit tests
- Full Doxygen-style documentation for all public APIs
- Support for C++23 modern features:
  - `std::print` for formatted output
  - `std::string_view` for zero-copy parameters
  - Three-way comparison operator (`<=>`) for version comparison
- Cross-platform support:
  - Linux (GCC 13.2+, Clang 17+)
  - macOS (Apple Clang, GCC, Clang)
  - Windows (MSVC 2022+)
- CMake export targets for integrate-as-separate-package use cases
- Installation support with custom prefixes
- Comprehensive error handling with descriptive exceptions
- Performance optimizations:
  - Zero-copy with string_view parameters
  - Async support for non-blocking operations
  - Efficient memory usage

### Documentation

- Comprehensive README.md with usage examples and troubleshooting
- Inline documentation with Doxygen comments
- Examples for synchronous and asynchronous usage
- CLI usage guide with exit code documentation
- Build and installation instructions
- Integration guides for CMake projects

### Testing

- `test_basic`: Integration test suite including:
  - SemVer parsing tests
  - Version comparison tests
  - Synchronous update checking (standard and API URLs)
  - Asynchronous update checking with timing measurements
  - Error handling tests (invalid URLs, invalid versions)
  - Version comparison edge cases

### Build & Packaging

- CMake 3.23+ support with modern practices
- FetchContent-based dependency management for nlohmann/json and libcurl
- Configurable installation paths
- CMake package configuration files for find_package() support
- Support for both header-only and linked configurations

### Known Limitations

- No built-in rate limiting protection (GitHub API rate limits apply)
- Requires network connectivity for update checks
- No authentication support in v1.0.0 (planned for v1.1.0)
- Default libcurl timeout is system-dependent (user-configurable in future versions)

### Dependencies

- **nlohmann/json** 3.11.3+ for JSON parsing
- **libcurl** 8.7.1+ for HTTP/HTTPS requests
- **C++23 compatible compiler**:
  - GCC 13.2 or later
  - Clang 17 or later
  - MSVC 2022 or later with `/std:c++latest`

### Contributors

- ZHENG Robert - Original author and maintainer

---

## Versioning Strategy

This project uses [Semantic Versioning](https://semver.org/):

- **MAJOR**: Breaking API changes or significant feature additions
- **MINOR**: New features, backwards compatible
- **PATCH**: Bug fixes, backwards compatible

## Release Process

1. Update version numbers in `CMakeLists.txt` and code comments
2. Update CHANGELOG.md with new version section
3. Create git tag: `git tag -a v1.0.0 -m "Release 1.0.0"`
4. Push to repository: `git push origin main --tags`
5. Create GitHub release with release notes

## Links

- [Repository](https://github.com/yourusername/gh-update-checker)
- [Issues](https://github.com/yourusername/gh-update-checker/issues)
- [Releases](https://github.com/yourusername/gh-update-checker/releases)

---

### Format Guide

For future releases, use this structure:

```markdown
## [X.Y.Z] - YYYY-MM-DD

### Added

- List new features

### Changed

- List changes to existing functionality

### Deprecated

- List soon-to-be removed features

### Removed

- List removed features

### Fixed

- List bug fixes

### Security

- List security patches and updates
```

Categories to use (in order):

- **Added** - New features and additions
- **Changed** - Changes to existing functionality
- **Deprecated** - Soon-to-be removed features (mark as deprecated, don't remove yet)
- **Removed** - Previously deprecated features that are now removed
- **Fixed** - Bug fixes
- **Security** - Security vulnerabilities and patches

---

**Last Updated**: 2026-02-10
**Current Stable Version**: [1.0.1]
**Development Version**: [Unreleased]

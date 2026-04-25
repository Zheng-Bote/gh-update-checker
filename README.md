<div id="top" align="center">
<h1>gh-update-checker</h1>

<p>A modern C++23 header-only library to check for the latest releases of GitHub repositories using semantic versioning (SemVer).</p>

![License: LGPL-3](https://img.shields.io/badge/LGPL-3.0-lightgrey)
[![CLA assistant](https://cla-assistant.io/readme/badge/Zheng-Bote/gh-update-checker)](https://cla-assistant.io/Zheng-Bote/gh-update-checker)
![Platform](https://img.shields.io/badge/platform-Linux%20%7C%20Windows%20%7C%20macOS-lightgrey.svg)
[![GitHub release (latest by date)](https://img.shields.io/github/v/release/Zheng-Bote/gh-update-checker?logo=GitHub)](https://github.com/Zheng-Bote/gh-update-checker/releases)

[Report Issue](https://github.com/Zheng-Bote/gh-update-checker/issues) · [Request Feature](https://github.com/Zheng-Bote/gh-update-checker/pulls)

</div>

---

<!-- START doctoc generated TOC please keep comment here to allow auto update -->
<!-- DON'T EDIT THIS SECTION, INSTEAD RE-RUN doctoc TO UPDATE -->
**Table of Contents**

  - [Overview](#overview)
  - [Features](#features)
  - [Requirements](#requirements)
    - [Build Requirements](#build-requirements)
    - [Runtime Requirements](#runtime-requirements)
  - [Installation](#installation)
    - [Option 1: Build from Source (Recommended)](#option-1-build-from-source-recommended)
      - [Prerequisites](#prerequisites)
      - [Clone and Build](#clone-and-build)
      - [Install](#install)
    - [Option 2: Using as a Header-Only Library in CMakeLists.txt](#option-2-using-as-a-header-only-library-in-cmakeliststxt)
  - [Usage](#usage)
    - [Command-Line Interface](#command-line-interface)
      - [Basic Usage](#basic-usage)
      - [Using GitHub API URLs](#using-github-api-urls)
      - [Exit Codes](#exit-codes)
      - [Practical Examples](#practical-examples)
    - [C++ Library Usage](#c-library-usage)
      - [Synchronous Update Check](#synchronous-update-check)
      - [Asynchronous Update Check](#asynchronous-update-check)
      - [Parsing Semantic Versions](#parsing-semantic-versions)
    - [CMakeLists.txt Integration](#cmakeliststxt-integration)
  - [Building and Testing](#building-and-testing)
    - [Build Options](#build-options)
    - [Running Tests](#running-tests)
    - [Test Coverage](#test-coverage)
  - [Development](#development)
    - [Project Structure](#project-structure)
    - [Building with Different Compilers](#building-with-different-compilers)
      - [GCC](#gcc)
      - [Clang](#clang)
      - [MSVC (Windows)](#msvc-windows)
    - [Code Style and Standards](#code-style-and-standards)
    - [Compiler Flags](#compiler-flags)
  - [Error Handling](#error-handling)
  - [Performance Considerations](#performance-considerations)
  - [Troubleshooting](#troubleshooting)
    - [Build Issues](#build-issues)
      - [CMake not found](#cmake-not-found)
      - [C++23 not supported](#c23-not-supported)
      - [cpp-httplib or nlohmann/json not found](#cpp-httplib-or-nlohmannjson-not-found)
    - [Runtime Issues](#runtime-issues)
      - [Network timeouts](#network-timeouts)
      - [Invalid version format](#invalid-version-format)
      - [Rate limiting](#rate-limiting)
  - [Dependencies](#dependencies)
    - [External (Auto-fetched)](#external-auto-fetched)
    - [System](#system)
  - [License](#license)
  - [Contributing](#contributing)
  - [Examples and Recipes](#examples-and-recipes)
    - [Checking Multiple Repositories](#checking-multiple-repositories)
    - [Integration with Build Systems](#integration-with-build-systems)
  - [Support](#support)
  - [Changelog](#changelog)
- [Contributing](#contributing-1)
- [License](#license-1)
- [Author](#author)
  - [Code Contributors](#code-contributors)

<!-- END doctoc generated TOC please keep comment here to allow auto update -->

---

## Overview


[![C++23](https://img.shields.io/badge/C%2B%2B-23-blue.svg)]()
[![CMake](https://img.shields.io/badge/CMake-3.23+-blue.svg)]()
[![Conan](https://img.shields.io/badge/Conan-2.x-blue.svg)]()

**gh-update-checker** provides an efficient way to check if a newer version of a GitHub repository release is available. It automatically handles:

- GitHub URL normalization (converts standard GitHub URLs to API endpoints)
- Semantic versioning parsing and comparison
- JSON response parsing via `nlohmann/json`
- HTTP requests via `cpp-httplib`
- Both synchronous and asynchronous update checking

Perfect for:

- Build systems and CI/CD pipelines
- Package managers
- Automated deployment tools
- Version control in applications

## Features

- **C++23 Modern Features**
  - `std::print` for formatted output
  - `std::expected` for error handling (via std::optional)
  - Three-way comparison operator for version comparison
  - String views to minimize allocations

- **Flexible Input Handling**
  - Accepts full GitHub URLs: `https://github.com/owner/repo`
  - Accepts GitHub API URLs: `https://api.github.com/repos/owner/repo/releases/latest`
  - Auto-converts standard URLs to API format

- **Semantic Versioning Support**
  - Parse and compare versions: `1.2.3`, `v1.2.3`, `1.2`
  - Precise three-component versioning (major.minor.patch)

- **Both Sync and Async APIs**
  - Synchronous: `check_github_update()`
  - Asynchronous: `check_github_update_async()` using `std::async`

- **Header-Only Library**
  - Easy integration with a single include
  - Configurable via CMake for linking external dependencies

## Requirements

### Build Requirements

- **C++23 compliant compiler**
  - GCC 13.2+
  - Clang 17+
  - MSVC 2022+ with `/std:c++latest`
- **CMake** 3.23 or later
- **Conan** 2.x

### Runtime Requirements

- Network connectivity for GitHub API calls
- HTTPS support via `cpp-httplib` (OpenSSL backend)

## Installation

### Option 1: Build from Source (Recommended)

#### Prerequisites

```bash
# Ubuntu/Debian
sudo apt-get install cmake ninja-build build-essential git python3-pip
pipx install conan || pip install --user conan

# macOS (Homebrew)
brew install cmake ninja conan

# Windows
# Use Visual Studio 2022 and install Conan
choco install cmake ninja visualstudio2022community conan
```

#### Clone and Build

```bash
# Clone the repository
git clone https://github.com/Zheng-Bote/gh-update-checker.git
cd gh-update-checker

# Install Conan dependencies (Release)
conan profile detect --force
conan install . --build=missing -s build_type=Release

# Configure with CMake preset
cmake --preset conan-release

# Build the project
cmake --build --preset conan-release

# Optionally run tests
ctest --preset conan-release
```

#### Install

```bash
# Install to system
sudo cmake --install build/Release

# Or install to custom location
cmake --install build/Release --prefix ~/local
```

### Option 2: Using as a Header-Only Library in CMakeLists.txt

```cmake
find_package(gh_update_checker REQUIRED)

# In your target
target_link_libraries(my_target PRIVATE gh_update_checker::gh_update_checker)
```

## Usage

### Command-Line Interface (Example Only, Not Built by Default)

#### Basic Usage

```bash
# Check if nlohmann/json has an update
gh-update-checker https://github.com/nlohmann/json 3.11.2

# Output:
# Local version:  3.11.2
# Remote version: 3.11.3
# Update:         YES
```

#### Using GitHub API URLs

```bash
gh-update-checker https://api.github.com/repos/nlohmann/json/releases/latest 3.11.2
```

#### Exit Codes

- **0**: Success - no update available (local version is current)
- **1**: Usage error - invalid number of arguments
- **2**: Success - update available (newer version found on GitHub)
- **3**: Runtime error - network, API parsing, or version parsing error

#### Practical Examples

```bash
# In shell scripts
if gh-update-checker https://github.com/nlohmann/json "$(installed_version)" ; then
    echo "Your version is up to date"
else
    echo "An update is available"
fi

# In CI/CD pipelines
gh-update-checker "https://github.com/nlohmann/json" "$CURRENT_VERSION"
UPDATE_EXIT=$?

if [ $UPDATE_EXIT -eq 2 ]; then
    echo "::warning::Update available for dependency"
    # Trigger update workflow
fi
```

### C++ Library Usage

#### Synchronous Update Check

```cpp
#include <check_gh-update.hpp>
#include <iostream>

int main() {
    try {
        // Check using GitHub repository URL
        auto result = ghupdate::check_github_update(
            "https://github.com/nlohmann/json",
            "3.11.2"
        );

        std::cout << "Remote version: " << result.latestVersion << "\n";

        if (result.hasUpdate) {
            std::cout << "Update available!\n";
        } else {
            std::cout << "You are running the latest version.\n";
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }
    return 0;
}
```

#### Asynchronous Update Check

```cpp
#include <check_gh-update.hpp>
#include <iostream>

int main() {
    try {
        // Start async check
        auto future = ghupdate::check_github_update_async(
            "https://github.com/nlohmann/json",
            "3.11.2"
        );

        // Do other work while request is in progress
        std::cout << "Checking for updates...\n";

        // Wait for result (blocking)
        auto result = future.get();

        std::cout << "Latest version: " << result.latestVersion << "\n";

        if (result.hasUpdate) {
            std::cout << "Update available!\n";
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }
    return 0;
}
```

#### Parsing Semantic Versions

```cpp
#include <check_gh-update.hpp>

int main() {
    try {
        // Parse version strings
        auto v1 = ghupdate::SemVer::parse("v1.2.3");
        auto v2 = ghupdate::SemVer::parse("1.3.0");

        // Compare versions
        if (v2 > v1) {
            std::cout << "Version 1.3.0 is newer\n";
        }
    } catch (const std::exception& e) {
        std::cerr << "Invalid version format: " << e.what() << "\n";
        return 1;
    }
    return 0;
}
```

### CMakeLists.txt Integration

```cmake
cmake_minimum_required(VERSION 3.23)
project(MyProject LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)

# Add gh-update-checker (via find_package if installed)
find_package(gh_update_checker REQUIRED)

# Link to your target
add_executable(my_app main.cpp)
target_link_libraries(my_app PRIVATE gh_update_checker::gh_update_checker)
```

## Building and Testing

### Build Options

```bash
conan install . --build=missing -s build_type=Release
cmake --preset conan-release

# Standard release build
cmake --build --preset conan-release

# Debug build with symbols
conan install . --build=missing -s build_type=Debug
cmake --preset conan-debug
cmake --build --preset conan-debug

# Verbose build output
cmake --build --preset conan-release --verbose
```

### Running Tests

```bash
# Run all tests
ctest --preset conan-release

# Run with verbose output
ctest --preset conan-release --output-on-failure

# Run specific test
ctest --test-dir build/Release -R test_basic -V

# Or using the test executable directly
./build/Release/test_basic
```

### Test Coverage

The project includes basic functional tests:

- **test_basic**: Tests API URL parsing and synchronous update checking

Expected test behavior:

```
Test the check_github_update() function with:
- Valid GitHub URLs (standard and API format)
- Real GitHub API calls to nlohmann/json
- Version comparison logic
```

Run the test suite after building:

```bash
ctest --preset conan-release
# or
ctest --test-dir build/Release --output-on-failure
```

## Development

### Project Structure

```
gh-update-checker/
├── include/
│   └── check_gh-update.hpp      # Main header-only library
├── src/                          # Source files (if any)
│   └── (currently using header-only pattern)
├── examples/
│   └── gh-update-checker.cpp     # Example CLI (not built by default)
├── tests/
│   └── test_basic.cpp            # Basic integration tests
├── cmake/
│   └── gh_update_checkerConfig.cmake.in
├── CMakeLists.txt
└── README.md
```

### Building with Different Compilers

#### GCC

```bash
conan install . --build=missing -s build_type=Release
cmake --preset conan-release -DCMAKE_CXX_COMPILER=g++
cmake --build --preset conan-release
```

#### Clang

```bash
conan install . --build=missing -s build_type=Release
cmake --preset conan-release -DCMAKE_CXX_COMPILER=clang++
cmake --build --preset conan-release
```

#### MSVC (Windows)

```bash
conan install . --build=missing -s build_type=Release
cmake --preset conan-release
cmake --build --preset conan-release
```

### Code Style and Standards

- **C++23** - Latest standard features
- **Compiler Flags**: `-Wall -Wextra -Wpedantic -Wshadow -Wconversion`
- **Documentation**: Doxygen-style comments with examples
- **Naming**: `snake_case` for functions/variables, `PascalCase` for classes

### Compiler Flags

The project uses strict compiler warnings:

- GCC/Clang: `-std=c++23 -Wall -Wextra -Wpedantic -Wshadow -Wconversion`
- MSVC: `/std:c++latest /W4`

## Error Handling

All functions throw `std::runtime_error` for error conditions:

```cpp
// Network errors
// Invalid GitHub URLs
// Invalid version format
// GitHub API errors (rate limiting, repository not found, etc.)
```

Catch exceptions appropriately:

```cpp
try {
    auto result = ghupdate::check_github_update(url, version);
} catch (const std::runtime_error& e) {
    // Handle network or parsing errors
    std::cerr << "Error: " << e.what() << "\n";
} catch (const std::exception& e) {
    // Handle other exceptions
    std::cerr << "Unexpected error: " << e.what() << "\n";
}
```

## Performance Considerations

- **String Views**: Minimize copies with `std::string_view` parameters
- **Network Timeouts**: `cpp-httplib` connection/read/write timeouts are configured in `http_get()`
- **Async Operations**: Use `check_github_update_async()` for non-blocking calls
- **Memory**: Asynchronous checks use `std::async` which spawns lightweight threads on most systems

## Troubleshooting

### Build Issues

#### CMake not found

```bash
# Install CMake
Ubuntu: sudo apt-get install cmake
macOS:  brew install cmake
```

#### C++23 not supported

Ensure your compiler is updated:

- GCC: 14 or later
- G++: 14 or later
- Clang: 17 or later

#### cpp-httplib or nlohmann/json not found

Install dependencies with Conan before configuring CMake:

```bash
conan profile detect --force
conan install . --build=missing -s build_type=Release
```

### Runtime Issues

#### Network timeouts

Check network connectivity and GitHub API availability:

```bash
curl https://api.github.com/repos/nlohmann/json/releases/latest
```

#### Invalid version format

Ensure version strings follow SemVer: `major.minor[.patch]` or `vmajor.minor[.patch]`

#### Rate limiting

GitHub API has rate limits. For authenticated requests, set an OAuth token:

```cpp
// Future versions may support authentication headers
```

## Dependencies

### External (Conan-managed)

- **nlohmann/json** v3.11.3 - JSON parsing
- **cpp-httplib** v0.39.0 - HTTP requests

### System

- Standard C++ Library (C++23)
- System SSL/TLS runtime libraries as required by `cpp-httplib` OpenSSL support

## License

This project is licensed under the MIT License - see the LICENSE file for details.

SPDX-License-Identifier: MIT

## Contributing

Contributions are welcome! Please:

1. Fork the repository
2. Create a feature branch
3. Make your changes
4. Add tests for new functionality
5. Submit a pull request

## Examples and Recipes

### Checking Multiple Repositories

```cpp
#include <check_gh-update.hpp>
#include <vector>
#include <iostream>

int main() {
    std::vector<std::pair<std::string, std::string>> repos = {
        {"https://github.com/nlohmann/json", "3.11.2"},
        {"https://github.com/curl/curl", "8.19.0"},
    };

    for (const auto& [url, version] : repos) {
        try {
            auto result = ghupdate::check_github_update(url, version);
            std::cout << url << ": " << (result.hasUpdate ? "UPDATE" : "OK") << "\n";
        } catch (const std::exception& e) {
            std::cerr << url << ": ERROR - " << e.what() << "\n";
        }
    }
    return 0;
}
```

### Integration with Build Systems

```bash
#!/bin/bash
# check_dependencies.sh - Verify all dependencies are current

DEPS=(
    "https://github.com/nlohmann/json::3.11.2"
    "https://github.com/curl/curl::8.19.0"
)

for dep in "${DEPS[@]}"; do
    url="${dep%::*}"
    version="${dep##*::}"

    if ! gh-update-checker "$url" "$version"; then
        echo "Update available for $url"
        exit 2
    fi
done

echo "All dependencies are current"
exit 0
```

## Support

For issues, questions, or suggestions:

- Open an issue on GitHub
- Check existing documentation and examples
- Review the test cases for usage patterns

## Changelog

See [CHANGELOG.md](CHANGELOG.md) for version history.

# Contributing

Contributions are welcome! Please fork the repository and create a pull request.

1. Fork the Project
2. Create your Feature Branch (git checkout -b feature/AmazingFeature)
3. Commit your Changes (git commit -m 'Add some AmazingFeature')
4. Push to the Branch (git push origin feature/AmazingFeature)
5. Open a Pull Request

# License

Distributed under the LGPL-3.0 License. See LICENSE for more information.

Copyright (c) 2026 ZHENG Robert

# Author

[![Zheng Robert - Core Development](https://img.shields.io/badge/Github-Zheng_Robert-black?logo=github)](https://www.github.com/Zheng-Bote)

## Code Contributors

![Contributors](https://img.shields.io/github/contributors/Zheng-Bote/gh-update-checker?color=dark-green)

---

**Happy checking! 🚀** :vulcan_salute:

<p align="right">(<a href="#top">back to top</a>)</p>
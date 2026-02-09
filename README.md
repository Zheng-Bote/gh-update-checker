# gh-update-checker

A modern C++23 library and CLI tool to check for the latest releases of GitHub repositories using semantic versioning (SemVer).

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![C++23](https://img.shields.io/badge/C%2B%2B-23-blue.svg)]()
[![CMake](https://img.shields.io/badge/CMake-3.23+-blue.svg)]()

## Overview

**gh-update-checker** provides an efficient way to check if a newer version of a GitHub repository release is available. It automatically handles:

- GitHub URL normalization (converts standard GitHub URLs to API endpoints)
- Semantic versioning parsing and comparison
- JSON response parsing via `nlohmann/json`
- HTTP requests via `libcurl`
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
- **libcurl** (fetched automatically or use system package)
- **nlohmann/json** (fetched automatically or use system package)

### Runtime Requirements

- Network connectivity for GitHub API calls
- HTTPS support in libcurl

## Installation

### Option 1: Build from Source (Recommended)

#### Prerequisites

```bash
# Ubuntu/Debian
sudo apt-get install cmake build-essential git

# macOS (Homebrew)
brew install cmake

# Windows
# Use Visual Studio 2022 or install via chocolatey
choco install cmake visualstudio2022community
```

#### Clone and Build

```bash
# Clone the repository
git clone https://github.com/yourusername/gh-update-checker.git
cd gh-update-checker

# Create build directory
mkdir build
cd build

# Configure CMake (optional: set custom installation path)
cmake -DCMAKE_INSTALL_PREFIX=/usr/local ..

# Build the project
cmake --build .

# Optionally run tests
ctest --output-on-failure
```

#### Install

```bash
# Install to system
sudo cmake --install .

# Or install to custom location
cmake --install . --prefix ~/local
```

### Option 2: Using as a Header-Only Library in CMakeLists.txt

```cmake
include(FetchContent)

FetchContent_Declare(
    gh_update_checker
    GIT_REPOSITORY https://github.com/yourusername/gh-update-checker.git
    GIT_TAG main
)
FetchContent_MakeAvailable(gh_update_checker)

# In your target
target_link_libraries(my_target PRIVATE gh_update_checker)
```

## Usage

### Command-Line Interface

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

# Add gh-update-checker (via find_package if installed, or FetchContent)
find_package(gh_update_checker REQUIRED)

# Or use FetchContent
include(FetchContent)
FetchContent_Declare(
    gh_update_checker
    GIT_REPOSITORY https://github.com/yourusername/gh-update-checker.git
)
FetchContent_MakeAvailable(gh_update_checker)

# Link to your target
add_executable(my_app main.cpp)
target_link_libraries(my_app PRIVATE gh_update_checker::gh_update_checker)
```

## Building and Testing

### Build Options

```bash
cd build

# Standard release build
cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr/local ..
cmake --build .

# Debug build with symbols
cmake -DCMAKE_BUILD_TYPE=Debug ..
cmake --build .

# Verbose build output
cmake --build . --verbose
```

### Running Tests

```bash
# From the build directory
cd build

# Run all tests
ctest

# Run with verbose output
ctest --output-on-failure

# Run specific test
ctest -R test_basic -V

# Or using the test executable directly
./test_basic
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
make test
# or
ctest --output-on-failure
```

## Development

### Project Structure

```
gh-update-checker/
├── include/
│   └── check_gh-update.hpp      # Main header-only library
├── src/                          # Source files (if any)
│   └── (currently using header-only pattern)
├── cli/
│   └── gh-update-checker.cpp     # CLI application
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
cmake -DCMAKE_CXX_COMPILER=g++ ..
cmake --build .
```

#### Clang

```bash
cmake -DCMAKE_CXX_COMPILER=clang++ ..
cmake --build .
```

#### MSVC (Windows)

```bash
cmake -G "Visual Studio 17 2022" ..
cmake --build . --config Release
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
- **Network Timeouts**: Default libcurl timeout is system-dependent; consider setting `CURLOPT_TIMEOUT` for production
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

- GCC: 13.2 or later
- Clang: 17 or later

#### libcurl or nlohmann/json not found

These are fetched automatically by CMake. If offline, install manually:

```bash
# Ubuntu
sudo apt-get install libcurl4-openssl-dev nlohmann-json3-dev

# macOS
brew install curl nlohmann-json
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

### External (Auto-fetched)

- **nlohmann/json** v3.11.3 - JSON parsing
- **libcurl** v8.7.1 - HTTP requests

### System

- Standard C++ Library (C++23)
- System SSL/TLS for HTTPS support

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
        {"https://github.com/curl/curl", "8.7.0"},
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
    "https://github.com/curl/curl::8.7.0"
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

---

**Happy checking! 🚀**

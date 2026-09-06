from conan import ConanFile
from conan.tools.cmake import CMake, CMakeDeps, CMakeToolchain, cmake_layout


class GhUpdateCheckerConan(ConanFile):
    name = "gh-update-checker"
    version = "2.0.0"
    package_type = "header-library"
    license = "LGPL-3.0"
    author = "ZHENG Robert <robert@hase-zheng.net>"
    url = "https://github.com/Zheng-Bote/gh-update-checker"
    description = "Header-only C++23 library to check GitHub release updates"
    topics = ("Utilities", "GitHub", "Update Checker", "Header-only", "C++23")
    exports_sources = (
        "CMakeLists.txt",
        "cmake/*",
        "include/*",
        "tests/*",
        "examples/*",
        "LICENSE",
        "README.md",
    )

    settings = "os", "compiler", "build_type", "arch"

    requires = (
        "nlohmann_json/[>=3.12.0 <4]",
        "libcurl/[>=8.0.0 <9]",
    )

    default_options = {
        "libcurl/*:with_ssl": "openssl",
    }

    generators = ()

    def layout(self):
        cmake_layout(self)

    def generate(self):
        deps = CMakeDeps(self)
        deps.generate()

        toolchain = CMakeToolchain(self)
        toolchain.cache_variables["CMAKE_CXX_STANDARD"] = "23"
        toolchain.cache_variables["CMAKE_CXX_STANDARD_REQUIRED"] = "ON"
        toolchain.generate()

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def package(self):
        cmake = CMake(self)
        cmake.install()

    def package_info(self):
        self.cpp_info.set_property("cmake_file_name", "gh_update_checker")
        self.cpp_info.set_property("cmake_target_name", "gh_update_checker::gh_update_checker")

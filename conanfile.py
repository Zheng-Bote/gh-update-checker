from conan import ConanFile
from conan.tools.cmake import CMake, CMakeDeps, CMakeToolchain, cmake_layout


class GhUpdateCheckerConan(ConanFile):
    name = "gh-update-checker"
    version = "1.0.8"
    package_type = "application"

    settings = "os", "compiler", "build_type", "arch"

    requires = (
        "nlohmann_json/[>=3.12.0]",
        "cpp-httplib/[>=0.39.0]",
    )

    default_options = {
        "cpp-httplib/*:with_openssl": True,
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

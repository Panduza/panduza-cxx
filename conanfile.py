from conans import ConanFile
from conan.tools.cmake import CMake, CMakeToolchain, CMakeDeps, cmake_layout
import os
import re

class PzaCxx(ConanFile):
    name = "libpza-cxx"
    settings = "os", "compiler", "build_type", "arch"
    options = {"shared": [True, False]}
    generators = "CMakeDeps", "CMakeToolchain"
    default_options = {"shared": True}
    exports_sources = "CMakeLists.txt", "source/*", "version.h.in", "CHANGELOG.md", "test/*"

    def requirements(self):
        self.options["*"].shared = self.options.shared
        self.requires("paho-mqtt-cpp/1.2.0")
        self.requires("spdlog/1.11.0")
        self.requires("nlohmann_json/3.11.2")
        self.requires("gtest/cci.20210126")
        self.requires("cppcheck/2.10")

    def set_version(self):
        version_regex = re.compile(r"^#\s+\[(\d+\.\d+\.\d+)\]")
        with open(self.recipe_folder + "/CHANGELOG.md", "r") as f:
            changelog = f.read()
        match = version_regex.search(changelog)
        if match:
            self.version = match.group(1)
            return
        else:
            raise Exception("Failed to extract version from changelog")

    def layout(self):
        cmake_layout(self)

    def generate(self):
        tc = CMakeToolchain(self)
        tc.generate()
        deps = CMakeDeps(self)
        deps.generate()

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def package(self):
        cmake = CMake(self)
        cmake.install()

    def package_info(self):
        self.cpp_info.name = "Panduza C++ Library"
        self.cpp_info.libs = ["pza-cxx"]
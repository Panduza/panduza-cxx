from conans import ConanFile
from conan.tools.cmake import CMake, CMakeToolchain, CMakeDeps, cmake_layout
import os
import re

class PzaCxx(ConanFile):
    name = "libpza-cxx"
    settings = "os", "compiler", "build_type", "arch"
    options = {
        "shared": [True, False],
        "build_examples": [True, False]
    }
    default_options = {
        "shared": True,
        "build_examples": False
    }
    generators = "CMakeDeps", "CMakeToolchain", "virtualrunenv"
    exports_sources = "CMakeLists.txt", "source/*", "version.h.in", "CHANGELOG.md", "test/*", "cmake/*", "examples/*", "LICENSE"

    def requirements(self):
        self.requires("paho-mqtt-cpp/[>=1.2.0]")
        self.requires("spdlog/[>=1.11.0]")
        self.requires("nlohmann_json/[>=3.11.2]")
        self.requires("gtest/cci.20210126")
        self.requires("cppcheck/[>=2.10]")
        self.requires("magic_enum/[>=0.9.2]")

    def layout(self):
        cmake_layout(self, build_folder=os.getcwd())

    def configure(self):
        if self.settings.os == "Windows":
            self.options["*"].shared = False
        else:
            self.options["*"].shared = self.options.shared

    def generate(self):
        tc = CMakeToolchain(self)
        tc.variables["BUILD_EXAMPLES"] = self.options.build_examples
        tc.filename = "pzacxx_toolchain.cmake"
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
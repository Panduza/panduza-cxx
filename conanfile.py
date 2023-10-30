from conan import ConanFile
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
        "build_examples": True
    }

    def requirements(self):
        self.requires("paho-mqtt-cpp/[>=1.2.0]")
        self.requires("spdlog/[>=1.12.0]")
        self.requires("nlohmann_json/[>=3.11.2]")
        self.requires("magic_enum/[>=0.9.2]")
        if self.settings.os == "Linux":
            self.requires("gtest/[>=1.14.0]")
            self.requires("cppcheck/[>=2.12.1]")

    def layout(self):
        cmake_layout(self, build_folder=os.getcwd())

    def configure(self):
        self.options["paho-mqtt-cpp"].shared = self.options.shared
        self.options["spdlog"].shared = self.options.shared
        self.options["nlohmann_json"].shared = self.options.shared
        self.options["magic_enum"].shared = self.options.shared

    def generate(self):
        tc = CMakeToolchain(self)
        tc.filename = "pzacxx_toolchain.cmake"
        tc.generate()
        deps = CMakeDeps(self)
        deps.generate()

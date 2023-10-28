from conan import ConanFile
from conan.tools.cmake import CMake, CMakeToolchain, CMakeDeps, cmake_layout
import os
import re

class PzaCxx(ConanFile):
    name = "libpza-cxx"
    settings = "os", "compiler", "build_type", "arch"
    options = {
        "shared": [True, False]
    }
    default_options = {
        "shared": True
    }

    def requirements(self):
        self.requires("paho-mqtt-cpp/[>=1.2.0]")
        self.requires("spdlog/[>=1.12.0]")
        self.requires("nlohmann_json/[>=3.11.2]")

    def layout(self):
        self.folders.build_folder_vars = ['settings.os', 'settings.compiler', 'options.shared']
        cmake_layout(self, generator="Ninja Multi-Config")

    def configure(self):
        self.options["paho-mqtt-cpp"].shared = self.options.shared
        self.options["spdlog"].shared = self.options.shared
        self.options["nlohmann_json"].shared = self.options.shared

    def generate(self):
        tc = CMakeToolchain(self)
        tc.generator = "Ninja Multi-Config"
        tc.filename = "pzacxx_toolchain.cmake"
        tc.user_presets_path = 'ConanPresets.json'
        tc.presets_prefix = ""
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

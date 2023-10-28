from conan import ConanFile
from conan.tools.files import save, load
from conan.tools.gnu import AutotoolsToolchain, AutotoolsDeps
from conan.tools.microsoft import unix_path, VCVars, is_msvc
from conan.errors import ConanInvalidConfiguration
from conan.errors import ConanException
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

    def imports(self):
        if self.settings.os == "Windows" and self.options.shared:
            folder = f"{self.build_folder}/bin"
            self.copy("*.dll", dst=folder, src="bin")
            mingw_dlls = ["libgcc_s_seh-1.dll", "libwinpthread-1.dll", "libstdc++-6.dll"]
            mingw_dll_path = "/usr/x86_64-w64-mingw32/bin"
            for dll in mingw_dlls:
                self.copy(dll, dst=folder, src=mingw_dll_path)
        

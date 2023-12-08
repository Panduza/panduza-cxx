conan install -pr:b ./conan_profiles/linux_gcc -pr:h ./conan_profiles/linux_gcc --build=missing -s build_type=Debug -o shared=False .
conan install -pr:b ./conan_profiles/linux_gcc -pr:h ./conan_profiles/linux_gcc --build=missing -s build_type=Release -o shared=False .
conan install -pr:b ./conan_profiles/linux_gcc -pr:h ./conan_profiles/linux_gcc --build=missing -s build_type=Debug -o shared=True .
conan install -pr:b ./conan_profiles/linux_gcc -pr:h ./conan_profiles/linux_gcc --build=missing -s build_type=Release -o shared=True .

conan install -pr:b ./conan_profiles/linux_clang -pr:h ./conan_profiles/linux_clang --build=missing -s build_type=Debug -o shared=False .
conan install -pr:b ./conan_profiles/linux_clang -pr:h ./conan_profiles/linux_clang --build=missing -s build_type=Release -o shared=False .
conan install -pr:b ./conan_profiles/linux_clang -pr:h ./conan_profiles/linux_clang --build=missing -s build_type=Debug -o shared=True .
conan install -pr:b ./conan_profiles/linux_clang -pr:h ./conan_profiles/linux_clang --build=missing -s build_type=Release -o shared=True .

conan install -pr:b ./conan_profiles/linux_gcc -pr:h ./conan_profiles/win_mingw --build=missing -s build_type=Debug -o shared=False .
conan install -pr:b ./conan_profiles/linux_gcc -pr:h ./conan_profiles/win_mingw --build=missing -s build_type=Release -o shared=False .
conan install -pr:b ./conan_profiles/linux_gcc -pr:h ./conan_profiles/win_mingw --build=missing -s build_type=Debug -o shared=True .
conan install -pr:b ./conan_profiles/linux_gcc -pr:h ./conan_profiles/win_mingw --build=missing -s build_type=Release -o shared=True .

cmake --preset windows-gcc-shared
cmake --build --preset windows-gcc-shared-debug
cmake --build --preset windows-gcc-shared-release

cmake --preset linux-gcc-shared
cmake --build --preset linux-gcc-shared-debug
cmake --build --preset linux-gcc-shared-release

cmake --preset linux-clang-shared
cmake --build --preset linux-clang-shared-debug
cmake --build --preset linux-clang-shared-release


cmake --preset windows-gcc-static
cmake --build --preset windows-gcc-static-debug
cmake --build --preset windows-gcc-static-release

cmake --preset linux-gcc-static
cmake --build --preset linux-gcc-static-debug
cmake --build --preset linux-gcc-static-release

cmake --preset linux-clang-static
cmake --build --preset linux-clang-static-debug
cmake --build --preset linux-clang-static-release

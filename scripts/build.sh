#!/bin/bash

BUILD_DIR_LINUX="build"
BUILD_DIR_WINDOWS="buildwin"
BUILD_TYPE="Debug"
BUILD_EXAMPLES="False"
SHARED="True"
TARGET="Linux"
BUILD_DIR=$BUILD_DIR_LINUX

function set_build_dir {
	if [ "$TARGET" = "Windows" ]; then
		BUILD_DIR=$BUILD_DIR_WINDOWS
	fi
	if [ "$SHARED" = "False" ]; then
		BUILD_DIR="$BUILD_DIR-static"
	fi
}

function install_deps {
	echo "Installing dependencies for $TARGET..."
	if [ "$TARGET" = "Windows" ]; then
		EXTRA_CONAN_ARGS="-pr:h ../conan_profiles/x86_64_Cross_Windows"
	else
		EXTRA_CONAN_ARGS="-pr:h ../conan_profiles/x86_64_Linux"
	fi
	mkdir -p $BUILD_DIR
	cd $BUILD_DIR
	conan install .. -o shared=$SHARED -o build_examples=$BUILD_EXAMPLES -s build_type=$BUILD_TYPE --build=missing -pr:b ../conan_profiles/x86_64_Linux $EXTRA_CONAN_ARGS
}

function build {
	echo "Building for $TARGET..."
	cd $BUILD_DIR
	conan build ..
}

function clean {
	echo "Cleaning build directory for $TARGET"
	set_build_dir
	rm -rf $BUILD_DIR
}

function usage {
	echo "Usage: $0 [-t <target>] [-r] [-s] [-d] [-e] [-c] [-h]"
	echo "  -t  Target platform (Windows or Linux). Default is Linux"
	echo "  -r  Build in Release mode. Default is Debug"
	echo "  -s  Build in Static mode. Default is Shared"
	echo "  -d  Install dependencies"
	echo "  -e  Build examples"
	echo "  -c  Clean build directory"
	echo "  -h  Display this help message"
}

# Parse command line arguments
while getopts "t:rscdebh" opt; do
	case $opt in
		t)
			TARGET=$OPTARG
			if [ "$TARGET" != "Windows" ] && [ "$TARGET" != "Linux" ]; then
				echo "Invalid target: $TARGET" >&2
				usage
				exit 1
			fi
			;;
		r)
			BUILD_TYPE="Release"
			;;
		s)
			SHARED="False"
			;;
		c)
			CLEAN="True"
			;;
		d)
			DEPS="True"
			;;
		e)
			BUILD_EXAMPLES="True"
			;;
		h)
			usage
			exit 0
			;;
		\?)
			echo "Invalid option: -$OPTARG" >&2
			usage
			exit 1
			;;
		:)
			echo "Option -$OPTARG requires an argument." >&2
			usage
			exit 1
			;;
	esac
done

set_build_dir

if [ "$CLEAN" = "True" ]; then
	clean
	exit 0
fi

if [ "$DEPS" = "True" ]; then
	install_deps
	exit 0
fi

build
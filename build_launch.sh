#! /bin/bash

# These flags control the build type and process
# DEBUG: Use --debug when we need to run the application itself with gdb to catch crash issues on launch or anything
# CLEAR: Use --clear when we need to clear the build directory before building again
DEBUG=false
CLEAR=false
BUILD_ONLY=false

while [[ $# -gt 0 ]]; do
    case "$1" in 
        --debug)
            DEBUG=true
            shift
            ;;
        --clear)
            CLEAR=true
            shift
            ;;
        --build_only)
            BUILD_ONLY=true
            shift
            ;;
        *)
            echo "Unknown Argument: $1"
            exit 1;
    esac
done

# First thing is to create the directory _build
mkdir -pv _build


# Clear only if we want a clean build
if [ "$CLEAR" == true ]; then
    rm -rf _build/* -v
fi

if [ "$BUILD_ONLY" == true ]; then
    cmake -S . -B _build -DCMAKE_BUILD_TYPE=debug -DCMAKE_VERBOSE_MAKEFILE=ON && make -C _build
    exit 0
fi

# Run cmake on the source dir for building in the _build directory
# cmake -S . -B _build -DCMAKE_VERBOSE_MAKEFILE=OFF
# Once the Cmake command is successful
# Run Make on the build dir
# And if the make is successful, we will have the executable ready to be launched

# Using the base build type as debug to get logging to work
# Release build does not include logging
if [[ "$DEBUG" == true ]]; then
    echo -e "\n\n\nRunning with GDB\n\n\n"
    cmake -S . -B _build -DCMAKE_BUILD_TYPE=debug -DCMAKE_VERBOSE_MAKEFILE=ON && make -C _build && gdb -ex run --args ./_build/src/VOID
else
    cmake -S . -B _build -DCMAKE_BUILD_TYPE=debug -DCMAKE_VERBOSE_MAKEFILE=ON && make -C _build && ./_build/src/VOID
fi


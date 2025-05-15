#! /bin/bash

# First thing is to create and clear the directory _build
mkdir -p _build -v

# Clear
rm -rf _build/* -v

# Run cmake on the source dir for building in the _build directory
# cmake -S . -B _build -DCMAKE_VERBOSE_MAKEFILE=OFF
# Once the Cmake command is successful
# Run Make on the build dir
# And if the make is successful, we will have the executable ready to be launched
if [[ " $@ " =~ " --debug " ]]; then
    echo -e "\n\n\nRunning with GDB\n\n\n"
    cmake -S . -B _build -DCMAKE_BUILD_TYPE=debug -DCMAKE_VERBOSE_MAKEFILE=ON && make -C _build && gdb -ex run --args ./_build/src/VOID
else
    cmake -S . -B _build -DCMAKE_BUILD_TYPE=debug -DCMAKE_VERBOSE_MAKEFILE=ON && make -C _build && ./_build/src/VOID
fi


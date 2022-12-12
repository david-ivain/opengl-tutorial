#!/bin/sh

cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE=./dependencies/vcpkg/scripts/buildsystems/vcpkg.cmake -DCMAKE_EXPORT_COMPILE_COMMANDS:BOOL=TRUE -DCMAKE_BUILD_TYPE:STRING=MinSizeRel

#!/bin/sh

set -e
dir=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd) #"

mkdir -p "$dir/Build/Linux32Debug"
cd "$dir/Build/Linux32Debug"

CC="gcc -m32 -L/usr/lib/i386-linux-gnu -I/usr/lib/i386-linux-gnu/glib-2.0/include" \
CXX="g++ -m32 -L/usr/lib/i386-linux-gnu -I/usr/lib/i386-linux-gnu/glib-2.0/include" \
cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_PREFIX_PATH=/usr -DCMAKE_LIBRARY_ARCHITECTURE=i386-linux-gnu "$dir"

cmake --build . --parallel 4

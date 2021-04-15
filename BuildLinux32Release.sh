#!/bin/sh

set -e
dir=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd) #"

mkdir -p "$dir/Build/Linux32Release"
cd "$dir/Build/Linux32Release"

CC="gcc -m32 -L/usr/lib/i386-linux-gnu -I/usr/lib/i386-linux-gnu/glib-2.0/include" \
CXX="g++ -m32 -L/usr/lib/i386-linux-gnu -I/usr/lib/i386-linux-gnu/glib-2.0/include" \
cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH=/usr -DCMAKE_LIBRARY_ARCHITECTURE=i386-linux-gnu "$dir"

cmake --build . --parallel 4

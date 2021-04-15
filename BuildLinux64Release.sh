#!/bin/sh

set -e
dir=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd) #"

mkdir -p "$dir/Build/Linux64Release"
cd "$dir/Build/Linux64Release"

CC="gcc -m64 -L/usr/lib/x86_64-linux-gnu -I/usr/lib/x86_64-linux-gnu/glib-2.0/include" \
CXX="g++ -m64 -L/usr/lib/x86_64-linux-gnu -I/usr/lib/x86_64-linux-gnu/glib-2.0/include" \
cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH=/usr -DCMAKE_LIBRARY_ARCHITECTURE=x86_64-linux-gnu "$dir"

cmake --build . --parallel 4

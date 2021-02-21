#!/bin/sh

set -e
dir=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd) #"

mkdir -p "$dir/Build/LinuxDebug"
cd "$dir/Build/LinuxDebug"
cmake -DCMAKE_BUILD_TYPE=Debug "$dir"
cmake --build . --parallel 4

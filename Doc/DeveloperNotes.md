
Developer Notes
===============

Some useful notes in case you want to fiddle with the code.

Memory allocation in the compiler
---------------------------------

Compiler uses garbage-collected memory allocations. Any object that inherits from `GCObject` should be allocated
by passing instance of `GCHeap` to the `new` operator. Its lifetime will be tied to the corresponding heap, i.e.
it will be automatically destroyed when the heap is destroyed.

This method of allocation is extremely fast, but programmer should be careful to not use pointers to such objects
after heap is destroyed.

Also, beware that, by default, destructors for such objects are not called. If you need destructor to be called,
call `registerFinalizer()` method from the constructor (it can safely be called multiple times, e.g. from
constructors of both child and parent classes).

Required packages for Linux build
---------------------------------

```
apt-get install \
    build-essential \
    cmake \
    qtmultimedia5-dev \
    qt5-default \
    openjdk-15-dbg \
    openjdk-15-jdk \
    libpulse-dev \
    libxcursor-dev \
    libxrender-dev \
    libxfixes-dev \
    libxinerama-dev \
    libxi-dev \
    libxrandr-dev \
    libxss-dev \
    libxxf86vm-dev \
    libudev-dev \
    libdbus-1-dev \
    libibus-1.0-dev \
    libasound2-dev \
    libsndio-dev \
    libsamplerate0-dev \
    libaudio-dev \
    libdrm-dev \
    libgbm-dev \
    libegl-dev \
    libxkbcommon-dev \
    libwayland-dev \
    wayland-protocols
```

For cross-compilation:

```
dpkg --add-architecture i386
apt-get install \
    gcc-multilib \
    g++-multilib \
    qtmultimedia5-dev:i386 \
    qt5-default:i386 \
    mingw-w64-common \
    mingw-w64-i686-dev \
    mingw-w64-x86-64-dev \
    binutils-mingw-w64-i686 \
    binutils-mingw-w64-x86-64 \
    gcc-mingw-w64-i686 \
    gcc-mingw-w64-x86-64 \
    libglib2.0-dev:i386 \
    libpulse-dev:i386 \
    libxcursor-dev:i386 \
    libxrender-dev:i386 \
    libxfixes-dev:i386 \
    libxinerama-dev:i386 \
    libxi-dev:i386 \
    libxrandr-dev:i386 \
    libxss-dev:i386 \
    libxxf86vm-dev:i386 \
    libudev-dev:i386 \
    libdbus-1-dev:i386 \
    libibus-1.0-dev:i386 \
    libasound2-dev:i386 \
    libsndio-dev:i386 \
    libsamplerate0-dev:i386 \
    libaudio-dev:i386 \
    libdrm-dev:i386 \
    libgbm-dev:i386 \
    libegl-dev:i386 \
    libxkbcommon-dev:i386 \
    libwayland-dev:i386 \
    wayland-protocols:i386
```

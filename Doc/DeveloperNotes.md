
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
    qt5-default
```

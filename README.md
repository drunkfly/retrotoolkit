Retro Toolkit
=============

**NOTE: this software is currently being developed and have not reached even alpha quality. Expect bugs
and make frequent backups of your projects! If you find any problems, please report them through the
GitHub issue system.**

Compiling
---------

You will need to install the following dependencies:

* C++ Compiler with C++17 support (tested on MSVC 2019 and GCC 10.2.0)
* CMake 3.20 or newer.
* Qt 5.15. Older 5.x versions may work as well, but are not tested
* Java Development Kit 1.5 or newer.

Known Issues
------------

* Compressed data does not contain proper source location information.

* Generation of output files in `_out/files` can't be disabled

Legal
-----

Compiler and IDE
================

License for the compiler and IDE is GNU GPL v2 or any later version.

    Copyright (c) 2021 Drunk Fly

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

This license does not cover the code you write. Any programs you would create with the toolkit belong to
you and can be distributed under your own terms.

Runtimes
========

Runtimes for various platforms (located in directory `Sources/Runtimes`) are licensed under the MIT license.

    Copyright (c) 2021 Drunk Fly

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in all
    copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
    SOFTWARE.

Third Party Software
====================

This software also includes a bunch of third party software. Please consult licensing for this software in the
corresponding directories inside `Libs`.

This repository includes some of the "Farm-Fresh Web Icons" by FatCow (http://www.fatcow.com/free-icons).
These icon sets are licensed under a Creative Commons Attribution 3.0 License. The icons may not be resold,
sub-licensed, rented, transferred or otherwise made available for use. Please link to the page on
fatcow.com if you would like to spread the word.

This repository includes some of the "Fugue Icons" by Yusuke Kamiyamane (http://p.yusukekamiyamane.com/).
These icons are licensed under a Creative Commons Attribution 3.0 License.

Apache Ant, included into source distribution, is licensed under the Apache 2 license, which is incompatible with
GNU GPL v2. Apache Ant is only used during the compilation process as an external tool and is not included into
the final distribution.

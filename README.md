Retro Toolkit
=============

**NOTE: this software is currently being developed and have not reached even alpha quality. Expect bugs
and make frequent backups of your projects! If you find any problems, please report them through the
GitHub issue system.**

Compiling
---------

You will need to install the following dependencies:

* CMake 3.16 or newer.
* Qt 5.15. Older 5.x versions may work as well, but are not tested
* Java Development Kit 1.5 or newer.

Known Issues
------------

* Compressed data does not contain proper source location information.

* Generation of output files in `_out/files` can't be disabled

License
-------

License is GNU GPL v2 or any later version.

    Copyright (c) 2019 Nikolay Zapolnov

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

Please note that the license covers only the toolkit itself. Any programs you would create with the toolkit belong to
you and can be distributed under your own terms.

This software also includes a bunch of third party software. Please consult licensing for this software in the
corresponding directories inside `Libs`.

Note that this repository includes "Farm-Fresh Web Icons" by FatCow (http://www.fatcow.com/free-icons).
These icon sets are licensed under a Creative Commons Attribution 3.0 License. The icons may not be resold,
sub-licensed, rented, transferred or otherwise made available for use. Please link to the page on
fatcow.com if you would like to spread the word.

Note that Apache Ant included into source distribution uses the Apache 2 license which is incompatible with
GNU GPL v2. Apache Ant is only used during the compilation process as an external tool and is not included into
the final distribution.

/* ide.c: constants etc used for IDE emulation
   Copyright (c) 2004-2005 Philip Kendall

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

   Author contact information:

   Philip Kendall <philip-fuse@shadowmagic.org.uk>

*/

#include "ide.h"

const char *HDF_SIGNATURE = "RS-IDE\x1a";
const size_t HDF_SIGNATURE_LENGTH = 7;
const size_t HDF_DATA_OFFSET = 0x80;

char
hdf_version( enum hdf_version_t version )
{
  switch( version ) {
  case HDF_VERSION_10: return '\x10';
  case HDF_VERSION_11: return '\x11';
  }

  return 0;
}

size_t
hdf_data_offset( enum hdf_version_t version )
{
  switch( version ) {
  case HDF_VERSION_10: return 0x0080;
  case HDF_VERSION_11: return 0x0216;
  }

  return 0;
}

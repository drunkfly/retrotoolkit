/* ide.h: constants etc used for IDE emulation
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

#ifndef FUSE_UTILS_IDE_H
#define FUSE_UTILS_IDE_H

#include <stdlib.h>

extern const char *HDF_SIGNATURE;
extern const size_t HDF_SIGNATURE_LENGTH;

enum hdf_version_t {

  HDF_VERSION_10,
  HDF_VERSION_11,

};

enum {

  HDF_SIGNATURE_OFFSET   = 0x00,
  HDF_VERSION_OFFSET     = 0x07,
  HDF_COMPACT_OFFSET     = 0x08,
  HDF_DATA_OFFSET_OFFSET = 0x09,
  HDF_IDENTITY_OFFSET    = 0x16,

};

enum {

  IDENTITY_CYLINDERS_OFFSET    = 2,
  IDENTITY_HEADS_OFFSET        = 6,
  IDENTITY_SECTORS_OFFSET      = 12,
  IDENTITY_MODEL_NUMBER_OFFSET = 54,
  IDENTITY_CAPABILITIES_OFFSET = 98,

};

char hdf_version( enum hdf_version_t version );
size_t hdf_data_offset( enum hdf_version_t version );

#endif				/* #ifndef FUSE_UTILS_IDE_H */

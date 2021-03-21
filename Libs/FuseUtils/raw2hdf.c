/* raw2hdf.c: Create an .hdf file from a raw binary image
   Copyright (c) 2005 Matthew Westcott, 2006 Philip Kendall
   Copyright (c) 2014-2015 Sergio Baldovi

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

#include <config.h>

#include <errno.h>
#include <getopt.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "ide.h"

#define PROGRAM_NAME "raw2hdf"

const char *progname;

static const char *MODEL_NUMBER = "rCaeet dybr wah2fd                      ";
static const size_t MODEL_NUMBER_LENGTH = 40;

#define CHUNK_LENGTH 1 << 20

static void
show_version( void )
{
  printf(
    PROGRAM_NAME " (" PACKAGE ") " PACKAGE_VERSION "\n"
    "Copyright (c) 2005 Matthew Westcott\n"
    "Copyright (c) 2006 Philip Kendall\n"
    "License GPLv2+: GNU GPL version 2 or later "
    "<http://gnu.org/licenses/gpl.html>\n"
    "This is free software: you are free to change and redistribute it.\n"
    "There is NO WARRANTY, to the extent permitted by law.\n" );
}

static void
show_help( void )
{
  printf(
    "Usage: %s [OPTION] <rawfile> <hdffile>\n"
    "Converts a binary dump of a hard disk's data into an IDE disk image in .hdf \n"
    "format.\n"
    "\n"
    "Options:\n"
    "  -v <version>   Specifies the version of .hdf image to be created (values\n"
    "                   1.0 or 1.1). Defaults to creating version 1.1 files.\n"
    "  -h, --help     Display this help and exit.\n"
    "  -V, --version  Output version information and exit.\n"
    "\n"
    "Report %s bugs to <%s>\n"
    "%s home page: <%s>\n"
    "For complete documentation, see the manual page of %s.\n",
    progname,
    PROGRAM_NAME, PACKAGE_BUGREPORT, PACKAGE_NAME, PACKAGE_URL, PROGRAM_NAME
  );
}

static int
parse_options( int argc, char **argv, const char **raw_filename,
	       const char **hdf_filename, enum hdf_version_t *version )
{
  int c;
  int error = 0;

  struct option long_options[] = {
    { "help", 0, NULL, 'h' },
    { "version", 0, NULL, 'V' },
    { 0, 0, 0, 0 }
  };

  while( ( c = getopt_long( argc, argv, "v:hV", long_options, NULL ) ) != -1 ) {

    switch( c ) {

    case 'v':
      if( strcasecmp( optarg, "1.0" ) == 0 ) {
	*version = HDF_VERSION_10;
      } else if( strcasecmp( optarg, "1.1" ) == 0 ) {
	*version = HDF_VERSION_11;
      } else {
        error = 1;
        fprintf( stderr, "%s: version not supported\n", progname );
      }
      break;

    case 'h':
      show_help();
      exit( 0 );

    case 'V':
      show_version();
      exit( 0 );

    case '?':
      /* getopt prints an error message to stderr */
      error = 1;
      break;

    default:
      error = 1;
      fprintf( stderr, "%s: unknown option `%c'\n", progname, (char) c );
      break;

    }
  }
  argc -= optind;
  argv += optind;

  if( error ) return error;

  if( argc != 2 ) {
    fprintf( stderr, "%s: usage: %s [-v<version>] <raw-filename> <hdf-filename>\n",
             progname, progname );
    return 1;
  }

  *raw_filename  = argv[0];
  *hdf_filename  = argv[1];

  return 0;
}

static int
write_header( FILE *f, size_t byte_count, const char *filename,
	      enum hdf_version_t version )
{

  char *hdf_header, *identity;
  size_t bytes, total_sectors, head_count, cyl_count, sector_count;
  size_t sectors_per_head, data_offset;

  data_offset = hdf_data_offset( version );

  hdf_header = calloc( 1, data_offset );
  if( !hdf_header ) {
    fprintf( stderr, "%s: out of memory at line %d\n", progname, __LINE__ );
    return 1;
  }

  memcpy( &hdf_header[ HDF_SIGNATURE_OFFSET ], HDF_SIGNATURE,
          HDF_SIGNATURE_LENGTH );
  hdf_header[ HDF_VERSION_OFFSET ] = hdf_version( version );
  hdf_header[ HDF_COMPACT_OFFSET ] = 0;
  hdf_header[ HDF_DATA_OFFSET_OFFSET     ] = ( data_offset      ) & 0xff;
  hdf_header[ HDF_DATA_OFFSET_OFFSET + 1 ] = ( data_offset >> 8 ) & 0xff;

  identity = &hdf_header[ HDF_IDENTITY_OFFSET ];

  /* invent suitable geometry to match the file length */
  if( byte_count & 0x1f ) {
    fprintf( stderr,
	     "%s: warning: image is not a whole number of sectors in length\n",
	     progname );
  }
  total_sectors = byte_count >> 9;
  if( total_sectors >= 16514064 ) {
    /* image > 8GB; use dummy 'large disk' CHS values */
    cyl_count = 16383;
    head_count = 16;
    sector_count = 63;
  } else {
    /* find largest factor <= 16 to use as the head count */
    for( head_count = 16; head_count > 1; head_count-- ) {
      if( total_sectors % head_count == 0 ) break;
    }
    sectors_per_head = total_sectors / head_count;
    /* find largest factor <= 63 to use as the sector count */
    for( sector_count = 63; sector_count > 1; sector_count-- ) {
      if( sectors_per_head % sector_count == 0 ) break;
    }
    cyl_count = sectors_per_head / sector_count;
    if( cyl_count > 16384 ) {
      /* attempt to factorise into sensible CHS values failed dismally;
         fall back on dummy 'large disk' values */
      cyl_count = 16383;
      head_count = 16;
      sector_count = 63;
    }
  }

  identity[ IDENTITY_CYLINDERS_OFFSET     ] = ( cyl_count      ) & 0xff;
  identity[ IDENTITY_CYLINDERS_OFFSET + 1 ] = ( cyl_count >> 8 ) & 0xff;

  identity[ IDENTITY_HEADS_OFFSET     ] = ( head_count      ) & 0xff;
  identity[ IDENTITY_HEADS_OFFSET + 1 ] = ( head_count >> 8 ) & 0xff;

  identity[ IDENTITY_SECTORS_OFFSET     ] = ( sector_count      ) & 0xff;
  identity[ IDENTITY_SECTORS_OFFSET + 1 ] = ( sector_count >> 8 ) & 0xff;

  /* set 'LBA supported' flag */
  identity[ IDENTITY_CAPABILITIES_OFFSET + 1 ] = 0x02;

  memcpy( &identity[ IDENTITY_MODEL_NUMBER_OFFSET ], MODEL_NUMBER,
          MODEL_NUMBER_LENGTH );  

  rewind( f );
  bytes = fwrite( hdf_header, 1, data_offset, f );
  if( bytes != data_offset ) {
    fprintf( stderr,
             "%s: could write only %lu header bytes out of %lu to '%s'\n",
             progname, (unsigned long)bytes, (unsigned long)data_offset,
             filename );
    free( hdf_header );
    return 1;
  }

  free( hdf_header );

  return 0;
}

static int
copy_data( FILE *from, FILE *to, size_t *byte_count, const char *from_filename,
	   const char *to_filename, enum hdf_version_t version )
{
  char buffer[ CHUNK_LENGTH ];
  size_t bytes_read;

  if( fseek( to, hdf_data_offset( version ), SEEK_SET ) ) {
    fprintf( stderr, "%s: error seeking in '%s': %s\n", progname,
             to_filename, strerror( errno ) );
    return 1;
  }
  
  *byte_count = 0;
  
  while( !feof( from ) ) {
    bytes_read = fread( buffer, 1, CHUNK_LENGTH, from );
    if( ferror( from ) ) {
      fprintf( stderr, "%s: error reading from '%s': %s\n", progname,
               from_filename, strerror( errno ) );
      return 1;
    }
    *byte_count += bytes_read;
    
    fwrite( buffer, 1, bytes_read, to );
    if( ferror( to ) ) {
      fprintf( stderr, "%s: error writing to '%s': %s\n", progname,
               to_filename, strerror( errno ) );
      return 1;
    }
  }

  return 0;
}

int
main( int argc, char **argv )
{
  const char *raw_filename, *hdf_filename;
  FILE *raw, *hdf;
  int error;
  size_t byte_count;
  enum hdf_version_t version = HDF_VERSION_11;

  progname = argv[0];

  error = parse_options( argc, argv, &raw_filename, &hdf_filename, &version );
  if( error ) {
    fprintf( stderr, "Try `%s --help' for more information.\n", progname );
    return error;
  }

  raw = fopen( raw_filename, "rb" );
  if( !raw ) {
    fprintf( stderr, "%s: error opening '%s': %s\n", progname, raw_filename,
	     strerror( errno ) );
    return 1;
  }

  hdf = fopen( hdf_filename, "wb" );
  if( !hdf ) {
    fprintf( stderr, "%s: error opening '%s': %s\n", progname, hdf_filename,
	     strerror( errno ) );
    fclose( raw );
    return 1;
  }

  error = copy_data( raw, hdf, &byte_count, raw_filename, hdf_filename,
		     version );
  if( error ) {
    fclose( hdf );
    fclose( raw );
    return error;
  }

  fclose( raw );

  error = write_header( hdf, byte_count, hdf_filename, version );
  if( error ) {
    fclose( hdf );
    return error;
  }

  if( fclose( hdf ) ) {
    fprintf( stderr, "%s: error closing `%s': %s\n", progname, hdf_filename,
             strerror( errno ) );
    return 1;
  }

  return 0;
}

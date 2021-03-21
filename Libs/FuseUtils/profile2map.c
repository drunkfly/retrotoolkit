/* profile2map.c: convert Fuse profiler output into the Z80 map format
   Copyright (c) 2007 Stuart Brady
   Copyright (c) 2015 Sergio Baldovi

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

   Philip: philip-fuse@shadowmagic.org.uk

*/

#include <config.h>

#include <errno.h>
#include <getopt.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <libspectrum.h>

#include "utils.h"

#define PROGRAM_NAME "profile2map"

/* argv[0] */
char *progname;

libspectrum_byte map[ 0x10000 / 8 ];

static void
show_version( void )
{
  printf(
    PROGRAM_NAME " (" PACKAGE ") " PACKAGE_VERSION "\n"
    "Copyright (c) 2007 Stuart Brady\n"
    "License GPLv2+: GNU GPL version 2 or later "
    "<http://gnu.org/licenses/gpl.html>\n"
    "This is free software: you are free to change and redistribute it.\n"
    "There is NO WARRANTY, to the extent permitted by law.\n" );
}

static void
show_help( void )
{
  printf(
    "Usage: %s [OPTION] <profile> <map>\n"
    "Converts Fuse profiler output into Z80-style map format.\n"
    "\n"
    "Options:\n"
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

int main( int argc, char **argv )
{
  char *profile, *mapfile;
  FILE *f;
  long address, count;
  int ret;

  int c;
  int error = 0;

  struct option long_options[] = {
    { "help", 0, NULL, 'h' },
    { "version", 0, NULL, 'V' },
    { 0, 0, 0, 0 }
  };

  progname = argv[0];

  while( ( c = getopt_long( argc, argv, "hV", long_options, NULL ) ) != -1 ) {

    switch( c ) {

    case 'h': show_help(); return 0;

    case 'V': show_version(); return 0;

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

  if( error ) {
    fprintf( stderr, "Try `%s --help' for more information.\n", progname );
    return error;
  }

  if( argc != 2 ) {
    fprintf( stderr, "%s: usage: %s <profile> <map>\n", progname, progname );
    fprintf( stderr, "Try `%s --help' for more information.\n", progname );
    return 1;
  }

  profile = argv[0];
  mapfile = argv[1];

  f = fopen( profile, "r" );
  if( !f ) {
    fprintf( stderr, "%s: unable to open profile map '%s' for reading\n",
	     progname, profile );
    return 1;
  }

  memset( map, 0, sizeof( map ) );

  while( 1 ) {
    ret = fscanf( f, "%li,%li\n", &address, &count );
    if( ret == EOF ) break;

    if( count && address >= 0 && address < 0x10000 )
      map[ address / 8 ] |= ( 1 << ( address % 8 ) );
  }

  fclose( f );

  error = write_file( mapfile, map, sizeof( map ) );
  if( error ) { return error; }

  return 0;
}

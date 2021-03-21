/* rzxtool.c: Simple modifications to RZX files
   Copyright (c) 2007-2015 Philip Kendall, Sergio Baldovi

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

   E-mail: philip-fuse@shadowmagic.org.uk

*/

#include <config.h>

#include <errno.h>
#include <getopt.h>
#include <unistd.h>
#include <string.h>

#ifdef HAVE_LIB_GLIB
#include <glib.h>
#endif				/* #ifdef HAVE_LIB_GLIB */
#include <libspectrum.h>

#include "utils.h"

#define PROGRAM_NAME "rzxtool"

const char *progname;
static libspectrum_creator *creator;

typedef enum action_type_t {

  ACTION_DELETE_BLOCK,
  ACTION_EXTRACT_SNAP,
  ACTION_INSERT_SNAP,
  ACTION_FINALISE_RZX,

} action_type_t;

typedef struct action_t {

  action_type_t type;
  size_t where;
  char *filename;

} action_t;

typedef struct options_t {

  const char *rzxfile;
  const char *outfile;
  int uncompressed;

} options_t;

static libspectrum_rzx_iterator
get_block( libspectrum_rzx *rzx, size_t where )
{
  libspectrum_rzx_iterator it;

  for( it = libspectrum_rzx_iterator_begin( rzx );
       it && where;
       where--, it = libspectrum_rzx_iterator_next( it ) )
    ;	/* Do nothing */

  return it;
}

static int
delete_block( libspectrum_rzx *rzx, size_t where )
{
  libspectrum_rzx_iterator it;

  it = get_block( rzx, where );
  if( !it ) {
    fprintf( stderr, "%s: not enough blocks in RZX file\n", progname );
    return 1;
  }

  libspectrum_rzx_iterator_delete( rzx, it );

  return 0;
}

static int
write_snapshot( libspectrum_snap *snap, const char *filename )
{
  unsigned char *buffer = NULL; size_t length = 0;
  int error, flags;
  libspectrum_id_t type;
  libspectrum_class_t class;

  error = libspectrum_identify_file_with_class( &type, &class, filename, NULL,
						0 );
  if( error ) return error;

  if( class != LIBSPECTRUM_CLASS_SNAPSHOT || type == LIBSPECTRUM_ID_UNKNOWN )
    type = LIBSPECTRUM_ID_SNAPSHOT_SZX;

  error = libspectrum_snap_write( &buffer, &length, &flags, snap, type,
				  creator, 0 );
  if( error ) return error;

  error = write_file( filename, buffer, length );
  if( error ) { free( buffer ); return error; }

  free( buffer );

  return 0;
}

static int
extract_snap( libspectrum_rzx *rzx, size_t where, const char *filename )
{
  libspectrum_rzx_iterator it;
  libspectrum_snap *snap;
  int e;

  it = get_block( rzx, where );
  if( !it ) {
    fprintf( stderr, "%s: not enough blocks in RZX file\n", progname );
    return 1;
  }

  snap = libspectrum_rzx_iterator_get_snap( it );
  if( !snap ) {
    fprintf( stderr, "%s: not a snapshot block\n", progname );
    return 1;
  }

  e = write_snapshot( snap, filename );
  if( e ) return e;
  
  return 0;
}

static libspectrum_snap*
read_snap( const char *filename )
{
  unsigned char *buffer = NULL;
  size_t length = 0;
  int error;
  libspectrum_snap *snap;

  if( read_file( filename, &buffer, &length ) ) {
    return NULL;
  }

  snap = libspectrum_snap_alloc();

  error = libspectrum_snap_read( snap, buffer, length, LIBSPECTRUM_ID_UNKNOWN,
                                 filename );

  if( error ) {
    libspectrum_snap_free( snap );
    snap = NULL;
  }

  free( buffer );

  return snap;
}

static int
insert_snap( libspectrum_rzx *rzx, size_t where, const char *filename )
{
  libspectrum_snap *snap;

  snap = read_snap( filename );
  if( !snap ) {
    fprintf( stderr, "%s: couldn't read `%s'\n", progname, filename );
    return 1;
  }

  libspectrum_rzx_insert_snap( rzx, snap, where );

  return 0;
}

static void
apply_action( void *data, void *user_data )
{
  const action_t *action = data;
  libspectrum_rzx *rzx = user_data;
  int where = action->where;

  switch( action->type ) {
  case ACTION_DELETE_BLOCK:
    delete_block( rzx, where );
    break;
  case ACTION_EXTRACT_SNAP:
    extract_snap( rzx, where, action->filename );
    break;
  case ACTION_INSERT_SNAP:
    insert_snap( rzx, where, action->filename );
    break;
  case ACTION_FINALISE_RZX:
    libspectrum_rzx_finalise( rzx );
    break;
  default:
    fprintf( stderr, "%s: unknown action type %d\n", progname, action->type );
  }

  /* Really would like to handle errors */
}

static int
parse_argument( const char *argument, int *where, const char **filename )
{
  char *comma;
  char *buffer;

  buffer = strdup( argument );
  if( !buffer ) {
    fprintf( stderr, "%s: out of memory at %s:%d\n", progname,
	     __func__, __LINE__ );
    return 1;
  }
  
  comma = strchr( buffer, ',' );
  if( !comma ) {
    fprintf( stderr, "%s: no comma found in argument `%s'\n", progname,
	     argument );
    free( buffer );
    return 1;
  }

  *comma = 0;

  *where = atoi( buffer );

  *filename = &argument[ comma + 1 - buffer ];

  free( buffer );

  return 0;
}

static int
add_action( GSList **actions, action_type_t type, const char *argument )
{
  int error, where;
  const char *filename;
  action_t *action;

  action = malloc( sizeof( *action ) );
  if( !action ) {
    fprintf( stderr, "%s: out of memory at %s:%d\n", progname,
	     __func__, __LINE__ );
    return 1;
  }

  action->type = type;

  if( type == ACTION_DELETE_BLOCK ) {
    action->where = atoi( argument );
    action->filename = NULL;
  } else if( type == ACTION_FINALISE_RZX ) {
    action->where = 0;
    action->filename = NULL;
  } else {

    error = parse_argument( argument, &where, &filename );
    if( error ) return error;

    action->where = where;
    action->filename = strdup( filename );
    if( !action->filename ) {
      fprintf( stderr, "%s: out of memory at %s:%d\n", progname,
	       __func__, __LINE__ );
      return 1;
    }

  }

  *actions = g_slist_append( *actions, action );

  return 0;
}

static void
show_version( void )
{
  printf(
    PROGRAM_NAME " (" PACKAGE ") " PACKAGE_VERSION "\n"
    "Copyright (c) 2007-2014 Philip Kendall\n"
    "License GPLv2+: GNU GPL version 2 or later "
    "<http://gnu.org/licenses/gpl.html>\n"
    "This is free software: you are free to change and redistribute it.\n"
    "There is NO WARRANTY, to the extent permitted by law.\n" );
}

static void
show_help( void )
{
  printf(
    "Usage: %s [OPTION]... <rzxfile> [<outfile>]\n"
    "Modify Sinclair ZX Spectrum input recording files.\n"
    "\n"
    "Options:\n"
    "  -d <block>     Delete block number.\n"
    "  -e <block>,<filename>\n"
    "                 The snapshot in the specified block number will be written\n"
    "                   to the specified filename.\n"
    "  -i <block>,<filename>\n"
    "                 The snapshot specified in filename will be added to the file\n"
    "                   at the position specified by block number.\n"
    "  -f             Finalise the RZX file, removing any interspersed snapshot.\n"
    "  -u             Write uncompressed data to the output RZX file.\n"
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
parse_options( int argc, char **argv, GSList **actions,
	       struct options_t *options )
{
  int c, error = 0;
  int output_needed = 0;

  options->uncompressed = 0;

  struct option long_options[] = {
    { "help", 0, NULL, 'h' },
    { "version", 0, NULL, 'V' },
    { 0, 0, 0, 0 }
  };

  while( ( c = getopt_long( argc, argv, "d:e:i:ufhV", long_options,
                            NULL ) ) != -1 ) {
    switch( c ) {
    case 'd':
      error = add_action( actions, ACTION_DELETE_BLOCK, optarg );
      output_needed = 1;
      break;
    case 'e':
      error = add_action( actions, ACTION_EXTRACT_SNAP, optarg );
      break;
    case 'i':
      error = add_action( actions, ACTION_INSERT_SNAP, optarg );
      output_needed = 1;
      break;
    case 'f':
      error = add_action( actions, ACTION_FINALISE_RZX, optarg );
      output_needed = 1;
      break;
    case 'u':
      options->uncompressed = 1;
      output_needed = 1;
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

    if( error ) break;
  }

  if( error ) return error;

  if( !argv[ optind ] ) {
    fprintf( stderr, "%s: no RZX file specified\n", progname );
    return 1;
  }

  options->rzxfile = argv[ optind++ ];

  if( output_needed ) {

    if( !argv[ optind ] ) {
      fprintf( stderr, "%s: no RZX output file specified\n", progname );
      return 1;
    }

    options->outfile = argv[ optind++ ];
  } else {
    options->outfile = NULL;
  }

  if( argv[ optind ] ) {
    fprintf( stderr, "%s: extra argument on command line\n", progname );
    return 1;
  }

  return 0;
}

static int
write_rzx( const char *filename, libspectrum_rzx *rzx, int compressed )
{
  unsigned char *buffer = NULL; size_t length = 0;
  int error;

  error = libspectrum_rzx_write( &buffer, &length, rzx, LIBSPECTRUM_ID_UNKNOWN,
				 creator, compressed, NULL );
  if( error ) return error;

  error = write_file( filename, buffer, length );
  if( error ) { free( buffer ); return error; }

  free( buffer );

  return 0;
}  

int
main( int argc, char *argv[] )
{
  GSList *actions = NULL;
  options_t options;
  unsigned char *buffer = NULL; size_t length = 0;
  libspectrum_rzx *rzx;
  int error;

  progname = argv[0];

  error = init_libspectrum(); if( error ) return error;

  error = get_creator( &creator, "rzxtool" ); if( error ) return error;

  error = parse_options( argc, argv, &actions, &options );
  if( error ) {
    fprintf( stderr, "Try `%s --help' for more information.\n", progname );
    return error;
  }

  error = read_file( options.rzxfile, &buffer, &length );
  if( error ) return error;

  rzx = libspectrum_rzx_alloc();

  error = libspectrum_rzx_read( rzx, buffer, length );
  if( error ) return error;

  free( buffer );

  g_slist_foreach( actions, apply_action, rzx );

  if( options.outfile ) {
    error = write_rzx( options.outfile, rzx, !options.uncompressed );
    if( error ) return error;
  }

  return 0;
}

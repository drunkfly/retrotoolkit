/* fmfconv_compr.c: Convert .fmf movie files - zlib compression routines
   Copyright (c) 2004-2015 Gergely Szasz
   Copyright (c) 2014-2016 Sergio Baldovi

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

   E-mail: szaszg@hu.inter.net
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "fmfconv_compr.h"

#ifdef HAVE_ZLIB_H
#include <zlib.h>

#define ZBUF_SIZE 1024
#define ZBUF_INP_SIZE 512
z_stream zstream;			/* zlib struct */
unsigned char zbuf_o[ZBUF_SIZE];	/* zlib output buffer */
unsigned char zbuf_i[ZBUF_INP_SIZE];	/* zlib input buffer */
int fmf_compr_feof = 0;
#endif

/* fmf variables */
int fmf_compr = 0;			/* fmf compressed or not */

#ifdef HAVE_ZLIB_H
void
fmf_compr_init( void )
{
  zstream.zalloc = Z_NULL;
  zstream.zfree = Z_NULL;
  zstream.opaque = Z_NULL;
  zstream.avail_in = 0;
  zstream.next_in = Z_NULL;
  inflateInit( &zstream );
  zstream.avail_out = ZBUF_SIZE;
}

void
fmf_compr_end( void )
{
  inflateEnd( &zstream );
}

int
feof_compr( FILE *f )
{
  if( fmf_compr ) {
    if( fmf_compr_feof )
      return 1;
    if( zstream.avail_in > 0 || zstream.avail_out != ZBUF_SIZE )
      return 0;
    fmf_compr_feof = feof( f );
    return fmf_compr_feof;
  } else {
    return feof( f );
  }
}

int
fgetc_compr( FILE *f )
{
  int s;

  if( fmf_compr ) {
    if( zstream.avail_out == ZBUF_SIZE ) {
      zstream.next_out = zbuf_o;
      do {
	if( zstream.avail_in == 0 && !fmf_compr_feof ) {
	  zstream.avail_in = fread( zbuf_i, 1, ZBUF_INP_SIZE, f );
	  zstream.next_in = zbuf_i;
	  if( zstream.avail_in == 0 )
	    fmf_compr_feof = 1;
	}
	s = inflate( &zstream, fmf_compr_feof ? Z_FINISH : Z_SYNC_FLUSH );
      } while ( zstream.avail_out != 0 && s == Z_OK );
      zstream.next_out = zbuf_o;
    }
    if( zstream.avail_out == ZBUF_SIZE )	/* end of file */
      return -1;
    zstream.avail_out++;
    return *( zstream.next_out++ );    /* !!! we use it for own purpose */
  } else {
    return fgetc( f );
  }
}

int
fread_compr( void *buff, size_t n, size_t m, FILE *f )
{
  size_t i;
  int d;
  char *b = buff;

  if( fmf_compr ) {
    for( i = n * m; i > 0; i-- ) {
      if( ( d = fgetc_compr( f ) ) == -1 )	/****FIXME may too slow */
        return ( n * m - i ) / n;
      *b++ = d;
    }
    return m;
  } else {
    return fread( b, n, m, f );
  }
}
#endif	/* HAVE_ZLIB_H */

int
fget_word(FILE *input)
{
  return fgetc_compr(input) + (fgetc_compr(input) << 8);
}

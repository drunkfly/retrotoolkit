/* fmfconv_compr.h: Convert .fmf movie files - zlib compression routines
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

#ifndef FMFCONV_COMPR_H
#define FMFCONV_COMPR_H

#include <config.h>

#include <stdio.h>

#ifdef HAVE_ZLIB_H
int feof_compr( FILE *f );
int fgetc_compr( FILE *f );
int fread_compr( void *buff, size_t n, size_t m, FILE *f );
void fmf_compr_init( void );
void fmf_compr_end( void );
#else	/* HAVE_ZLIB_H */
#define fgetc_compr fgetc
#define fread_compr fread
#define feof_compr feof
#endif	/* HAVE_ZLIB_H */

int fget_word(FILE *input);

extern int fmf_compr;			/* fmf compressed or not */

#endif /* #ifndef FMFCONV_COMPR_H */

/* fmfconv.h: Convert .fmf movie files
   Copyright (c) 2004-2015 Gergely Szasz

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

#ifndef FMFCONV_H
#define FMFCONV_H

#include <libspectrum.h>

#include "fmfconv_types.h"

#define FMFCONV_VER_MAJOR 0
#define FMFCONV_VER_MINOR 5
#define FMFCONV_RC_VERSION FMFCONV_VER_MAJOR,FMFCONV_VER_MINOR,0,0
#define __FMFCONV_VER_STRING(maj, min) #maj "." #min
#define _FMFCONV_VER_STRING(maj, min) __FMFCONV_VER_STRING( maj, min )
#define FMFCONV_VER_STRING _FMFCONV_VER_STRING( FMFCONV_VER_MAJOR, FMFCONV_VER_MINOR )

#ifdef HAVE_INTTYPES_H
#include <inttypes.h>
#else
#define PRIu64 "llu"
#endif /* #ifdef HAVE_INTTYPES_H */

#ifdef HAVE_ZLIB_H
#define USE_ZLIB
#endif

#define printe(format...) \
    fprintf (stderr, "*ERR " format)

#define printw(format...) \
    fprintf (stderr, "*WRN " format)

#define printi(lvl,format...) \
    if( lvl <= verbose ) fprintf( stderr, "*INF " format )

enum {
  ERR_OPEN_INP = 1,
  ERR_OPEN_OUT,
  ERR_OPEN_SND,
  ERR_CORRUPT_INP,
  ERR_VERSION_INP,
  ERR_NO_ZLIB,
  ERR_ENDOFFILE,
  ERR_OUTOFMEM,
  ERR_CORRUPT_SND,
  ERR_BAD_PARAM,
  ERR_WRITE_OUT,
  ERR_WRITE_SND,
};


typedef enum {
  TYPE_UNSET = -1,

  TYPE_NONE = 0,

  TYPE_FMF,	/* i */

  TYPE_SCR,	/* io (no conv) */
  TYPE_PPM,	/* o (conv to RGB) */
  TYPE_PNG,
  TYPE_JPEG,

  TYPE_MJPEG,	/* o */
  TYPE_AVI,	/* os */

  TYPE_YUV,	/* o (conv to YUV) */
  TYPE_FFMPEG,	/* os */

  TYPE_WAV,	/* s */
  TYPE_AU,	/* s */
  TYPE_AIFF,	/* s */

  TYPE_444,
  TYPE_422,
  TYPE_420J,
  TYPE_420M,
  TYPE_420,
  TYPE_410,
  TYPE_mono,

  TYPE_AVI_DIB,

  TYPE_RESCALE_WH,
  TYPE_RESCALE_X,

  TYPE_PERC,
  TYPE_BAR,
  TYPE_FRAME,
  TYPE_TIME,

  TYPE_NOCUT,
  TYPE_CUTFROM,
  TYPE_CUT,



} type_t;

extern int verbose;

extern FILE *out, *snd;
extern int out_to_stdout;

extern int frm_slice_x, frm_slice_y, frm_slice_w, frm_slice_h;
extern int frm_w, frm_h;
extern int frm_fps;
extern fmf_machine_type frm_mch;
extern libspectrum_qword output_no;	/* output frame no */

extern type_t yuv_t, out_t, snd_t;
extern fmf_screen_type scr_t;

extern const char *out_name;
extern int out_w, out_h;
extern int out_fps;			/* desired output frame rate */
extern int out_header_ok;			/* output header ok? */

extern int out_chn, out_rte, out_fsz, out_len;

extern libspectrum_signed_byte *sound8;	/* sound buffer for x-law */
extern libspectrum_signed_word *sound16;	/* sound buffer for pcm */

extern fmf_sound_type snd_enc;			/* sound type (pcm/alaw/ulaw) */
extern int snd_rte, snd_chn, snd_fsz, snd_len;	/* sound rate (Hz), sound channels (1/2), sound length in byte  */
extern int snd_header_ok;			/* sound header ok? */
extern int snd_little_endian;

extern libspectrum_byte zxscr[];		/* 2x 40x240 bitmap1 bitmap2 */
extern libspectrum_byte attrs[];		/* 40x240 attrib */
extern libspectrum_byte pix_rgb[];		/* other view of data */
extern libspectrum_byte *pix_yuv[];		/* other view of data */
extern int yuv_ylen, yuv_uvlen;
extern int machine_timing[];

extern int force_aifc;			/* record aifc file even PCM sound */

FILE *fopen_overwr( const char *path, const char *mode, int rw );
libspectrum_dword swap_endian_dword( libspectrum_dword d );
void pcm_swap_endian( void );	/* buff == sound */
int next_outname( libspectrum_qword num );
int close_out( void );
int open_out( void );

int snd_write_avi( void );
int out_write_avi( void );
void out_finalize_avi( void );

int out_write_yuv( void );

int out_write_scr( void );
int out_write_ppm( void );
#ifdef USE_LIBPNG
int out_write_png( void );
void print_png_version( void );
#endif
#ifdef USE_LIBJPEG
int out_write_jpg( void );
int out_write_mjpeg( void );
int out_build_avi_mjpeg_frame( char **frame_buff,
                               unsigned long int *frame_size );
void out_finalize_mjpeg( void );
void print_jpeg_version( void );
#endif

int snd_write_wav( void );
void snd_finalize_wav( void );

int snd_write_au( void );
void snd_finalize_au( void );

int snd_write_aiff( void );
void snd_finalize_aiff( void );

#endif	/* FMFCONV_H */

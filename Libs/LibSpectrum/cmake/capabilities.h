#ifdef HAVE_ZLIB_H
/* we support snapshots etc. requiring zlib (e.g. compressed szx) */
#define	LIBSPECTRUM_SUPPORTS_ZLIB_COMPRESSION	(1)

/* zlib (de)compression routines */

WIN32_DLL libspectrum_error
libspectrum_zlib_inflate( const libspectrum_byte *gzptr, size_t gzlength,
			  libspectrum_byte **outptr, size_t *outlength );

WIN32_DLL libspectrum_error
libspectrum_zlib_compress( const libspectrum_byte *data, size_t length,
			   libspectrum_byte **gzptr, size_t *gzlength );

#endif

/* we support files compressed with bz2 */
#ifdef HAVE_LIBBZ2
#define	LIBSPECTRUM_SUPPORTS_BZ2_COMPRESSION	(1)
#endif

/* we support wav files */
#ifdef HAVE_LIB_AUDIOFILE
#define	LIBSPECTRUM_SUPPORTS_AUDIOFILE	(1)
#endif

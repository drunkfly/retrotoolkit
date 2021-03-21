/* snapdump.c: List contents of ZX Spectrum snapshot files
   Copyright (c) 2017 Sergio Baldoví

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

   E-mail: serbalgi@gmail.com

*/

#include <config.h>

#include <errno.h>
#include <getopt.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#ifdef HAVE_GCRYPT_H
#include <gcrypt.h>
#endif				/* #ifdef HAVE_GCRYPT_H */

#include <libspectrum.h>

#include "compat.h"
#include "utils.h"

/* Sizes of some of the arrays in the snap structure */
#define SNAPSHOT_RAM_PAGES 16
#define SNAPSHOT_SLT_PAGES 256
#define SNAPSHOT_ZXATASP_PAGES 32
#define SNAPSHOT_ZXCF_PAGES 64
#define SNAPSHOT_DOCK_EXROM_PAGES 8
#define SNAPSHOT_JOYSTICKS 7
#define SNAPSHOT_DIVIDE_PAGES 4
#define SNAPSHOT_DIVMMC_PAGES 64

#define PROGRAM_NAME "snapdump"

char *progname;
static int dump_memory = 0;
static const char *snap_file = NULL;

typedef int (*periph_active_fn)( libspectrum_snap *snap );
typedef void (*periph_dump_fn)( libspectrum_snap *snap );

typedef struct snap_periph_t {
  periph_active_fn active;
  periph_dump_fn dump;
  const char *name;
} snap_periph_t;

#ifdef HAVE_GCRYPT_H

static char *
calculate_sha1sum( const libspectrum_byte *data, size_t data_length )
{
  unsigned int digest_length, i;
  libspectrum_byte *digest;
  char *hash, *p;

  if( !data || !data_length ) return NULL;

  /* Get SHA1 sum */
  digest_length = gcry_md_get_algo_dlen( GCRY_MD_SHA1 );
  digest = libspectrum_new( libspectrum_byte, digest_length );
  gcry_md_hash_buffer( GCRY_MD_SHA1, digest, data, data_length );

  /* Convert bytes to hex */
  hash = libspectrum_new( char, digest_length * 2 + 1 );
  p = hash;

  for ( i = 0; i < digest_length; i++ ) {
    snprintf( p, 3, "%02x", digest[i] );
    p += 2;
  }
  *p = '\0';
  libspectrum_free( digest );

  return hash;
}

#else

static char *
calculate_sha1sum( const libspectrum_byte *data, size_t data_length )
{
  return NULL;
}

#endif				/* #ifdef HAVE_GCRYPT_H */

static char *
get_memory_filename( const char *memory_name, int page )
{
  char *buffer, *buffer2, *last_dot, *filename;
  size_t filename_len;

  /* Get the basename without the last extension */
  buffer = strdup( snap_file );
  if( !buffer ) {
    fprintf( stderr, "out of memory at %s:%d", __FILE__, __LINE__ );
    return NULL;
  }

  buffer2 = compat_basename( buffer );
  last_dot = strrchr( buffer2, '.' ); if( last_dot ) *last_dot = '\0';

  filename_len = strlen( buffer2 ) + strlen( memory_name ) + sizeof( page ) + 7;
  filename = libspectrum_new( char, filename_len );

  if( page >= 0 )
    snprintf( filename, filename_len, "%s_%s_%d.bin", buffer2, memory_name,
              page );
  else
    snprintf( filename, filename_len, "%s_%s.bin", buffer2, memory_name );

  free( buffer );

  return filename;
}

static void
print_memory_block( const char *name, const libspectrum_byte *data, size_t size )
{
  char *sha1, *filename;

  sha1 = calculate_sha1sum( data, size );
  if( sha1 ) {
    printf( "%s size: 0x%04lX, sha1: %s\n", name, (unsigned long)size, sha1 );
    libspectrum_free( sha1 );
  } else {
    printf( "%s size: 0x%04lX\n", name, (unsigned long)size );
  }

  if( dump_memory ) {
    filename = get_memory_filename( name, -1 );
    write_file( filename, data, size );
    free( filename );
  }
}

static void
print_memory_page( const char *name, int page, const libspectrum_byte *data,
                   size_t size )
{
  char *sha1, *filename;

  sha1 = calculate_sha1sum( data, size );
  if( sha1 ) {
    printf( "%s_%d size: 0x%04lX, sha1: %s\n", name, page, (unsigned long)size,
            sha1 );
    libspectrum_free( sha1 );
  } else {
    printf( "%s_%d size: 0x%04lX\n", name, page, (unsigned long)size );
  }

  if( dump_memory ) {
    filename = get_memory_filename( name, page );
    write_file( filename, data, size );
    free( filename );
  }
}

static void
dump_beta128( libspectrum_snap *snap )
{
  libspectrum_byte *rom_data;

  if( ! libspectrum_snap_beta_active( snap ) )
    return;

  printf( "\nBETA 128\n" );
  printf( "Beta 128 paged: %d\n", libspectrum_snap_beta_paged( snap ) );
  printf( "Beta 128 autoboot: %d\n", libspectrum_snap_beta_autoboot( snap ) );
  printf( "Beta 128 custom rom: %d\n", libspectrum_snap_beta_custom_rom( snap ) );
  rom_data = libspectrum_snap_beta_rom( snap, 0 );
  if( rom_data ) print_memory_block( "beta128_rom", rom_data, 0x4000 );

  printf( "Beta 128 direction: %d\n", libspectrum_snap_beta_direction( snap ) );
  printf( "Beta 128 drive count: %d\n", libspectrum_snap_beta_drive_count( snap ) );
  printf( "Beta 128 system: 0x%02X\n", libspectrum_snap_beta_system( snap ) );
  printf( "Beta 128 track:  0x%02X\n", libspectrum_snap_beta_track( snap ) );
  printf( "Beta 128 sector: 0x%02X\n", libspectrum_snap_beta_sector( snap ) );
  printf( "Beta 128 data:   0x%02X\n", libspectrum_snap_beta_data( snap ) );
  printf( "Beta 128 status: 0x%02X\n", libspectrum_snap_beta_status( snap ) );
}

static void
dump_covox( libspectrum_snap *snap )
{
  if( !libspectrum_snap_covox_active( snap ) )
    return;

  printf( "\nCOVOX\n" );
  printf( "Covox dac: 0x%02X\n", libspectrum_snap_covox_dac( snap ) );
}

static void
dump_divide( libspectrum_snap *snap )
{
  libspectrum_byte *eprom_data, *ram_data;
  size_t pages, i;

  if( ! libspectrum_snap_divide_active( snap ) )
    return;

  printf( "\nDIVIDE\n" );
  printf( "DivIDE paged: %d\n", libspectrum_snap_divide_paged( snap ) );
  printf( "DivIDE control: 0x%02X\n", libspectrum_snap_divide_control( snap ) );
  printf( "DivIDE eprom writeprotect: %d\n",
          libspectrum_snap_divide_eprom_writeprotect( snap ) );
  eprom_data = libspectrum_snap_divide_eprom( snap, 0 );
  if( eprom_data ) print_memory_block( "divide_eprom", eprom_data, 0x2000 );

  pages = libspectrum_snap_divide_pages( snap );
  printf( "DivIDE pages: %lu\n", (unsigned long)pages );

  for( i = 0; i < SNAPSHOT_DIVIDE_PAGES; i++ ) {
    ram_data = libspectrum_snap_divide_ram( snap, i );
    if( ram_data ) print_memory_page( "divide_ram_page", i, ram_data, 0x2000 );
  }
}

static void
dump_divmmc( libspectrum_snap *snap )
{
  libspectrum_byte *eprom_data, *ram_data;
  size_t pages, i;

  if( ! libspectrum_snap_divmmc_active( snap ) )
    return;

  printf( "\nDIVMMC\n" );
  printf( "DivMMC paged: %d\n", libspectrum_snap_divmmc_paged( snap ) );
  printf( "DivMMC control: 0x%02X\n", libspectrum_snap_divmmc_control( snap ) );
  printf( "DivMMC eprom writeprotect: %d\n",
          libspectrum_snap_divmmc_eprom_writeprotect( snap ) );
  eprom_data = libspectrum_snap_divmmc_eprom( snap, 0 );
  if( eprom_data ) print_memory_block( "divmmc_eprom", eprom_data, 0x2000 );

  pages = libspectrum_snap_divmmc_pages( snap );
  printf( "DivMMC pages: %lu\n", (unsigned long)pages );

  for( i = 0; i < SNAPSHOT_DIVMMC_PAGES; i++ ) {
    ram_data = libspectrum_snap_divmmc_ram( snap, i );
    if( ram_data ) print_memory_page( "divmmc_ram_page", i, ram_data, 0x2000 );
  }
}

static void
dump_interface1( libspectrum_snap *snap )
{
  libspectrum_byte *rom_data;
  size_t rom_length;

  if( ! libspectrum_snap_interface1_active( snap ) )
    return;

  printf( "\nINTERFACE I\n" );
  printf( "Interface I paged: %d\n", libspectrum_snap_interface1_paged( snap ) );
  printf( "Interface I custom rom: %d\n",
          libspectrum_snap_interface1_custom_rom( snap ) );
  rom_data = libspectrum_snap_interface1_rom( snap, 0 );
  rom_length = libspectrum_snap_interface1_rom_length( snap, 0 );
  if( rom_data ) print_memory_block( "Interface_I_rom", rom_data, rom_length );
  printf( "Interface I microdrive count: %d\n",
          libspectrum_snap_interface1_drive_count( snap ) );
}

static void
dump_interface2( libspectrum_snap *snap )
{
  libspectrum_byte *rom_data;
  size_t rom_length;

  if( ! libspectrum_snap_interface2_active( snap ) )
    return;

  printf( "\nINTERFACE II\n" );
  rom_data = libspectrum_snap_interface2_rom( snap, 0 );
  rom_length = 0x4000;
  if( rom_data ) print_memory_block( "Interface_II_rom", rom_data, rom_length );
}

static void
dump_multiface( libspectrum_snap *snap )
{
  libspectrum_byte *ram_data;
  size_t ram_length;

  if( ! libspectrum_snap_multiface_active( snap ) )
    return;

  printf( "\nMULTIFACE\n" );

  if( libspectrum_snap_multiface_model_one( snap ) )
    printf( "Multiface model: Multiface One\n" );
  else if( libspectrum_snap_multiface_model_128( snap ) )
    printf( "Multiface model: Multiface 128\n" );
  else if( libspectrum_snap_multiface_model_3( snap ) )
    printf( "Multiface model: Multiface 3\n" );
  else
    printf( "Multiface model: unknown\n" );

  printf( "Multiface paged: %d\n", libspectrum_snap_multiface_paged( snap ) );

  if( libspectrum_snap_multiface_model_one( snap ) ) {
    printf( "Multiface One disabled: %d\n",
            libspectrum_snap_multiface_disabled( snap ) );
  } else {
    printf( "Multiface 128/3 software lockout: %d\n",
            libspectrum_snap_multiface_software_lockout( snap ) );
  }

  printf( "Multiface red button disabled: %d\n",
          libspectrum_snap_multiface_red_button_disabled( snap ) );

  ram_length = libspectrum_snap_multiface_ram_length( snap, 0 );
  ram_data = libspectrum_snap_multiface_ram( snap, 0 );
  if( ram_data ) print_memory_block( "multiface_ram", ram_data, ram_length );
}

static void
dump_opus( libspectrum_snap *snap )
{
  libspectrum_byte *rom_data, *ram_data;

  if( ! libspectrum_snap_opus_active( snap ) )
    return;

  printf( "\nOPUS DISCOVERY\n" );
  printf( "Opus Discovery paged: %d\n", libspectrum_snap_opus_paged( snap ) );
  printf( "Opus Discovery custom rom: %d\n",
          libspectrum_snap_opus_custom_rom( snap ) );
  rom_data = libspectrum_snap_opus_rom( snap, 0 );
  if( rom_data ) print_memory_block( "opus_rom", rom_data, 0x2000 );
  ram_data = libspectrum_snap_opus_ram( snap, 0 );
  if( ram_data ) print_memory_block( "opus_ram", ram_data, 0x0800 );

  printf( "Opus Discovery control register A: %d\n",
          libspectrum_snap_opus_control_a( snap ) );
  printf( "Opus Discovery data direction A: %d\n",
          libspectrum_snap_opus_data_dir_a( snap ) );
  printf( "Opus Discovery peripheral reg A: %d\n",
          libspectrum_snap_opus_data_reg_a( snap ) );

  printf( "Opus Discovery control register B: %d\n",
          libspectrum_snap_opus_control_b( snap ) );
  printf( "Opus Discovery data direction B: %d\n",
          libspectrum_snap_opus_data_dir_b( snap ) );
  printf( "Opus Discovery peripheral reg B: %d\n",
          libspectrum_snap_opus_data_reg_b( snap ) );

  printf( "Opus Discovery direction: %d\n",
          libspectrum_snap_opus_direction( snap ) );
  printf( "Opus Discovery drive count: %d\n",
          libspectrum_snap_opus_drive_count( snap ) );
  printf( "Opus Discovery track:  0x%02X\n",
          libspectrum_snap_opus_track( snap ) );
  printf( "Opus Discovery sector: 0x%02X\n",
          libspectrum_snap_opus_sector( snap ) );
  printf( "Opus Discovery data:   0x%02X\n",
          libspectrum_snap_opus_data( snap ) );
  printf( "Opus Discovery status: 0x%02X\n",
          libspectrum_snap_opus_status( snap ) );
}

static void
dump_plusd( libspectrum_snap *snap )
{
  libspectrum_byte *rom_data, *ram_data;

  if( ! libspectrum_snap_plusd_active( snap ) )
    return;

  printf( "\n+D\n" );
  printf( "+D paged: %d\n", libspectrum_snap_plusd_paged( snap ) );
  printf( "+D custom rom: %d\n", libspectrum_snap_plusd_custom_rom( snap ) );
  rom_data = libspectrum_snap_plusd_rom( snap, 0 );
  if( rom_data ) print_memory_block( "plusd_rom", rom_data, 0x2000 );
  ram_data = libspectrum_snap_plusd_ram( snap, 0 );
  if( ram_data ) print_memory_block( "plusd_ram", ram_data, 0x2000 );

  printf( "+D control register B: %d\n",
          libspectrum_snap_plusd_control( snap ) );

  printf( "+D direction: %d\n", libspectrum_snap_plusd_direction( snap ) );
  printf( "+D drive count: %d\n", libspectrum_snap_plusd_drive_count( snap ) );
  printf( "+D track:  0x%02X\n", libspectrum_snap_plusd_track( snap ) );
  printf( "+D sector: 0x%02X\n", libspectrum_snap_plusd_sector( snap ) );
  printf( "+D data:   0x%02X\n", libspectrum_snap_plusd_data( snap ) );
  printf( "+D status: 0x%02X\n", libspectrum_snap_plusd_status( snap ) );
}

static void
dump_specdrum( libspectrum_snap *snap )
{
  if( ! libspectrum_snap_specdrum_active( snap ) )
    return;

  printf( "\nSPECDRUM\n" );
  printf( "SpecDrum dac: %d\n", libspectrum_snap_specdrum_dac( snap ) );
}

static void
dump_spectranet( libspectrum_snap *snap )
{
  libspectrum_byte *w5100_regs;
  int i;

  if( ! libspectrum_snap_spectranet_active( snap ) )
    return;

  printf( "\nSPECTRANET\n" );
  printf( "Spectranet paged: %d\n",
          libspectrum_snap_spectranet_paged( snap ) );
  printf( "Spectranet paged via io: %d\n",
          libspectrum_snap_spectranet_paged_via_io( snap ) );

  printf( "Spectranet programmable trap: 0x%04X\n",
          libspectrum_snap_spectranet_programmable_trap( snap ) );
  printf( "Spectranet programmable trap active: %d\n",
          libspectrum_snap_spectranet_programmable_trap_active( snap ) );
  printf( "Spectranet programmable trap msb: %d\n",
          libspectrum_snap_spectranet_programmable_trap_msb( snap ) );
  printf( "Spectranet deny downstream a15: %d\n",
          libspectrum_snap_spectranet_deny_downstream_a15( snap ) );
  printf( "Spectranet nmi flipflop: %d\n",
          libspectrum_snap_spectranet_nmi_flipflop( snap ) );

  printf( "Spectranet all traps disabled: %d\n",
          libspectrum_snap_spectranet_all_traps_disabled( snap ) );
  printf( "Spectranet rst8 trap disabled: %d\n",
          libspectrum_snap_spectranet_rst8_trap_disabled( snap ) );
  printf( "Spectranet page A: %d\n",
          libspectrum_snap_spectranet_page_a( snap ) );
  printf( "Spectranet page B: %d\n",
          libspectrum_snap_spectranet_page_b( snap ) );

  printf( "Spectranet W5100 registers:" );
  w5100_regs = libspectrum_snap_spectranet_w5100( snap, 0 );
  for( i = 0; i < 48; i++ ) {
    if( i % 16 == 0 ) printf( "\n " );
    printf( " %02X", w5100_regs[i] );
  }
  printf( "\n" );

  if( libspectrum_snap_spectranet_flash( snap, 0 ) )
    print_memory_block( "spectranet_flash",
                        libspectrum_snap_spectranet_flash( snap, 0 ), 0x20000 );

  if( libspectrum_snap_spectranet_ram( snap, 0 ) )
    print_memory_block( "spectranet_ram",
                        libspectrum_snap_spectranet_ram( snap, 0 ), 0x20000 );
}

static void
dump_slt( libspectrum_snap *snap )
{
  libspectrum_byte *slt_data;
  size_t slt_length;
  int i, have_slt;

  have_slt = 0;
  for( i = 0; i < SNAPSHOT_SLT_PAGES; i++ ) {
    if( libspectrum_snap_slt_length( snap, i ) ) {
      have_slt = 1;
      break;
    }
  }

  if( !have_slt ) return;

  printf( "\nSLT\n" );

  slt_data = libspectrum_snap_slt_screen( snap );
  if( slt_data ) {
    printf( "slt screen level: %d\n",
            libspectrum_snap_slt_screen_level( snap ) );
    print_memory_block( "slt_screen", slt_data, 6912 );
  }

  for( i = 0; i < SNAPSHOT_SLT_PAGES; i++ ) {
    slt_length = libspectrum_snap_slt_length( snap, i );
    if( slt_length ) {
      slt_data = libspectrum_snap_slt( snap, i );
      print_memory_page( "slt_level", i, slt_data, slt_length );
    }
  }
}

static void
dump_ulaplus( libspectrum_snap *snap )
{
  libspectrum_byte *buffer;
  unsigned int i;

  if( ! libspectrum_snap_ulaplus_active( snap ) )
    return;

  printf( "\nULAplus\n" );
  printf( "ULAplus enabled: %d\n",
          libspectrum_snap_ulaplus_palette_enabled( snap ) );
  printf( "ULAplus current register: %d\n",
          libspectrum_snap_ulaplus_current_register( snap ) );

  buffer = libspectrum_snap_ulaplus_palette( snap, 0 );
  printf( "ULAplus registers:" );
  for( i = 0; i < 64; i++ ) {
    if( i % 16 == 0 ) printf( "\n " );
    printf( " %02X", buffer[i] );
  }
  printf( "\n" );

  printf( "ULAplus ff register: %d\n",
          libspectrum_snap_ulaplus_ff_register( snap ) );
}

static void
dump_zxatasp( libspectrum_snap *snap )
{
  libspectrum_byte *ram_page;
  int i;

  if( ! libspectrum_snap_zxatasp_active( snap ) )
    return;

  printf( "\nZXATASP\n" );
  printf( "ZXATASP upload: %d\n", libspectrum_snap_zxatasp_upload( snap ) );
  printf( "ZXATASP write protected: %d\n",
          libspectrum_snap_zxatasp_writeprotect( snap ) );
  printf( "ZXATASP port A: 0x%02X\n", libspectrum_snap_zxatasp_port_a( snap ) );
  printf( "ZXATASP port B: 0x%02X\n", libspectrum_snap_zxatasp_port_b( snap ) );
  printf( "ZXATASP port C: 0x%02X\n", libspectrum_snap_zxatasp_port_c( snap ) );
  printf( "ZXATASP control port: 0x%02X\n",
          libspectrum_snap_zxatasp_control( snap ) );
  printf( "ZXATASP RAM pages: %lu\n",
          (unsigned long)libspectrum_snap_zxatasp_pages( snap ) );
  printf( "ZXATASP active page: %lu\n",
          (unsigned long)libspectrum_snap_zxatasp_current_page( snap ) );

  for( i = 0; i < SNAPSHOT_ZXATASP_PAGES; i++ ) {
    ram_page = libspectrum_snap_zxatasp_ram( snap, i );
    if( ram_page )
      print_memory_page( "zxatasp_ram_page", i, ram_page, 0x4000 );
  }
}

static void
dump_zxcf( libspectrum_snap *snap )
{
  libspectrum_byte *ram_page;
  int i;

  if( ! libspectrum_snap_zxcf_active( snap ) )
    return;

  printf( "\nZXCF\n" );
  printf( "ZXCF upload: %d\n", libspectrum_snap_zxcf_upload( snap ) );
  printf( "ZXCF memmory control register: 0x%02X\n",
          libspectrum_snap_zxcf_memctl( snap ) );
  printf( "ZXCF RAM pages: %lu\n",
          (unsigned long)libspectrum_snap_zxcf_pages( snap ) );

  for( i = 0; i < SNAPSHOT_ZXCF_PAGES; i++ ) {
    ram_page = libspectrum_snap_zxcf_ram( snap, i );
    if( ram_page )
      print_memory_page( "zxcf_ram_page", i, ram_page, 0x4000 );
  }
}

static const snap_periph_t periph_list[] = {
  { libspectrum_snap_beta_active, dump_beta128, "Beta 128" },
  { libspectrum_snap_covox_active, dump_covox, "Covox" },
  { libspectrum_snap_plusd_active, dump_plusd, "+D" },
  { libspectrum_snap_didaktik80_active, NULL, "Didaktik 80" },
  { libspectrum_snap_disciple_active, NULL, "DISCiPLE" },
  { libspectrum_snap_divide_active, dump_divide, "DivIDE" },
  { libspectrum_snap_divmmc_active, dump_divmmc, "DivMMC" },
  { libspectrum_snap_fuller_box_active, NULL, "Fuller box" },
  { libspectrum_snap_interface1_active, dump_interface1, "Interface I" },
  { libspectrum_snap_interface2_active, dump_interface2, "Interface II cartridge" },
  { libspectrum_snap_kempston_mouse_active, NULL, "Kempston mouse" },
  { libspectrum_snap_issue2, NULL, "Keyboard issue 2" },
  { libspectrum_snap_melodik_active, NULL, "Melodik" },
  { libspectrum_snap_multiface_active, dump_multiface, "Multiface" },
  { libspectrum_snap_opus_active, dump_opus, "Opus Discovery" },
  { libspectrum_snap_simpleide_active, NULL, "Simple IDE" },
  { libspectrum_snap_specdrum_active, dump_specdrum, "SpecDrum" },
  { libspectrum_snap_spectranet_active, dump_spectranet, "Spectranet" },
  { libspectrum_snap_dock_active, NULL, "Timex Dock cartridge" },
  { libspectrum_snap_ulaplus_active, dump_ulaplus, "ULAplus" },
  { libspectrum_snap_usource_active, NULL, "uSource" },
  { libspectrum_snap_zxatasp_active, dump_zxatasp, "ZXATASP" },
  { libspectrum_snap_zxcf_active, dump_zxcf, "ZXCF" },
  { libspectrum_snap_zxmmc_active, NULL, "ZXMMC" },
  { libspectrum_snap_zx_printer_active, NULL, "ZX Printer" },
  { NULL, NULL, NULL }
};

static void
dump_snapshot( libspectrum_snap *snap )
{
  size_t i, custom_rom_pages, num_joysticks;
  const snap_periph_t *p;
  libspectrum_machine machine;
  int capabilities, custom_rom;

  printf( "filename: %s\n", snap_file );
  machine = libspectrum_snap_machine( snap );
  printf( "machine: %s\n", libspectrum_machine_name( machine ) );

  capabilities = libspectrum_machine_capabilities( machine );

  /* Registers and the like */
  printf( "\nREGISTERS\n" );
  printf( "PC:  0x%04X\n", libspectrum_snap_pc( snap ) );
  printf( "SP:  0x%04X\n", libspectrum_snap_sp( snap ) );
  printf( "AF:  0x%02X%02X\n", libspectrum_snap_a( snap ),
                               libspectrum_snap_f( snap ) );
  printf( "AF': 0x%02X%02X\n", libspectrum_snap_a_( snap ),
                               libspectrum_snap_f_( snap ) );
  printf( "BC:  0x%04X\n", libspectrum_snap_bc( snap ) );
  printf( "BC': 0x%04X\n", libspectrum_snap_bc_( snap ) );
  printf( "DE:  0x%04X\n", libspectrum_snap_de( snap ) );
  printf( "DE': 0x%04X\n", libspectrum_snap_de_( snap ) );
  printf( "HL:  0x%04X\n", libspectrum_snap_hl( snap ) );
  printf( "HL': 0x%04X\n", libspectrum_snap_hl_( snap ) );
  printf( "IX:  0x%04X\n", libspectrum_snap_ix( snap ) );
  printf( "IY:  0x%04X\n", libspectrum_snap_iy( snap ) );
  printf( "I:   0x%02X\n", libspectrum_snap_i( snap ) );
  printf( "R:   0x%02X\n", libspectrum_snap_r( snap ) );
  printf( "meptr:  0x%04X\n", libspectrum_snap_memptr( snap ) );
  printf( "IFF1:   %u\n", libspectrum_snap_iff1( snap ) );
  printf( "IFF2:   %u\n", libspectrum_snap_iff2( snap ) );
  printf( "IM:     %u\n", libspectrum_snap_im( snap ) );
  printf( "halted: %d\n", libspectrum_snap_halted( snap ) );
  printf( "last instruction EI: %d\n",
          libspectrum_snap_last_instruction_ei( snap ) );
  printf( "last instruction set flags: %d\n",
          libspectrum_snap_last_instruction_set_f( snap ) );

  /* Custom ROM */
  custom_rom = libspectrum_snap_custom_rom( snap );
  if( custom_rom ) {
    printf( "\nCUSTOM ROM\n" );
    printf( "custom rom: %d\n", custom_rom );
    custom_rom_pages = libspectrum_snap_custom_rom_pages( snap );
    printf( "custom rom pages: %lu\n", (unsigned long)custom_rom_pages );
    for( i = 0; i < custom_rom_pages; i++ ) {
      print_memory_page( "custom_rom_", i, libspectrum_snap_roms( snap, i ),
                         libspectrum_snap_rom_length( snap, i ) );
    }
  }

  printf( "\nRAM PAGES\n" );
  for( i = 0; i < SNAPSHOT_RAM_PAGES; i++ )
    if( libspectrum_snap_pages( snap, i ) )
      print_memory_page( "ram_page", i, libspectrum_snap_pages( snap, i ),
                         0x4000 );

  if( libspectrum_snap_dock_active( snap ) ) {

    printf( "\nTIMEX PAGES\n" );
    for( i = 0; i < 8; i++ ) {
      if( libspectrum_snap_dock_cart( snap, i ) ) {
        if( libspectrum_snap_dock_ram( snap, i ) )
          print_memory_page( "dock_ram_page", i,
                             libspectrum_snap_dock_cart( snap, i ), 0x2000 );
        else
          print_memory_page( "dock_rom_page", i,
                             libspectrum_snap_dock_cart( snap, i ), 0x2000 );
      }
    }

    for( i = 0; i < 8; i++ ) {
      if( libspectrum_snap_exrom_cart( snap, i ) ) {
        if( libspectrum_snap_exrom_ram( snap, i ) )
          print_memory_page( "exrom_ram_page", i,
                             libspectrum_snap_exrom_cart( snap, i ), 0x2000 );
        else
          print_memory_page( "exrom_rom_page", i,
                             libspectrum_snap_exrom_cart( snap, i ), 0x2000 );
      }
    }

  }

  /* Peripheral status */
  printf( "\nPERIPHERAL STATUS\n" );
  printf( "tstates: %u\n", libspectrum_snap_tstates( snap ) );
  printf( "late timings: %d\n", libspectrum_snap_late_timings( snap ) );
  printf( "ULA: %02X\n", libspectrum_snap_out_ula( snap ) );
  printf( "128 mem: 0x%02X\n", libspectrum_snap_out_128_memoryport( snap ) );
  printf( "+3 mem: 0x%02X\n", libspectrum_snap_out_plus3_memoryport( snap ) );

  if( capabilities & ( LIBSPECTRUM_MACHINE_CAPABILITY_TIMEX_MEMORY |
                       LIBSPECTRUM_MACHINE_CAPABILITY_SE_MEMORY ) ||
      libspectrum_snap_out_scld_hsr( snap ) != 0 )
    printf( "Timex SCLD hsr: 0x%02X\n", libspectrum_snap_out_scld_hsr( snap ) );

  if( capabilities & LIBSPECTRUM_MACHINE_CAPABILITY_TIMEX_VIDEO ||
      libspectrum_snap_out_scld_dec( snap ) != 0 )
    printf( "Timex SCLD dec: 0x%02X\n", libspectrum_snap_out_scld_dec( snap ) );

  /* AY status */
  if( capabilities & LIBSPECTRUM_MACHINE_CAPABILITY_AY ||
      libspectrum_snap_melodik_active( snap ) ||
      libspectrum_snap_fuller_box_active( snap ) ) {

    printf( "AY: 0x%02X\n", libspectrum_snap_out_ay_registerport( snap ) );
    printf( "AY registers:" );
    for( i = 0; i < 16; i++ ) {
      printf( " %02X", libspectrum_snap_ay_registers( snap, i ) );
    }
    printf( "\n" );
  }

  /* .slt data */
  dump_slt( snap );

  num_joysticks = libspectrum_snap_joystick_active_count( snap );
  printf( "Joysticks: %lu\n", (unsigned long)num_joysticks );
  for( i = 0; i < num_joysticks; i++ ) {
    printf( "Joystick %lu Type: %s\n", (unsigned long)i,
      libspectrum_joystick_name( libspectrum_snap_joystick_list( snap, i ) ) );
  }

  for( i = 0; i < num_joysticks; i++ ) {
    printf( "Joystick %lu Inputs: %d\n", (unsigned long)i,
      libspectrum_snap_joystick_inputs( snap, i ) );
  }

  /* Peripherals list */
  printf( "Peripherals: " );
  i = 0;
  p = periph_list;
  while( p->active ) {
    if( p->active( snap ) ) {
      if( i++ ) printf( ", " );
      printf( "%s", p->name );
    }
    p++;
  }
  printf( "\n" );

  /* Peripheral details */
  p = periph_list;
  while( p->active ) {
    if( p->dump && p->active( snap ) ) {
      p->dump( snap );
    }
    p++;
  }
  printf( "\n" );
}

static void
show_version( void )
{
  printf(
   PROGRAM_NAME " (" PACKAGE ") " PACKAGE_VERSION "\n"
   "Copyright (c) 2017 Sergio Baldovi\n"
   "License GPLv2+: GNU GPL version 2 or later "
   "<http://gnu.org/licenses/gpl.html>\n"
   "This is free software: you are free to change and redistribute it.\n"
   "There is NO WARRANTY, to the extent permitted by law.\n" );
}

static void
show_help( void )
{
  printf(
    "Usage: %s [OPTION] <infile>\n"
    "List contents of Sinclair ZX Spectrum snapshot files.\n"
    "\n"
    "Options:\n"
    "  -m             Dump ROM/RAM memory pages to files.\n"
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

int
main( int argc, char **argv )
{
  libspectrum_snap *snap;
  libspectrum_id_t type; libspectrum_class_t class;
  unsigned char *buffer; size_t length;

  int error = 0;
  int c;

  struct option long_options[] = {
    { "help", 0, NULL, 'h' },
    { "version", 0, NULL, 'V' },
    { 0, 0, 0, 0 }
  };

  progname = argv[0];

  while( ( c = getopt_long( argc, argv, "mhV", long_options, NULL ) ) != -1 ) {

    switch( c ) {

    case 'm':
      dump_memory = 1;
      break;

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

  if( argc < 1 ) {
    fprintf( stderr, "%s: usage: %s [OPTION] <snapfile>\n",
             progname, progname );
    fprintf( stderr, "Try `%s --help' for more information.\n", progname );
    return 1;
  }

  error = init_libspectrum(); if( error ) return error;

  snap = libspectrum_snap_alloc();

  snap_file = argv[0];
  if( read_file( snap_file, &buffer, &length ) ) {
    libspectrum_snap_free( snap );
    return 1;
  }

  /* Parse snapshot */
  error = libspectrum_snap_read( snap, buffer, length, LIBSPECTRUM_ID_UNKNOWN,
                                 snap_file );
  if( error ) {
    libspectrum_snap_free( snap ); free( buffer );
    return error;
  }

  free( buffer );

  /* Is it really an snapshot? */
  error = libspectrum_identify_file_with_class( &type, &class, snap_file, NULL,
                                                0 );
  if( error ) { libspectrum_snap_free( snap ); return error; }

  if( class != LIBSPECTRUM_CLASS_SNAPSHOT ) {
    fprintf( stderr, "%s: '%s' is not a snapshot file\n", progname, snap_file );
    libspectrum_snap_free( snap );
    return 1;
  }

  dump_snapshot( snap );

  error = libspectrum_snap_free( snap );
  if( error ) return error;

  return 0;
}

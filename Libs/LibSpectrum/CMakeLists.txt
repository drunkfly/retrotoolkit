
if(MSVC)
    add_definitions(
        /wd4311         # 'type cast': pointer truncation
        /wd4804         # '-': unsafe use of type 'bool' in operation
        )
endif()

include_directories(
    "${CMAKE_CURRENT_SOURCE_DIR}"
    "${CMAKE_CURRENT_BINARY_DIR}"
    )

include(CheckIncludeFile)
include(CheckFunctionExists)
include(CheckCSourceCompiles)
include(TestBigEndian)

test_big_endian(WORDS_BIGENDIAN)
check_include_file("stdatomic.h" HAVE_STDATOMIC_H)
check_include_file("strings.h" HAVE_STRINGS_H)
check_include_file("inttypes.h" HAVE_INTTYPES_H)
check_include_file("memory.h" HAVE_MEMORY_H)
check_include_file("unistd.h" HAVE_UNISTD_H)
check_include_file("sys/types.h" HAVE_SYS_TYPES_H)
check_include_file("sys/stat.h" HAVE_SYS_STAT_H)
check_function_exists("strcasecmp" HAVE_STRCASECMP)
check_function_exists("strncasecmp" HAVE_STRNCASECMP)
check_function_exists("_stricmp" HAVE__STRICMP)
check_function_exists("_strnicmp" HAVE__STRNICMP)

check_c_source_compiles("
    #include <stdio.h>
    int main() {
        char buf[256];
        return snprintf(buf, sizeof(buf), \"%d\", 4);
    }" HAVE_SNPRINTF)

check_c_source_compiles("
    #include <stdio.h>
    int main() {
        char buf[256];
        return _snprintf(buf, sizeof(buf), \"%d\", 4);
    }" HAVE__SNPRINTF)

set(HAVE_GCRYPT_H FALSE)
set(HAVE_LIBBZ2 FALSE)
set(HAVE_LIB_AUDIOFILE TRUE)
set(HAVE_ZLIB_H FALSE)

set(VERSION "1.5.0")
set(STDC_HEADERS TRUE)
set(HAVE_STDINT_H TRUE)
set(HAVE_STDLIB_H TRUE)
set(HAVE_STRING_H TRUE)

set(config_h "${CMAKE_CURRENT_BINARY_DIR}/config.h")
set(config_h_cmakein "${CMAKE_CURRENT_BINARY_DIR}/config.h.cmakein")
set(config_h_in "${CMAKE_CURRENT_SOURCE_DIR}/config.h.in")
if(NOT EXISTS "${config_h_cmakein}"
        OR "${CMAKE_CURRENT_SOURCE_DIR}/CMakeLists.txt" IS_NEWER_THAN "${config_h_cmakein}"
        OR "${config_h_in}" IS_NEWER_THAN "${config_h_cmakein}"
        )
    file(READ "${config_h_in}" config)
    string(REPLACE "undef VERSION" "cmakedefine VERSION \"@VERSION@\"" config "${config}")
    string(REPLACE "undef" "cmakedefine" config "${config}")
    file(WRITE "${config_h_cmakein}" "${config}")
endif()
configure_file("${config_h_cmakein}" "${config_h}")
source_group("Generated Files" FILES "${config_h}" "${config_h_cmakein}")

set(libspectrum_h "${CMAKE_CURRENT_BINARY_DIR}/libspectrum.h")
set(libspectrum_h_in "${CMAKE_CURRENT_SOURCE_DIR}/libspectrum.h.in")
if(NOT EXISTS "${libspectrum_h}"
        OR "${CMAKE_CURRENT_SOURCE_DIR}/CMakeLists.txt" IS_NEWER_THAN "${libspectrum_h}"
        OR "${libspectrum_h_in}" IS_NEWER_THAN "${libspectrum_h}"
        )
    file(READ "${libspectrum_h_in}" header)
    string(REPLACE "__declspec( dllexport )" "" header "${header}")
    string(REPLACE "__declspec( dllimport )" "" header "${header}")
    string(REPLACE "LIBSPECTRUM_AUTOGEN_WARNING" "" header "${header}")
    string(REPLACE "LIBSPECTRUM_INCLUDE_GCRYPT" "" header "${header}")
    string(REPLACE "LIBSPECTRUM_DEFINE_TYPES" "#include \"cmake/define_types.h\"" header "${header}")
    string(REPLACE "LIBSPECTRUM_GLIB_REPLACEMENT" "#include \"cmake/glib_replacement.h\"" header "${header}")
    string(REPLACE "LIBSPECTRUM_CAPABILITIES" "#include \"cmake/capabilities.h\"" header "${header}")
    string(REPLACE "LIBSPECTRUM_SNAP_ACCESSORS" "#include \"cmake/snap_accessors.h\"" header "${header}")
    string(REPLACE "LIBSPECTRUM_TAPE_ACCESSORS" "#include \"cmake/tape_accessors.h\"" header "${header}")
    string(REPLACE "LIBSPECTRUM_SIGNATURE_PARAMETERS" "" header "${header}")
    file(WRITE "${libspectrum_h}" "${header}")
endif()
source_group("Generated Files" FILES "${libspectrum_h}")

if(NOT HAVE_STDATOMIC_H)
    set_source_files_properties(myglib/glock.c
        PROPERTIES
        HEADER_FILE_ONLY TRUE)
endif()

add(LibSpectrum
    STATIC_LIBRARY
    FOLDER
        "Libs"
    INCLUDE_DIRS
        "${CMAKE_CURRENT_SOURCE_DIR}"
    LIBS
        AudioFile
    SOURCES
        "${config_h}"
        "${config_h_cmakein}"
        "${libspectrum_h}"
        "${libspectrum_h_in}"
        myglib/garray.c
        myglib/ghash.c
        myglib/glock.c
        myglib/gslist.c
        buffer.c
        #bzip2.c
        config.h.in
        creator.c
        #crypto.c
        csw.c
        dck.c
        ide.c
        internals.h
        libspectrum.c
        memory.c
        microdrive.c
        mmc.c
        plusd.c
        pzx_read.c
        rzx.c
        sna.c
        snap_accessors.c
        snapshot.c
        snp.c
        sp.c
        symbol_table.c
        szx.c
        tap.c
        tape.c
        tape_accessors.c
        tape_block.c
        tape_block.h
        tape_set.c
        timings.c
        tzx_read.c
        tzx_write.c
        utilities.c
        warajevo_read.c
        wav.c
        z80.c
        z80em.c
        #zip.c
        #zip.h
        #zlib.c
        zxs.c
    )

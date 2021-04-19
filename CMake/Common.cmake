#################################
if(COMMON_CMAKE_INCLUDED)       #
    return()                    #
endif()                         #
set(COMMON_CMAKE_INCLUDED TRUE) #
#################################

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "CMake")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_VISIBILITY_INLINES_HIDDEN TRUE)
set(CMAKE_C_VISIBILITY_PRESET hidden)
set(CMAKE_CXX_VISIBILITY_PRESET hidden)

get_filename_component(BASE_PATH "${CMAKE_CURRENT_LIST_FILE}" ABSOLUTE)
get_filename_component(BASE_PATH "${BASE_PATH}" DIRECTORY)
get_filename_component(BASE_PATH "${BASE_PATH}" DIRECTORY)

get_filename_component(BINARIES_PATH "${CMAKE_BINARY_DIR}" DIRECTORY)
set(BINARIES_PATH "${BINARIES_PATH}/Binaries")

if(MSVC)
    add_definitions(
        -D_SCL_SECURE_NO_WARNINGS=1
        -D_CRT_SECURE_NO_WARNINGS=1
        -D_CRT_SECURE_NO_DEPRECATE=1
        -D_CRT_NONSTDC_NO_DEPRECATE=1
        )
endif()

find_package(Threads)

get_property(multiconfig GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)
if(NOT multiconfig AND (NOT CMAKE_BUILD_TYPE OR CMAKE_BUILD_TYPE STREQUAL ""))
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Build Type" FORCE)
    message(WARNING "CMAKE_BUILD_TYPE was not set, defaulting to '${CMAKE_BUILD_TYPE}'.")
endif()

#######################################################################################################################

macro(set_source_file_group file)
    get_filename_component(path "${file}" DIRECTORY)
    if ("${path}" STREQUAL "")
        string(REPLACE "/" "\\" group "Source Files")
    else()
        string(REPLACE "/" "\\" group "Source Files/${path}")
    endif()
    source_group("${group}" FILES "${file}")
endmacro()

#######################################################################################################################

macro(install_library config targetDir file)
    get_filename_component(name "${file}" NAME)
    if(EXISTS "${file}" AND NOT EXISTS "${targetDir}/${name}")
        message(STATUS "Installing dependency '${name}' (${config})")
        configure_file("${file}" "${targetDir}/${name}" COPYONLY)
    endif()
endmacro()

macro(mingw_install_phtread_library targetDir)
    if(MINGW)
        get_filename_component(gccdir "${CMAKE_CXX_COMPILER}" DIRECTORY)
        foreach(lib libwinpthread-1)
            install_library("${CMAKE_BUILD_TYPE}" "${targetDir}" "${gccdir}/${lib}.dll")
        endforeach()
    endif()
endmacro()

macro(mingw_install_libraries targetDir)
    if(MINGW)
        get_filename_component(gccdir "${CMAKE_CXX_COMPILER}" DIRECTORY)
        foreach(lib libgcc_s_dw2-1 libgcc_s_seh-1 libwinpthread-1 libstdc++-6)
            install_library("${CMAKE_BUILD_TYPE}" "${targetDir}" "${gccdir}/${lib}.dll")
        endforeach()
    endif()
endmacro()

macro(qt_install_win32_plugins suffix config targetDir libFile)
    get_filename_component(path "${libFile}" DIRECTORY)
    get_filename_component(path "${path}" DIRECTORY)
    foreach(plugin ${ARGN})
        set(name "${plugin}${suffix}.dll")
        set(file "${path}/plugins/${name}")
        if(EXISTS "${file}" AND NOT EXISTS "${targetDir}/${name}")
            message(STATUS "Installing dependency '${name}' (${config})")
            configure_file("${file}" "${targetDir}/plugins/${name}" COPYONLY)
        endif()
    endforeach()
endmacro()

macro(qt_install_library config targetDir lib)
    string(TOUPPER "${config}" cfg)
    get_target_property(file "${lib}" LOCATION_${cfg})
    get_filename_component(name "${file}" NAME)
    if(EXISTS "${file}" AND NOT EXISTS "${targetDir}/${name}")
        message(STATUS "Installing dependency '${name}' (${config})")
        configure_file("${file}" "${targetDir}/${name}" COPYONLY)
    endif()
    set(suffix)
    if("${file}" MATCHES "d\.dll$")
        set(suffix "d")
    endif()
    if(WIN32 AND "${lib}" STREQUAL "Qt5::Gui")
        qt_install_win32_plugins("${suffix}" "${config}" "${targetDir}" "${file}"
            iconengines/qsvgicon
            imageformats/qgif
            imageformats/qicns
            imageformats/qico
            imageformats/qjpeg
            imageformats/qsvg
            imageformats/qtga
            imageformats/qtiff
            imageformats/qwbmp
            imageformats/qwebp
            platforms/qdirect2d
            platforms/qwindows
            styles/qwindowsvistastyle
            )
    endif()
    if(WIN32 AND "${lib}" STREQUAL "Qt5::Multimedia")
        qt_install_win32_plugins("${suffix}" "${config}" "${targetDir}" "${file}"
            audio/qtaudio_wasapi
            audio/qtaudio_windows
            mediaservice/dsengine
            mediaservice/qtmedia_audioengine
            mediaservice/wmfengine
            )
    endif()
endmacro()

macro(qt_install_libraries target outputDir)
    foreach(name ${ARGN})
        set(lib "Qt5::${name}")
        if(MSVC OR MINGW)
            if(MSVC)
                foreach(config ${CMAKE_CONFIGURATION_TYPES})
                    qt_install_library("${config}" "${outputDir}/${config}" "${lib}")
                endforeach()
            elseif(MINGW)
                qt_install_library("${CMAKE_BUILD_TYPE}" "${outputDir}" "${lib}")
            endif()
        endif()
        target_link_libraries("${target}" PRIVATE "${lib}")
    endforeach()
endmacro()

macro(install_resources target installDir outputDir)
    foreach(file ${ARGN})
        get_filename_component(name "${file}" NAME)
        get_filename_component(path "${file}" ABSOLUTE)
        if(MSVC)
            foreach(config ${CMAKE_CONFIGURATION_TYPES})
                add_custom_command(OUTPUT "${installDir}/${config}/${outputDir}/${name}"
                    COMMAND "${CMAKE_COMMAND}" -E copy "${path}" "${installDir}/${config}/${outputDir}/${name}"
                    MAIN_DEPENDENCY "${path}"
                    DEPENDS "${path}"
                    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
                    COMMENT "Installing resource '${file}' (${config})"
                    )
                source_group("Generated Files\\${config}" FILES "${installDir}/${config}/${outputDir}/${name}")
                set_source_files_properties("${installDir}/${config}/${outputDir}/${name}" PROPERTIES HEADER_FILE_ONLY TRUE)
                target_sources("${target}" PRIVATE "${installDir}/${config}/${outputDir}/${name}")
            endforeach()
        else()
            add_custom_command(OUTPUT "${installDir}/${outputDir}/${name}"
                COMMAND "${CMAKE_COMMAND}" -E copy "${path}" "${installDir}/${outputDir}/${name}"
                MAIN_DEPENDENCY "${path}"
                DEPENDS "${path}"
                WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
                COMMENT "Installing resource '${file}'"
                )
            source_group("Generated Files" FILES "${installDir}/${outputDir}/${name}")
            set_source_files_properties("${installDir}/${outputDir}/${name}" PROPERTIES HEADER_FILE_ONLY TRUE)
            target_sources("${target}" PRIVATE "${installDir}/${outputDir}/${name}")
        endif()
    endforeach()
endmacro()

#######################################################################################################################

macro(add target type)
    set(noValue
        "CONSOLE"
        )

    set(oneValue
        "FOLDER"
        "INSTALL_RESOURCES_DIR"
        "OUTPUT_DIR"
        )

    set(multiValue
        "SOURCES"
        "LIBS"
        "USES"
        "DEPENDS"
        "PRIVATE_DEFINES"
        "DEFINES"
        "PRIVATE_INCLUDE_DIRS"
        "INCLUDE_DIRS"
        "INSTALL_RESOURCES"
        )

    cmake_parse_arguments(ARG "${noValue}" "${oneValue}" "${multiValue}" ${ARGN})

    if(ARG_UNPARSED_ARGUMENTS)
        message(FATAL_ERROR "Unknown arguments \"${ARG_UNPARSED_ARGUMENTS}\".")
    endif()

    if(NOT ARG_OUTPUT_DIR)
        get_filename_component(buildName "${CMAKE_BINARY_DIR}" NAME)
        set(ARG_OUTPUT_DIR "${BINARIES_PATH}/${buildName}")
    endif()

    #########################################################################################################
    ## Target

    set(qt FALSE)

    if("${type}" STREQUAL "EXECUTABLE")
        add_executable("${target}")
        if(MINGW)
            mingw_install_phtread_library("${ARG_OUTPUT_DIR}")
            target_link_libraries("${target}" PRIVATE mingw32)
        endif()
        if(NOT ARG_CONSOLE)
            set_target_properties("${target}" PROPERTIES WIN32_EXECUTABLE TRUE MACOSX_BUNDLE TRUE)
        endif()
    elseif("${type}" STREQUAL "QT_EXECUTABLE")
        set(qt TRUE)
        add_executable("${target}")
        target_include_directories("${target}" PRIVATE "${CMAKE_CURRENT_BINARY_DIR}")
        target_link_libraries("${target}" PRIVATE ${CMAKE_THREAD_LIBS_INIT})
        if(MINGW)
            mingw_install_libraries("${ARG_OUTPUT_DIR}")
            target_link_libraries("${target}" PRIVATE mingw32)
        endif()
        if(NOT ARG_CONSOLE)
            set_target_properties("${target}" PROPERTIES
                WIN32_EXECUTABLE TRUE
                MACOSX_BUNDLE TRUE
                MACOSX_BUNDLE_INFO_PLIST "${BASE_PATH}/CMake/Info.plist.in"
                )
        endif()
    elseif("${type}" STREQUAL "QT_SHARED_LIBRARY")
        set(qt TRUE)
        add_library("${target}" SHARED)
        target_include_directories("${target}" PRIVATE "${CMAKE_CURRENT_BINARY_DIR}")
        if(MINGW)
            mingw_install_libraries("${ARG_OUTPUT_DIR}")
            target_link_libraries("${target}" PRIVATE mingw32)
        endif()
    elseif("${type}" STREQUAL "QT_STATIC_LIBRARY")
        set(qt TRUE)
        cmake_policy(SET CMP0063 NEW) # Honor visibility for all target types
        add_library("${target}" STATIC)
        target_include_directories("${target}" PRIVATE "${CMAKE_CURRENT_BINARY_DIR}")
    elseif("${type}" STREQUAL "STATIC_LIBRARY")
        cmake_policy(SET CMP0063 NEW) # Honor visibility for all target types
        add_library("${target}" STATIC)
    elseif("${type}" STREQUAL "HEADER_ONLY_LIBRARY")
        if(NOT EXISTS "${CMAKE_CURRENT_BINARY_DIR}/${target}.c")
            string(MAKE_C_IDENTIFIER "${target}" identifier)
            file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/${target}.c" "void headeronlylibrary_${identifier}_() {}\n")
        endif()
        source_group("Generated Files" FILES "${CMAKE_CURRENT_BINARY_DIR}/${target}.c")
        add_library("${target}" STATIC "${CMAKE_CURRENT_BINARY_DIR}/${target}.c")
    else()
        message(FATAL_ERROR "Invalid target type '${type}'.")
    endif()

    #########################################################################################################
    ## Target properties

    set_target_properties("${target}" PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${ARG_OUTPUT_DIR}"
        LIBRARY_OUTPUT_DIRECTORY "${ARG_OUTPUT_DIR}"
        )

    if(ARG_FOLDER)
        set_target_properties("${target}" PROPERTIES
            FOLDER "${ARG_FOLDER}"
            )
    endif()

    #########################################################################################################
    ## Include directories

    if(ARG_INCLUDE_DIRS)
        target_include_directories("${target}" PUBLIC ${ARG_INCLUDE_DIRS})
    endif()

    if(ARG_PRIVATE_INCLUDE_DIRS)
        target_include_directories("${target}" PRIVATE ${ARG_PRIVATE_INCLUDE_DIRS})
    endif()

    #########################################################################################################
    ## Defines

    if(ARG_DEFINES)
        target_compile_definitions("${target}" PUBLIC ${ARG_DEFINES})
    endif()

    if(ARG_PRIVATE_DEFINES)
        target_compile_definitions("${target}" PRIVATE ${ARG_PRIVATE_DEFINES})
    endif()

    #########################################################################################################
    ## Link libraries

    if(ARG_LIBS)
        target_link_libraries("${target}" PUBLIC ${ARG_LIBS})
    endif()

    if(CMAKE_SYSTEM_NAME MATCHES "Linux")
        find_library(LIBM m)
        if(LIBM)
            target_link_libraries("${target}" PUBLIC ${LIBM})
        endif()
    endif()

    if(ARG_USES)
        find_package(Qt5 COMPONENTS ${ARG_USES} REQUIRED)
        qt_install_libraries("${target}" "${ARG_OUTPUT_DIR}" ${ARG_USES})
    endif()

    target_link_libraries("${target}" PUBLIC ${CMAKE_DL_LIBS})

    #########################################################################################################
    ## Dependencies

    if(ARG_DEPENDS)
        add_dependencies("${target}" ${ARG_DEPENDS})
    endif()

    #########################################################################################################
    ## Install files

    if(NOT ARG_INSTALL_RESOURCES_DIR)
        set(ARG_INSTALL_RESOURCES_DIR ".")
    endif()

    if(ARG_INSTALL_RESOURCES)
        install_resources("${target}" "${ARG_OUTPUT_DIR}" "${ARG_INSTALL_RESOURCES_DIR}" ${ARG_INSTALL_RESOURCES})
    endif()

    #########################################################################################################
    ## Source files

    set(src
        ${ARG_SOURCES}
        )

    set(gen)
    set(moc)
    set(rcc)

    foreach(file ${src})
        get_filename_component(ext "${file}" EXT)
        set_source_file_group("${file}")

        get_property(header_only SOURCE "${file}" PROPERTY HEADER_FILE_ONLY)
        if(header_only)
            continue()
        endif()

        if(qt)
            if(ext STREQUAL ".h")
                file(READ "${file}" contents)
                if(contents MATCHES "\n[ \t]*Q_OBJECT[ \t]*\r?\n")
                    qt5_wrap_cpp(moc "${file}")
                endif()
            elseif(ext STREQUAL ".ui")
                qt5_wrap_ui(gen "${file}")
            elseif(ext STREQUAL ".qrc")
                qt5_add_resources(rcc "${file}")
            endif()
        endif()
    endforeach()

    target_sources("${target}" PRIVATE ${src})

    if(qt)
        list(LENGTH moc num1)
        list(LENGTH rcc num2)
        if("${num1}" GREATER 0 OR "${num2}" GREATER 0)
            set(out)
            foreach(file ${moc})
                get_filename_component(full "${file}" ABSOLUTE)
                set(out "${out}#include \"${full}\"\n")
            endforeach()

            set(gen_cpp "${CMAKE_CURRENT_BINARY_DIR}/generated.cpp")
            set(dowrite TRUE)
            if(EXISTS "${gen_cpp}")
                file(READ "${gen_cpp}" old)
                if("${old}" STREQUAL "${out}")
                    set(dowrite FALSE)
                endif()
            endif()
            if(dowrite)
                file(WRITE "${gen_cpp}" "${out}")
            endif()

            set_source_files_properties(${moc} PROPERTIES HEADER_FILE_ONLY TRUE)
            list(APPEND gen "${gen_cpp}")
        endif()

        source_group("Generated Files" FILES ${gen} ${moc} ${rcc})
        target_sources("${target}" PRIVATE ${gen} ${moc} ${rcc})
    endif()
endmacro()

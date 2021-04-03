
# Arguments:
# SRCPATH = path to classes
# DSTPATH = output file

get_filename_component(SRCPATH "${SRCPATH}" ABSOLUTE)
get_filename_component(DSTPATH "${DSTPATH}" ABSOLUTE)

file(MAKE_DIRECTORY "${DSTPATH}")
file(MAKE_DIRECTORY "${DSTPATH}/ClassBytes")

get_filename_component(basePath "${CMAKE_CURRENT_LIST_FILE}" ABSOLUTE)
get_filename_component(basePath "${basePath}" DIRECTORY)
include("${basePath}/../../CMake/Bin2C.cmake")

macro(writeIfChanged outfile newContents)
    get_filename_component(name "${outfile}" NAME)
    if(EXISTS "${outfile}")
        file(READ "${outfile}" oldContents)
        if("${oldContents}" STREQUAL "${newContents}")
            message(STATUS "Keeping ${name}")
            file(TOUCH_NOCREATE "${outfile}")
        else()
            message(STATUS "Writing ${name}")
            file(WRITE "${outfile}" "${newContents}")
        endif()
    else()
        message(STATUS "Writing ${name}")
        file(WRITE "${outfile}" "${newContents}")
    endif()
endmacro()

file(GLOB_RECURSE classes
    RELATIVE "${SRCPATH}"
    "${SRCPATH}/*.class"
    )

set(out "")
set(out "${out}#ifndef COMPILER_JAVA_JAVACLASSES_H\n")
set(out "${out}#define COMPILER_JAVA_JAVACLASSES_H\n")
set(out "${out}\n")
set(out "${out}#include \"Compiler/Java/JavaStdClasses.h\"\n")
set(out "${out}\n")
set(out "${out}class JavaClasses : public JavaStdClasses\n")
set(out "${out}{\n")
set(out "${out}public:\n")

set(cxx "")
set(ini "")
set(fin "")
set(exp "")

foreach(fileName ${classes})
    # Get file name length
    string(LENGTH "${fileName}" fileNameLength)

    # Strip ".class" from the end
    math(EXPR fileNameNewLength "${fileNameLength} - 6")
    string(SUBSTRING "${fileName}" 0 "${fileNameNewLength}" className)

    # Replace '/' and '$' with '_' for symbol name
    string(REPLACE "/" "_" classSymbol "${className}")
    string(REPLACE "$" "_" classSymbol "${classSymbol}")

    set(out "${out}    static JNIClassRef ${classSymbol};\n")
    set(ini "${ini}    ${classSymbol}.defineGlobal(\"${className}\", ${classSymbol}_bytes, ${classSymbol}_len);\n")
    set(fin "${fin}    ${classSymbol}.release();\n")
    set(exp "${exp}    JavaStdClasses::write(path, \"${className}\", ${classSymbol}_bytes, ${classSymbol}_len);\n")
    set(cxx "${cxx}JNIClassRef JavaClasses::${classSymbol};\n")
    set(cxx "${cxx}namespace {\n")
    set(cxx "${cxx}#include \"Compiler/Java/ClassBytes/${classSymbol}.h\"\n")
    set(cxx "${cxx}}\n\n")

    get_filename_component(inPath "${SRCPATH}/${fileName}" ABSOLUTE)
    get_filename_component(outPath "${DSTPATH}/${classSymbol}.h" ABSOLUTE)
    if("${inPath}" IS_NEWER_THAN "${outPath}" OR "${CMAKE_CURRENT_LIST_FILE}" IS_NEWER_THAN "${outPath}")
        set(bin2c_output "")
        do_bin2c("${classSymbol}" "${inPath}")
        writeIfChanged("${DSTPATH}/ClassBytes/${classSymbol}.h" "${bin2c_output}")
    endif()
endforeach()

set(out "${out}\n")
set(out "${out}    static void ensureLoaded();\n")
set(out "${out}    static void releaseAll();\n")
set(out "${out}\n")
set(out "${out}    static void write(const std::filesystem::path& path);\n")
set(out "${out}    static void write(const std::filesystem::path& path, const char* className, const void* bytes, size_t size);\n")
set(out "${out}};\n")
set(out "${out}\n")
set(out "${out}#ifdef JAVACLASSES_IMPL\n\n")
set(out "${out}${cxx}")
set(out "${out}void JavaClasses::ensureLoaded()\n")
set(out "${out}{\n")
set(out "${out}    JavaStdClasses::ensureLoaded();\n")
set(out "${out}${ini}")
set(out "${out}}\n\n")
set(out "${out}void JavaClasses::releaseAll()\n")
set(out "${out}{\n")
set(out "${out}    JavaStdClasses::releaseAll();\n")
set(out "${out}${fin}")
set(out "${out}}\n\n")
set(out "${out}void JavaClasses::write(const std::filesystem::path& path)\n")
set(out "${out}{\n")
set(out "${out}    JavaStdClasses::write(path);\n")
set(out "${out}${exp}")
set(out "${out}}\n\n")
set(out "${out}void JavaClasses::write(const std::filesystem::path& path, const char* className, const void* bytes, size_t size)\n")
set(out "${out}{\n")
set(out "${out}    JavaStdClasses::write(path, className, bytes, size);\n")
set(out "${out}}\n\n")
set(out "${out}#endif\n")
set(out "${out}\n")
set(out "${out}#endif\n")

writeIfChanged("${DSTPATH}/JavaClasses.h" "${out}")

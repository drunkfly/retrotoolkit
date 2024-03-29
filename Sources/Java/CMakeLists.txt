
#######################################################################################################################
# Find JDK

if(WIN32 AND EXISTS "C:/Program Files/Java/jdk1.5.0_22/bin/javac.exe")
    set(JAVA_HOME "C:/Program Files/Java/jdk1.5.0_22")
elseif(WIN32 AND EXISTS "C:/Program Files (x86)/Java/jdk1.5.0_22/bin/javac.exe")
    set(JAVA_HOME "C:/Program Files (x86)/Java/jdk1.5.0_22")
elseif(NOT WIN32 AND EXISTS "/usr/lib/jvm/zulu-6-amd64")
    set(JAVA_HOME "/usr/lib/jvm/zulu-6-amd64")
elseif(NOT WIN32 AND EXISTS "/usr/lib/jvm/java-8-openjdk-amd64")
    set(JAVA_HOME "/usr/lib/jvm/java-8-openjdk-amd64")
endif()

find_package(Java 1.5 COMPONENTS Development Runtime REQUIRED)

get_filename_component(JDK_ROOT "${Java_JAVAC_EXECUTABLE}" REALPATH)
get_filename_component(JDK_ROOT "${JDK_ROOT}" PATH)
get_filename_component(JDK_ROOT "${JDK_ROOT}" PATH)

set(tools_jar "${JDK_ROOT}/lib/tools.jar")
if(NOT EXISTS "${tools_jar}")
    set(tools_jar "${JDK_ROOT}/jmods/jdk.compiler.jmod")
    if(NOT EXISTS "${tools_jar}")
        message(FATAL_ERROR "Neither \"lib/tools.jar\" nor \"jmods/jdk.compiler.jmod\" were found in the JDK.")
    endif()
endif()

#######################################################################################################################
# Setup Ant

if(WIN32)
    set(sep ";")
else()
    set(sep ":")
endif()

set(ANT_HOME "${CMAKE_CURRENT_SOURCE_DIR}/../../Libs/Ant")

set(ANT_CLASSPATH "${ANT_HOME}/lib/ant.jar")
set(ANT_CLASSPATH "${ANT_CLASSPATH}${sep}${ANT_HOME}/lib/ant-launcher.jar")
set(ANT_CLASSPATH "${ANT_CLASSPATH}${sep}${tools_jar}")

#######################################################################################################################
# Compile Java sources

file(GLOB_RECURSE src_java
    CONFIGURE_DEPENDS
    RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}"
        Classpath/*
        Tools/*
    )

foreach(file ${src_java} build.xml)
    set_source_file_group("${file}")
endforeach()

add_custom_target(JavaClasspath
    COMMAND
        "${CMAKE_COMMAND}" -E env "JAVA_HOME=${JDK_ROOT}"
            ${Java_JAVA_EXECUTABLE}
            -classpath "${ANT_CLASSPATH}"
            -Dant.home="${ANT_HOME}"
            -Dant.library.dir="${ANT_HOME}/lib"
            org.apache.tools.ant.launch.Launcher
            -q
    DEPENDS ${src_java} build.xml
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
    COMMENT "Compiling Java sources"
    SOURCES ${src_java} build.xml
    )

set_target_properties(JavaClasspath PROPERTIES
    FOLDER "Java"
    )

set_directory_properties(PROPERTIES
    ADDITIONAL_CLEAN_FILES "${CMAKE_CURRENT_SOURCE_DIR}/Build"
    )

#######################################################################################################################
# Generate class loader

set(JavaClasses_h "${CMAKE_CURRENT_SOURCE_DIR}/Build/_/Compiler/Java/JavaClasses.h")
set_source_files_properties("${JavaClasses_h}" PROPERTIES GENERATED TRUE)
source_group("Generated Files" FILES "${JavaClasses_h}")

set_source_files_properties("${CMAKE_CURRENT_SOURCE_DIR}/Build/RetroBuild.jar" PROPERTIES GENERATED TRUE)

add_custom_command(OUTPUT
        "${JavaClasses_h}"
    COMMAND
        "${CMAKE_COMMAND}"
            "-DSRCPATH=${CMAKE_CURRENT_SOURCE_DIR}/Build/Tools"
            "-DDSTPATH=${CMAKE_CURRENT_SOURCE_DIR}/Build/_/Compiler/Java"
            "-DJDKROOT=${JDK_ROOT}"
            -P "${CMAKE_CURRENT_SOURCE_DIR}/CMakeEmbedClasses.cmake"
    WORKING_DIRECTORY
        "${CMAKE_CURRENT_SOURCE_DIR}"
    MAIN_DEPENDENCY
        CMakeEmbedClasses.cmake
    DEPENDS
        "${CMAKE_CURRENT_SOURCE_DIR}/Build/RetroBuild.jar"
    )

add(JavaClasspathEmbedded
    STATIC_LIBRARY
    FOLDER
        "Java"
    DEPENDS
        JavaClasspath
    LIBS
        Compiler
    INCLUDE_DIRS
        "${CMAKE_CURRENT_SOURCE_DIR}/Build/_"
    SOURCES
        "${JavaClasses_h}"
        CMakeEmbedClasses.cmake
        JavaClasses.cpp
    )

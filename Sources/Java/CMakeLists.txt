
#######################################################################################################################
# Find JDK

if(WIN32 AND EXISTS "C:/Program Files/Java/jdk1.5.0_22/bin/javac.exe")
    set(JAVA_HOME "C:/Program Files/Java/jdk1.5.0_22")
elseif(WIN32 AND EXISTS "C:/Program Files (x86)/Java/jdk1.5.0_22/bin/javac.exe")
    set(JAVA_HOME "C:/Program Files (x86)/Java/jdk1.5.0_22")
endif()

find_package(Java 1.5 COMPONENTS Development Runtime REQUIRED)

get_filename_component(JDK_ROOT "${Java_JAVAC_EXECUTABLE}" PATH)
get_filename_component(JDK_ROOT "${JDK_ROOT}" PATH)

set(tools_jar "${JDK_ROOT}/lib/tools.jar")

if(NOT EXISTS "${tools_jar}")
    message(FATAL_ERROR "File \"${tools_jar}\" was not found.")
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
# Embed some classes into compiler binary

set(src)

macro(embed_class symbol file)
    set(output_cpp "${CMAKE_CURRENT_BINARY_DIR}/${symbol}.cpp")
    source_group("Generated Files" FILES "${output_cpp}")
    set_source_files_properties("${CMAKE_CURRENT_SOURCE_DIR}/${file}" PROPERTIES GENERATED TRUE)
    bin2c("${symbol}" "${CMAKE_CURRENT_SOURCE_DIR}/${file}" "${output_cpp}")
    list(APPEND src "${output_cpp}")
endmacro()

embed_class(JavaOutputWriter "Build/Tools/drunkfly/Messages.class")
embed_class(JavaBuilder "Build/Tools/drunkfly/Builder.class")
embed_class(JavaBuilderClassLoader "Build/Tools/drunkfly/BuilderClassLoader.class")
embed_class(JavaBuilderLauncher "Build/Tools/drunkfly/BuilderLauncher.class")

add(JavaClasspathEmbedded
    STATIC_LIBRARY
    FOLDER
        "Java"
    DEPENDS
        JavaClasspath
    SOURCES
        ${src}
    )

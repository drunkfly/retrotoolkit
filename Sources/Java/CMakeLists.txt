
set(OUTPUTWRITER_CPP "${CMAKE_CURRENT_BINARY_DIR}/JavaOutputWriter.cpp")
source_group("Generated Files" FILES "${OUTPUTWRITER_CPP}")
bin2c(JavaOutputWriter "Build/Tools/drunkfly/Output.class" "${OUTPUTWRITER_CPP}")

add(JavaOutputWriter
    STATIC_LIBRARY
    FOLDER
        "Generated"
    SOURCES
        "${OUTPUTWRITER_CPP}"
    )

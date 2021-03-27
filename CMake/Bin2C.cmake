#################################
if(COMMON_BIN2C_INCLUDED)       #
    return()                    #
endif()                         #
set(COMMON_BIN2C_INCLUDED TRUE) #
#################################

if(BIN2C_IS_TOOL)

    # BIN2C_IS_TOOL=TRUE
    # BIN2C_SYMBOLNAME=<identifier>
    # BIN2C_INFILE=<input file>
    # BIN2C_OUTFILE=<output file>

    file(READ "${BIN2C_INFILE}" data HEX)

    string(LENGTH "${data}" datalen)
    math(EXPR len "${datalen} / 2")
    set(result "\nextern const unsigned ${BIN2C_SYMBOLNAME}_len;\n")
    set(result "${result}extern const unsigned char ${BIN2C_SYMBOLNAME}[];\n")
    set(result "${result}\nconst unsigned ${BIN2C_SYMBOLNAME}_len = ${len};\n")

    if("${datalen}" GREATER "0")
        math(EXPR datalen "${datalen} - 1")
    endif()

    set(result "${result}const unsigned char ${BIN2C_SYMBOLNAME}[] =\n{")
    if("${datalen}" GREATER "0")
        foreach(index RANGE 0 "${datalen}" 2)
            math(EXPR tmp "(${index} / 2) % 14")
            if("${tmp}" EQUAL "0")
                set(result "${result}\n    ")
            endif()
            string(SUBSTRING "${data}" "${index}" 2 hex)
            set(result "${result}0x${hex},")
        endforeach()
    endif()
    set(result "${result}\n    0\n};\n")

    file(WRITE "${BIN2C_OUTFILE}" "${result}")

else()

    get_filename_component(bin2c_cmake "${CMAKE_CURRENT_LIST_FILE}" ABSOLUTE)

    macro(bin2c symbol infile outfile)
        add_custom_command(OUTPUT
                "${outfile}"
            COMMAND
                "${CMAKE_COMMAND}"
                    -DBIN2C_IS_TOOL=TRUE
                    "-DBIN2C_SYMBOLNAME=${symbol}"
                    "-DBIN2C_INFILE=${infile}"
                    "-DBIN2C_OUTFILE=${outfile}"
                    -P "${bin2c_cmake}"
            DEPENDS
                "${infile}"
                "${bin2c_cmake}"
            WORKING_DIRECTORY
                "${CMAKE_CURRENT_SOURCE_DIR}"
        )
    endmacro()

endif()

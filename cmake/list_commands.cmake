function(list_string SUB_COMMAND)
    set(ARGS)
    set(__index 0)

    # Search and Replace
    if ("${SUB_COMMAND}" STREQUAL "FIND")
        set(__items_var ${ARGV1})
        set(__substring ${ARGV2})
        if (ARGC GREATER 3)
            list(SUBLIST ARGV 3 -1 ARGS)
        endif()

        foreach(__item IN LISTS ${__items_var})
            string(${SUB_COMMAND} "${__item}" "${__substring}" __item ${ARGS})
            list(REMOVE_AT ${__items_var} ${__index})
            list(INSERT ${__items_var} ${__index} "${__item}")
            math(EXPR __index "${__index} + 1")
        endforeach()
    elseif ("${SUB_COMMAND}" STREQUAL "REPLACE")
        set(__match_string ${ARGV1})
        set(__replace_string ${ARGV2})
        set(__items_var ${ARGV3})

        foreach(__item IN LISTS ${__items_var})
            string(${SUB_COMMAND} "${__match_string}" "${__replace_string}" __item "${__item}")
            list(REMOVE_AT ${__items_var} ${__index})
            list(INSERT ${__items_var} ${__index} "${__item}")
            math(EXPR __index "${__index} + 1")
        endforeach()
    elseif ("${SUB_COMMAND} ${ARGV1}" STREQUAL "REGEX MATCH" OR "${SUB_COMMAND} ${ARGV1}" STREQUAL "REGEX MATCHALL")
        set(__match_regex ARGV2)
        set(__items_var ${ARGV3})

        foreach(__item IN LISTS ${__items_var})
            string(${SUB_COMMAND} ${ARGV1} "${__match_regex}" __item "${__item}")
            list(REMOVE_AT ${__items_var} ${__index})
            list(INSERT ${__items_var} ${__index} "${__item}")
            math(EXPR __index "${__index} + 1")
        endforeach()

    # Manipulation
    elseif ("${SUB_COMMAND}" STREQUAL "LENGTH" OR "${SUB_COMMAND}" STREQUAL "HASH" OR "${SUB_COMMAND}" STREQUAL "CONFIGURE" OR "${SUB_COMMAND}" STREQUAL "MAKE_C_IDENTIFIER")
        set(__items_var ${ARGV1})
        if (ARGC GREATER 2)
            list(SUBLIST ARGV 2 -1 ARGS)
        endif()


        foreach(__item IN LISTS ${__items_var})
            string(${SUB_COMMAND} "${__item}" __item ${ARGS})
            list(REMOVE_AT ${__items_var} ${__index})
            list(INSERT ${__items_var} ${__index} "${__item}")
            math(EXPR __index "${__index} + 1")
        endforeach()
    elseif ("${SUB_COMMAND}" STREQUAL "SUBSTRING")
        set(__items_var ARGV1)
        set(__begin ${ARGV2})
        set(__end ${ARGV3})
        set(__length ${ARGV4})

        foreach(__item IN LISTS ${__items_var})
            string(${SUB_COMMAND} "${__item}" "${__begin}" "${__length}" __item)
            list(REMOVE_AT ${__items_var} ${__index})
            list(INSERT ${__items_var} ${__index} "${__item}")
            math(EXPR __index "${__index} + 1")
        endforeach()
    elseif ("${SUB_COMMAND}" STREQUAL "REPEAT")
        set(__items_var ARGV1)
        set(__count ${ARGV2})

        foreach(__item IN LISTS ${__items_var})
            string(${SUB_COMMAND} "${__item}" "${__count}" __item)
            list(REMOVE_AT ${__items_var} ${__index})
            list(INSERT ${__items_var} ${__index} "${__item}")
            math(EXPR __index "${__index} + 1")
        endforeach()

    # Comparison
    elseif ("${SUB_COMMAND}" STREQUAL "COMPARE")
        set(__op ARGV1)
        set(__items_var ARGV2)
        set(__string2 ARGV3)

        foreach(__item IN LISTS ${__items_var})
            string(${SUB_COMMAND} "${__op}" "${__item}" "${__string2}" __item)
            list(REMOVE_AT ${__items_var} ${__index})
            list(INSERT ${__items_var} ${__index} "${__item}")
            math(EXPR __index "${__index} + 1")
        endforeach()
    endif()

    set(${__items_var} "${${__items_var}}" PARENT_SCOPE)
endfunction()

function(list_set_filename_component __component __items_var)
    set(__index 0)
    foreach(__item IN LISTS ${__items_var})
        get_filename_component(__item "${__item}" "${__component}" ${ARGN})
        list(REMOVE_AT ${__items_var} ${__index})
        list(INSERT ${__items_var} ${__index} "${__item}")
        math(EXPR __index "${__index} + 1")
    endforeach()

    set(${__items_var} "${${__items_var}}" PARENT_SCOPE)
endfunction()

function(list_cmake_convert SUB_COMMAND __items_var)
    set(__index 0)
    foreach(__item IN LISTS ${__items_var})
        file(${SUB_COMMAND} "${__item}" __item ${ARGN})
        list(REMOVE_AT ${__items_var} ${__index})
        list(INSERT ${__items_var} ${__index} "${__item}")
        math(EXPR __index "${__index} + 1")
    endforeach()

    set(${__items_var} "${${__items_var}}" PARENT_SCOPE)
endfunction()

function(list_cmake_relative_path __items_var __directory)
    set(__index 0)
    foreach(__item IN LISTS ${__items_var})
        file(RELATIVE_PATH __item "${__directory}" "${__item}")
        list(REMOVE_AT ${__items_var} ${__index})
        list(INSERT ${__items_var} ${__index} "${__item}")
        math(EXPR __index "${__index} + 1")
    endforeach()

    set(${__items_var} "${${__items_var}}" PARENT_SCOPE)
endfunction()

function(list_cmake_path SUB_COMMAND __items_var)
    set(__index 0)
    foreach(__item IN LISTS ${__items_var})
        cmake_path(${SUB_COMMAND} __item ${ARGN} __item)
        list(REMOVE_AT ${__items_var} ${__index})
        list(INSERT ${__items_var} ${__index} "${__item}")
        math(EXPR __index "${__index} + 1")
    endforeach()

    set(${__items_var} "${${__items_var}}" PARENT_SCOPE)
endfunction()

# To get the longest common path:
# path1 = /a/b/d/e
# path2 = /a/b/g
# RELATIVE_PATH path1 path2 = ../../g
# The common path is /a/b/d/e/../../ = /a/b
function(cmake_path_common_parent output)
    list(LENGTH ARGN ARGN_LEN)

    if (ARGN_LEN EQUAL 0)
        unset(${output})
        return()
    endif()

    if (ARGN_LEN EQUAL 1)
        set(${output} "${ARGV1}")
        return()
    endif()

    list(GET ARGN 0 __longest_common_path)
    list(SUBLIST ARGN 1 -1 ARGS)

    foreach(__path2 IN LISTS ARGS)
        set(__path1 "${__longest_common_path}")

        # __longest_common_path = ../../
        file(RELATIVE_PATH __longest_common_path "${__path1}" "${__path2}")

        # __longest_common_path = ../../
        string(REGEX MATCH "^([./]+)" __longest_common_path "${__longest_common_path}")

        # __longest_common_path = NORMALIZE /a/b/d/e/../../ = /a/b
        cmake_path(SET __longest_common_path NORMALIZE "${__path1}/${__longest_common_path}")
    endforeach()

    # Remove trailling slash
    string(REGEX REPLACE "/$" "" __longest_common_path "${__longest_common_path}")

    if ("x${__longest_common_path}" STREQUAL "x")
        set(__longest_common_path "/")
    endif()

    set(${output} "${__longest_common_path}" PARENT_SCOPE)
endfunction()

function(list_double_quote __items_var)
    set(__index 0)
    foreach(__item IN LISTS ${__items_var})
        string(REPLACE "\"" "\\\"" __item "${__item}")
        list(REMOVE_AT ${__items_var} ${__index})
        list(INSERT ${__items_var} ${__index} "\"${__item}\"")
        math(EXPR __index "${__index} + 1")
    endforeach()

    set(${__items_var} "${${__items_var}}" PARENT_SCOPE)
endfunction()

function(json_string_encode out_var string)
    string(JSON tmp_json SET "{}" "${string}" "0")
    string(REGEX REPLACE "^\\{[ \t\r\n]*" "" tmp_json "${tmp_json}")
    string(REGEX REPLACE "[ \t\r\n]*:[ \t\r\n]*0[ \t\r\n]*\\}$" "" tmp_json "${tmp_json}")
    if(NOT "${tmp_json}" MATCHES "^\"[^\n]*\"")
        message(FATAL_ERROR "Internal error: unexpected output: '${tmp_json}'")
    endif()
    set(${out_var} "${tmp_json}" PARENT_SCOPE)
endfunction()

function(list_json_string_encode __items_var)
    set(__index 0)
    foreach(__item IN LISTS ${__items_var})
        json_string_encode(__item "${__item}")
        list(REMOVE_AT ${__items_var} ${__index})
        list(INSERT ${__items_var} ${__index} "${__item}")
        math(EXPR __index "${__index} + 1")
    endforeach()

    set(${__items_var} "${${__items_var}}" PARENT_SCOPE)
endfunction()

function(list_to_json_array __items_var)
    set(options)
    set(oneValueArgs OUTPUT_VARIABLE INDENT)
    set(multiValueArgs)
    cmake_parse_arguments(PARSE_ARGV 1 list
        "${options}" "${oneValueArgs}" "${multiValueArgs}"
    )

    if (list_UNPARSED_ARGUMENTS)
        string(REPLACE ";" ", " list_UNPARSED_ARGUMENTS "${list_UNPARSED_ARGUMENTS}")
        message(FATAL_ERROR "Unknown arguments [${list_UNPARSED_ARGUMENTS}]")
    endif()

    if (NOT list_OUTPUT_VARIABLE)
        set(list_OUTPUT_VARIABLE ${__items_var})
    endif()

    list_json_string_encode(${__items_var})

    if (NOT list_INDENT)
        string(REPLACE ";" ", " list_OUTPUT "${${__items_var}}")
        set(${list_OUTPUT_VARIABLE} "[${list_OUTPUT}]" PARENT_SCOPE)
        return()
    endif()

    list(TRANSFORM ${__items_var} PREPEND "    ${list_INDENT}")
    list(TRANSFORM ${__items_var} APPEND ",")
    list(PREPEND ${__items_var} "[")
    list(APPEND ${__items_var} "${list_INDENT}]")
    string(REPLACE ";" "\n" list_OUTPUT "${${__items_var}}")

    set(${list_OUTPUT_VARIABLE} "${list_OUTPUT}" PARENT_SCOPE)
endfunction()

function(list_print __items_var)
    set(__title "${__items_var}")
    if (ARGC GREATER 1)
    set(__title "${ARGV1}")
    endif()

    set(__glue ",\n   ")
    if (ARGC GREATER 2)
        set(__glue "${ARGV2}")
    endif()

    list(JOIN ${__items_var} "${__glue}" __result)
    message(STATUS "${__title}: ${__result}")
endfunction()

function(list_intersection output_variable list_var1 list_var2)
    list(APPEND list1_minus_list2 ${${list_var1}})
    list(REMOVE_ITEM list1_minus_list2 ${${list_var2}})

    list(APPEND list2_minus_list1 ${${list_var2}})
    list(REMOVE_ITEM list2_minus_list1 ${${list_var1}})

    list(APPEND inter_list ${${list_var1}} ${${list_var2}})
    list(REMOVE_DUPLICATES inter_list)
    list(REMOVE_ITEM inter_list ${list1_minus_list2} ${list2_minus_list1})

    set(${output_variable} ${inter_list} PARENT_SCOPE)
endfunction()

function(list_get_duplicates __items_var)
    set(options)
    set(oneValueArgs OUTPUT_VARIABLE)
    set(multiValueArgs)
    cmake_parse_arguments(PARSE_ARGV 1 list
        "${options}" "${oneValueArgs}" "${multiValueArgs}"
    )

    if (list_UNPARSED_ARGUMENTS)
        string(REPLACE ";" ", " list_UNPARSED_ARGUMENTS "${list_UNPARSED_ARGUMENTS}")
        message(FATAL_ERROR "Unknown arguments [${list_UNPARSED_ARGUMENTS}]")
    endif()

    if (NOT list_OUTPUT_VARIABLE)
        set(list_OUTPUT_VARIABLE ${__items_var})
    endif()


    set(list_ENTRIES ${${__items_var}})
    list(SORT list_ENTRIES)
    set(list_OUTPUT)
    list(GET list_ENTRIES 0 lisit_PREV_ITEM)
    list(POP_FRONT list_ENTRIES)

    foreach (list_NEXT_ITEM IN LISTS list_ENTRIES)
        if (list_NEXT_ITEM EQUAL lisit_PREV_ITEM)
            list(APPEND list_OUTPUT ${list_NEXT_ITEM})
        else()
            set(lisit_PREV_ITEM ${list_NEXT_ITEM})
        endif()
    endforeach()

    list(REMOVE_DUPLICATES list_OUTPUT)
    set(${list_OUTPUT_VARIABLE} "${list_OUTPUT}" PARENT_SCOPE)
endfunction()

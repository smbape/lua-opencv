cmake_minimum_required(VERSION 3.25)

set(files_to_change
    "${CMAKE_INSTALL_PREFIX}/${CMAKE_CONFIG_DESTINATION}/${EXPORT_NAME}.cmake"
)

foreach(build_type IN ITEMS debug release)
    set(filename "${CMAKE_INSTALL_PREFIX}/${CMAKE_CONFIG_DESTINATION}/${EXPORT_NAME}-${build_type}.cmake")
    if (EXISTS "${filename}")
        list(APPEND files_to_change "${filename}")
    endif()
endforeach()

cmake_path(RELATIVE_PATH LIBRAY_DESTINATION BASE_DIRECTORY "${CMAKE_CONFIG_DESTINATION}" OUTPUT_VARIABLE _IMPORT_PREFIX)

foreach(filename IN LISTS files_to_change)
    file(READ "${filename}" old_contents)

    set(new_contents "${old_contents}")

    # replace get_filename_component(_IMPORT_PREFIX "${CMAKE_CURRENT_LIST_FILE}" PATH)
    # with    cmake_path(SET _IMPORT_PREFIX NORMALIZE "${CMAKE_CURRENT_LIST_DIR}/${_IMPORT_PREFIX}")
    set(match "get_filename_component(_IMPORT_PREFIX \"\${CMAKE_CURRENT_LIST_FILE}\" PATH)")
    set(replace "cmake_path(SET _IMPORT_PREFIX NORMALIZE \"\${CMAKE_CURRENT_LIST_DIR}/${_IMPORT_PREFIX}\")\nstring(REGEX REPLACE \"/$\" \"\" _IMPORT_PREFIX \"\${_IMPORT_PREFIX}\")")
    string(REPLACE "${match}" "${replace}" new_contents "${new_contents}")

    # remove all get_filename_component(_IMPORT_PREFIX lines
    string(REGEX REPLACE "get_filename_component\\(_IMPORT_PREFIX [^\n]+\n" "" new_contents "${new_contents}")

    # replace ${_IMPORT_PREFIX}/lib/
    # with ${_IMPORT_PREFIX}/
    string(REPLACE "\${_IMPORT_PREFIX}/lib/" "\${_IMPORT_PREFIX}/" new_contents "${new_contents}")

    if (NOT new_contents STREQUAL old_contents)
        file(WRITE "${filename}" "${new_contents}")
    endif()
endforeach()

#
# Build the minilua for host platform
#
if (NOT CMAKE_CROSSCOMPILING)
    add_subdirectory(minilua)
    set(MINILUA_TARGET_FILE $<TARGET_FILE:minilua>)
else()
    if ("${CMAKE_HOST_SYSTEM_NAME}" STREQUAL "Windows")
        set(MINILUA_EXE minilua.exe)
    else()
        set(MINILUA_EXE minilua)
    endif()

    set(minilua_BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}/minilua")
    set(MINILUA_TARGET_FILE "${minilua_BINARY_DIR}/bin/${MINILUA_EXE}")
    file(MAKE_DIRECTORY "${minilua_BINARY_DIR}")

    add_custom_command(OUTPUT "${MINILUA_TARGET_FILE}"
        COMMAND "${CMAKE_COMMAND}" "${CMAKE_CURRENT_SOURCE_DIR}/minilua"
        COMMAND "${CMAKE_COMMAND}" --build .
        WORKING_DIRECTORY "${minilua_BINARY_DIR}"
    )

    add_custom_target(minilua ALL DEPENDS "${MINILUA_TARGET_FILE}")
endif()

cmake_minimum_required(VERSION 3.25)

message(STATUS "PostInstall: TARGET_NAME = \"${TARGET_NAME}\"")
message(STATUS "PostInstall: TARGET_SOURCE_DIR = \"${TARGET_SOURCE_DIR}\"")
message(STATUS "PostInstall: TARGET_CURRENT_SOURCE_DIR = \"${TARGET_CURRENT_SOURCE_DIR}\"")
message(STATUS "PostInstall: TARGET_CURRENT_BINARY_DIR=\"${TARGET_CURRENT_BINARY_DIR}\"")
message(STATUS "PostInstall: DLL_DIRECTORIES=\"${DLL_DIRECTORIES}\"")
message(STATUS "PostInstall: REPAIR_DIR=\"${REPAIR_DIR}\"")

# !!! Do not remove, otherwise you may end up deleting you whole OS files
if (NOT (TARGET_NAME MATCHES "^[A-Za-z0-9_]+$"))
    message(FATAL_ERROR "For security reasons, target name variable cannot be empty and must only contains alpha numeric characters")
endif()

include("${TARGET_SOURCE_DIR}/cmake/list_commands.cmake")

set(PYPROJECT "${TARGET_CURRENT_BINARY_DIR}/pyproject")
file(REMOVE_RECURSE "${PYPROJECT}/")

list(GET PACKAGE_DATA 0 TARGET_FILE_NAME)
list_to_json_array(PACKAGE_DATA)
cmake_path(SET PACKAGE_DIR NORMALIZE "${CMAKE_INSTALL_PREFIX}/${LIBRAY_DESTINATION}")

configure_file("${TARGET_CURRENT_SOURCE_DIR}/setup.py.in" "${PYPROJECT}/setup.py" @ONLY)
configure_file("${TARGET_CURRENT_SOURCE_DIR}/pyproject.toml" "${PYPROJECT}/pyproject.toml" COPYONLY)

list(PREPEND DELVEWHEEL_add_path ${DLL_DIRECTORIES})

execute_process(
    COMMAND "${Python3_EXECUTABLE}" -m build --wheel
    WORKING_DIRECTORY "${PYPROJECT}"
    OUTPUT_VARIABLE WHEEL_BUILD_LOGS
    COMMAND_ECHO STDERR
    OUTPUT_STRIP_TRAILING_WHITESPACE
    COMMAND_ERROR_IS_FATAL ANY
)

# get WHEEL_FILE
string(REGEX MATCH "Successfully built (.+)" WHEEL_FILE "${WHEEL_BUILD_LOGS}")
string(LENGTH "Successfully built " WHEEL_FILE_BEGIN)
string(LENGTH "${WHEEL_FILE}" WHEEL_FILE_LENGTH)
math(EXPR WHEEL_FILE_LENGTH "${WHEEL_FILE_LENGTH} - ${WHEEL_FILE_BEGIN}")
string(SUBSTRING "${WHEEL_FILE}" ${WHEEL_FILE_BEGIN} ${WHEEL_FILE_LENGTH} WHEEL_FILE)

message(STATUS "PostInstall: WHEEL_FILE=\"${WHEEL_FILE}\"")

set(DELVEWHEEL_COMMAND_ARGS
    --analyze-existing
    --exclude "lua*.dll"
    --exclude "concrt*.dll"
    --exclude "GdiPlus.dll"
    --exclude "msvc*.dll"
    --exclude "ucrtbased.dll"
    --exclude "vcruntime*.dll"
)

foreach(flag IN ITEMS ignore_existing ignore_in_wheel no_mangle_all with_mangle strip custom_patch include_symbols include_imports)
    if (DELVEWHEEL_${flag})
        string(REPLACE "_" "-" flag "${flag}")
        list(APPEND DELVEWHEEL_COMMAND_ARGS "--${flag}")
    endif()
endforeach()

foreach(option IN ITEMS add_path include add_dll exclude no_dll wheel_dir no_mangle lib_sdir namespace_pkg)
    # Trim leading/trailing space
    string(REGEX REPLACE "(^[ \t\r\n]*;|;[ \t\r\n]*$)" "" DELVEWHEEL_${option} "${DELVEWHEEL_${option}}")
    foreach(value IN LISTS DELVEWHEEL_${option})
        string(REPLACE "_" "-" option "${option}")
        list(APPEND DELVEWHEEL_COMMAND_ARGS "--${option}" "${value}")
    endforeach()
endforeach()

list(APPEND DELVEWHEEL_COMMAND_ARGS "dist/${WHEEL_FILE}")

# Check that the target file can be repaired
execute_process(
    COMMAND "${Python3_EXECUTABLE}" -m delvewheel show ${DELVEWHEEL_COMMAND_ARGS}
    WORKING_DIRECTORY "${PYPROJECT}"
    OUTPUT_VARIABLE command_OUTPUT
    COMMAND_ECHO STDERR
    COMMAND_ERROR_IS_FATAL ANY
)

string(REGEX MATCHALL "([^ \t\r\n]+) \\(Error: Not Found\\)" repaired_NOT_FOUND "${command_OUTPUT}")
if (repaired_NOT_FOUND)
    string(REPLACE ";" "\n    " repaired_NOT_FOUND "${repaired_NOT_FOUND}")
    message(FATAL_ERROR "${PACKAGE_DATA} cannot not be repaired:\n    ${repaired_NOT_FOUND}")
endif()

# repair the WHEEL_FILE
execute_process(
    COMMAND "${Python3_EXECUTABLE}" -m delvewheel repair ${DELVEWHEEL_COMMAND_ARGS}
    WORKING_DIRECTORY "${PYPROJECT}"
    OUTPUT_VARIABLE command_OUTPUT
    COMMAND_ECHO STDERR
    ERROR_STRIP_TRAILING_WHITESPACE
    COMMAND_ERROR_IS_FATAL ANY
)

if(NOT command_OUTPUT MATCHES "no external dependencies are needed")
    # get repaired_WHEEL_FILE
    string(REGEX MATCH "fixed wheel written to (.+)" repaired_WHEEL_FILE "${command_OUTPUT}")
    string(LENGTH "fixed wheel written to " repaired_WHEEL_FILE_BEGIN)
    string(LENGTH "${repaired_WHEEL_FILE}" repaired_WHEEL_FILE_LENGTH)
    math(EXPR repaired_WHEEL_FILE_LENGTH "${repaired_WHEEL_FILE_LENGTH} - ${repaired_WHEEL_FILE_BEGIN}")
    string(SUBSTRING "${repaired_WHEEL_FILE}" ${repaired_WHEEL_FILE_BEGIN} ${repaired_WHEEL_FILE_LENGTH} repaired_WHEEL_FILE)

    cmake_path(GET repaired_WHEEL_FILE PARENT_PATH repaired_WHEEL_DIR)
    file(GLOB repaired_WHEEL_FILE "${repaired_WHEEL_DIR}/${TARGET_NAME}-${PROJECT_VERSION}-*.whl")

    message(STATUS "PostInstall: repaired_WHEEL_FILE=\"${repaired_WHEEL_FILE}\"")

    # Replace shared library with the repaired one
    execute_process(
        COMMAND unzip -o -d "${REPAIR_DIR}" "${repaired_WHEEL_FILE}"
        WORKING_DIRECTORY "${PYPROJECT}"
        COMMAND_ECHO STDERR
        COMMAND_ERROR_IS_FATAL ANY
    )

    file(REMOVE_RECURSE "${TARGET_CURRENT_BINARY_DIR}/${REPAIR_DIR}/")
    file(MAKE_DIRECTORY "${TARGET_CURRENT_BINARY_DIR}/${REPAIR_DIR}")
    file(GLOB repaired_FILES RELATIVE "${PYPROJECT}/${REPAIR_DIR}" "${PYPROJECT}/${REPAIR_DIR}/${TARGET_NAME}/*" "${PYPROJECT}/${REPAIR_DIR}/${TARGET_NAME}.libs")

    list(REMOVE_ITEM repaired_FILES "${TARGET_NAME}/__init__.py")

    if ("${TARGET_NAME}.libs" IN_LIST repaired_FILES)
        list(REMOVE_ITEM repaired_FILES "${TARGET_NAME}.libs")
        file(REMOVE_RECURSE "${PACKAGE_DIR}/${TARGET_NAME}/libs/")
        file(RENAME "${PYPROJECT}/${REPAIR_DIR}/${TARGET_NAME}.libs" "${PYPROJECT}/${REPAIR_DIR}/libs")
        file(INSTALL TYPE DIRECTORY
            FILES "${PYPROJECT}/${REPAIR_DIR}/libs"
            DESTINATION "${PACKAGE_DIR}/${TARGET_NAME}"
        )
    endif()

    foreach(repaired_FILE IN LISTS repaired_FILES)
        if (IS_DIRECTORY "${PYPROJECT}/${REPAIR_DIR}/${repaired_FILE}")
            set(repaired_FILE_TYPE DIRECTORY)
        else()
            set(repaired_FILE_TYPE FILE)
        endif()
        file(INSTALL TYPE ${repaired_FILE_TYPE}
            FILES "${PYPROJECT}/${REPAIR_DIR}/${repaired_FILE}"
            DESTINATION "${PACKAGE_DIR}"
        )
    endforeach()

elseif(NOT DELVEWHEEL_exclude)
    return()
endif()

configure_file("${TARGET_CURRENT_SOURCE_DIR}/init.lua.in" "${PYPROJECT}/${REPAIR_DIR}/init.lua" @ONLY)
file(INSTALL TYPE FILE
    FILES "${PYPROJECT}/${REPAIR_DIR}/init.lua"
    DESTINATION "${CMAKE_INSTALL_PREFIX}/${LUAMOD_DESTINATION}/${TARGET_NAME}"
)

file(REMOVE_RECURSE "${PYPROJECT}/${REPAIR_DIR}")


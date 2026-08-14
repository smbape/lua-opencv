cmake_minimum_required(VERSION 3.25)

message(STATUS "PostInstall: TARGET_NAME = \"${TARGET_NAME}\"")
message(STATUS "PostInstall: TARGET_SOURCE_DIR = \"${TARGET_SOURCE_DIR}\"")
message(STATUS "PostInstall: TARGET_CURRENT_SOURCE_DIR = \"${TARGET_CURRENT_SOURCE_DIR}\"")
message(STATUS "PostInstall: TARGET_CURRENT_BINARY_DIR=\"${TARGET_CURRENT_BINARY_DIR}\"")
message(STATUS "PostInstall: ORIGIN_FILE=\"${ORIGIN_FILE}\"")
message(STATUS "PostInstall: REPAIR_DIR=\"${REPAIR_DIR}\"")

# !!! Do not remove, otherwise you may end up deleting you whole OS files
if (NOT (TARGET_NAME MATCHES "^[A-Za-z0-9_]+$"))
    message(FATAL_ERROR "For security reasons, target name variable cannot be empty and must only contains alpha numeric characters")
endif()

include("${TARGET_SOURCE_DIR}/cmake/list_commands.cmake")

set(PYPROJECT "${TARGET_CURRENT_BINARY_DIR}/pyproject")
file(REMOVE_RECURSE "${PYPROJECT}/")

set(repaired_PACKAGE_DATA ${PACKAGE_DATA})
list(GET PACKAGE_DATA 0 TARGET_FILE_NAME)
list_to_json_array(PACKAGE_DATA)
cmake_path(SET PACKAGE_DIR NORMALIZE "${CMAKE_INSTALL_PREFIX}/${LIBRAY_DESTINATION}")

configure_file("${TARGET_CURRENT_SOURCE_DIR}/setup.py.in" "${PYPROJECT}/setup.py" @ONLY)
configure_file("${TARGET_CURRENT_SOURCE_DIR}/pyproject.toml" "${PYPROJECT}/pyproject.toml" COPYONLY)

set(TARGET_FILE "${PACKAGE_DIR}/${TARGET_FILE_NAME}")

if (NOT ORIGIN_FILE)
    set(ORIGIN_FILE "${TARGET_FILE}")
endif()

# get LD_LIBRARY_PATH
execute_process(
    COMMAND patchelf --print-rpath "${ORIGIN_FILE}"
    WORKING_DIRECTORY "${PYPROJECT}"
    OUTPUT_VARIABLE library_RPATH
    OUTPUT_STRIP_TRAILING_WHITESPACE
    COMMAND_ERROR_IS_FATAL ANY
)

cmake_path(GET ORIGIN_FILE PARENT_PATH origin_TARGET_FILE)

string(REPLACE "$ORIGIN" "${origin_TARGET_FILE}" LD_LIBRARY_PATH "${library_RPATH}")

string(REPLACE ":" ";" LD_LIBRARY_PATH "${LD_LIBRARY_PATH}")
set(search_PATHS)
foreach(lib_PATH IN LISTS LD_LIBRARY_PATH)
    if (EXISTS "${lib_PATH}")
        list(APPEND search_PATHS "${lib_PATH}")
    endif()
endforeach()

if (search_PATHS)
    execute_process(
        COMMAND find ${search_PATHS} -mindepth 1 -maxdepth 1 -name "*.so*" -exec realpath "{}" ";"
        WORKING_DIRECTORY "${PYPROJECT}"
        OUTPUT_VARIABLE lib_SONAMES
        OUTPUT_STRIP_TRAILING_WHITESPACE
        COMMAND_ERROR_IS_FATAL ANY
    )

    string(REGEX REPLACE "[ \t\r\n]+" ";" lib_SONAMES "${lib_SONAMES}")
    set(search_PATHS ${LD_LIBRARY_PATH})
    foreach(lib_SONAME IN LISTS lib_SONAMES)
        cmake_path(GET lib_SONAME PARENT_PATH lib_SONAME_PARENT_PATH)
        list(APPEND search_PATHS "${lib_SONAME_PARENT_PATH}")
    endforeach()

    list(REMOVE_DUPLICATES search_PATHS)

    if (search_PATHS)
        string(REPLACE ";" ":" LD_LIBRARY_PATH "${search_PATHS}")
    endif()
else()
    string(REPLACE ";" ":" LD_LIBRARY_PATH "${LD_LIBRARY_PATH}")
endif()

message(STATUS "PostInstall: LD_LIBRARY_PATH=\"${LD_LIBRARY_PATH}\"")

# Check that the target file can be repaired
execute_process(
    COMMAND ldd "${TARGET_FILE}"
    WORKING_DIRECTORY "${PYPROJECT}"
    OUTPUT_VARIABLE command_OUTPUT
    COMMAND_ECHO STDERR
    COMMAND_ERROR_IS_FATAL ANY
)
string(REGEX MATCHALL "([^ \t\r\n]+) => not found" repaired_NOT_FOUND_1 "${command_OUTPUT}")

execute_process(
    COMMAND "${CMAKE_COMMAND}" -E env "LD_LIBRARY_PATH=${LD_LIBRARY_PATH}" -- ldd "${TARGET_FILE}"
    WORKING_DIRECTORY "${PYPROJECT}"
    OUTPUT_VARIABLE command_OUTPUT
    COMMAND_ECHO STDERR
    COMMAND_ERROR_IS_FATAL ANY
)
string(REGEX MATCHALL "([^ \t\r\n]+) => not found" repaired_NOT_FOUND_2 "${command_OUTPUT}")

list_intersection(repaired_NOT_FOUND repaired_NOT_FOUND_1 repaired_NOT_FOUND_2)

if (repaired_NOT_FOUND)
    string(REPLACE ";" "\n    " repaired_NOT_FOUND "${repaired_NOT_FOUND}")
    message(WARNING "${TARGET_FILE} may not be repaired:\n    ${repaired_NOT_FOUND}")
endif()

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

set(AUDITWHEEL_COMMAND "${Python3_EXECUTABLE}" -m auditwheel repair --exclude "liblua*")

foreach(flag IN ITEMS strip only_plat disable_isa_ext_check)
    if (AUDITWHEEL_${flag})
        string(REPLACE "_" "-" flag "${flag}")
        list(APPEND AUDITWHEEL_COMMAND "--${flag}")
    endif()
endforeach()

foreach(option IN ITEMS plat exclude)
    # Trim leading/trailing space
    string(REGEX REPLACE "(^[ \t\r\n]*;|;[ \t\r\n]*$)" "" DELVEWHEEL_${option} "${DELVEWHEEL_${option}}")
    foreach(value IN LISTS AUDITWHEEL_${option})
        list(APPEND AUDITWHEEL_COMMAND "--${option}" "${value}")
    endforeach()
endforeach()

list(APPEND AUDITWHEEL_COMMAND "dist/${WHEEL_FILE}")

# repair the WHEEL_FILE
execute_process(
    COMMAND "${CMAKE_COMMAND}" -E env "LD_LIBRARY_PATH=${LD_LIBRARY_PATH}" -- ${AUDITWHEEL_COMMAND}
    WORKING_DIRECTORY "${PYPROJECT}"
    ERROR_VARIABLE command_OUTPUT
    COMMAND_ECHO STDERR
    ERROR_STRIP_TRAILING_WHITESPACE
    COMMAND_ERROR_IS_FATAL ANY
)

# get repaired_WHEEL_FILE
string(REGEX MATCH "Fixed-up wheel written to (.+)" repaired_WHEEL_FILE "${command_OUTPUT}")
string(LENGTH "Fixed-up wheel written to " repaired_WHEEL_FILE_BEGIN)
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

file(REMOVE_RECURSE "${PYPROJECT}/${REPAIR_DIR}")

# Set RPATH to $ORIGIN/${TARGET_NAME}/libs
set(libs_DIRECTORY "${PACKAGE_DIR}/${TARGET_NAME}/libs")
foreach(lib_SONAME IN LISTS repaired_PACKAGE_DATA)
    set(lib_FILEPATH "${PACKAGE_DIR}/${lib_SONAME}")
    cmake_path(GET lib_FILEPATH PARENT_PATH lib_DIRECTORY)
    cmake_path(RELATIVE_PATH libs_DIRECTORY BASE_DIRECTORY "${lib_DIRECTORY}" OUTPUT_VARIABLE lib_ORIGIN)

    message(STATUS "Set non-toolchain portion of runtime path of \"${lib_FILEPATH}\" to \"$ORIGIN/${lib_ORIGIN}:$ORIGIN\"")
    execute_process(
        COMMAND patchelf --force-rpath --set-rpath "$ORIGIN/${lib_ORIGIN}:$ORIGIN" "${PACKAGE_DIR}/${lib_SONAME}"
        WORKING_DIRECTORY "${PYPROJECT}"
        OUTPUT_VARIABLE library_RPATH
        OUTPUT_STRIP_TRAILING_WHITESPACE
        COMMAND_ERROR_IS_FATAL ANY
    )
endforeach()

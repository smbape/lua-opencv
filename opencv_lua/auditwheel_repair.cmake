cmake_minimum_required(VERSION 3.25)

macro(cmake_script_append_var content_var)
    foreach(var_name IN ITEMS ${ARGN})
        set(${content_var} "${${content_var}}
set(${var_name} \"${${var_name}}\")
")
    endforeach()
endmacro()

if (NOT ENABLE_REPAIR)
    get_filename_component(SONAME "${TARGET_FILE}" NAME)

    execute_process(
        COMMAND patchelf --print-rpath "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}/${SONAME}"
        WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
        OUTPUT_VARIABLE LD_LIBRARY_PATH
        OUTPUT_STRIP_TRAILING_WHITESPACE
        COMMAND_ERROR_IS_FATAL ANY
    )

    string(REPLACE ":" ";" LD_LIBRARY_PATH "${LD_LIBRARY_PATH}")
    list(APPEND LD_LIBRARY_PATH "$ORIGIN")
    list(REMOVE_DUPLICATES LD_LIBRARY_PATH)
    string(REPLACE ";" ":" LD_LIBRARY_PATH "${LD_LIBRARY_PATH}")

    execute_process(
        COMMAND patchelf --force-rpath --set-rpath "${LD_LIBRARY_PATH}" "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}/${SONAME}"
        COMMAND_ECHO STDERR
        COMMAND_ERROR_IS_FATAL ANY
    )

    #
    # Save variables that are needed when repairing
    #
    set(repair_CONFIG_SCRIPT "")
    cmake_script_append_var(repair_CONFIG_SCRIPT
        target_name
        PROJECT_VERSION
        Python3_EXECUTABLE
        CMAKE_SOURCE_DIR
        CMAKE_CURRENT_SOURCE_DIR
        CMAKE_CURRENT_BINARY_DIR
        TARGET_FILE
        ORIGIN_FILE
    )
    set(repair_rock_config "${CMAKE_SOURCE_DIR}/luarocks/lua_modules/repair_rock_config.cmake")
    file(WRITE "${repair_rock_config}" "${repair_CONFIG_SCRIPT}")

    return()
endif()

include("${CMAKE_SOURCE_DIR}/luarocks/lua_modules/repair_rock_config.cmake")
include("${CMAKE_SOURCE_DIR}/cmake/list_commands.cmake")

message(STATUS "PostInstall: CMAKE_SOURCE_DIR = \"${CMAKE_SOURCE_DIR}\"")
message(STATUS "PostInstall: CMAKE_CURRENT_SOURCE_DIR = \"${CMAKE_CURRENT_SOURCE_DIR}\"")
message(STATUS "PostInstall: CMAKE_CURRENT_BINARY_DIR = \"${CMAKE_CURRENT_BINARY_DIR}\"")
message(STATUS "PostInstall: CMAKE_INSTALL_PREFIX = \"${CMAKE_INSTALL_PREFIX}\"")
message(STATUS "PostInstall: CMAKE_INSTALL_LIBDIR = \"${CMAKE_INSTALL_LIBDIR}\"")
message(STATUS "PostInstall: TARGET_FILE = \"${TARGET_FILE}\"")
message(STATUS "PostInstall: ORIGIN_FILE = \"${ORIGIN_FILE}\"")
message(STATUS "PostInstall: PACKAGE_DATA = \"${PACKAGE_DATA}\"")
message(STATUS "PostInstall: CMAKE_INSTALL_LIBSDIR = \"${CMAKE_INSTALL_LIBSDIR}\"")
message(STATUS "PostInstall: target_name = \"${target_name}\"")

# !!! Do not remove, otherwise you may end up deleting you whole OS files
if (NOT (target_name MATCHES "^[A-Za-z0-9_]+$"))
    message(FATAL_ERROR "For security reasons, target name variable cannot be empty and must only contains alpha numeric characters")
endif()

# get LD_LIBRARY_PATH
execute_process(
    COMMAND patchelf --print-rpath "${TARGET_FILE}"
    WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
    OUTPUT_VARIABLE library_RPATH
    OUTPUT_STRIP_TRAILING_WHITESPACE
    COMMAND_ERROR_IS_FATAL ANY
)

if (ORIGIN_FILE)
    cmake_path(GET ORIGIN_FILE PARENT_PATH origin_TARGET_FILE)
else()
    cmake_path(GET TARGET_FILE PARENT_PATH origin_TARGET_FILE)
endif()

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
        WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
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

get_filename_component(SONAME "${TARGET_FILE}" NAME)

set(INSTALL_LIBDIR "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}")
set(INSTALL_SONAME "${INSTALL_LIBDIR}/${SONAME}")

message(STATUS "PostInstall: Set non-toolchain portion of runtime path of \"${INSTALL_SONAME}\"")

set(PYPROJECT "${CMAKE_CURRENT_BINARY_DIR}/pyproject")

file(REMOVE_RECURSE "${PYPROJECT}/")

configure_file("${CMAKE_CURRENT_SOURCE_DIR}/setup.py.in" "${PYPROJECT}/setup.py" @ONLY)
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/pyproject.toml" "${PYPROJECT}/pyproject.toml" COPYONLY)

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

message(STATUS "PostInstall: repaired_WHEEL_FILE=\"${repaired_WHEEL_FILE}\"")

set(repaired_DIR "${CMAKE_INSTALL_LIBDIR}/repaired")

# Replace shared library with the repaired one
execute_process(
    COMMAND unzip -o -d "${repaired_DIR}" "${repaired_WHEEL_FILE}"
    WORKING_DIRECTORY "${CMAKE_INSTALL_PREFIX}"
    COMMAND_ECHO STDERR
    COMMAND_ERROR_IS_FATAL ANY
)

file(REMOVE_RECURSE "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBSDIR}")

file(GLOB repaired_FILES RELATIVE "${CMAKE_INSTALL_PREFIX}/${repaired_DIR}" "${CMAKE_INSTALL_PREFIX}/${repaired_DIR}/${target_name}/*" "${CMAKE_INSTALL_PREFIX}/${repaired_DIR}/${target_name}.libs")

unset(COPY_COMMANDS)

if ("${target_name}.libs" IN_LIST repaired_FILES)
    get_filename_component(CMAKE_INSTALL_LIBSDIR_PARENT "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBSDIR}" DIRECTORY)
    file(MAKE_DIRECTORY "${CMAKE_INSTALL_LIBSDIR_PARENT}")
    list(APPEND COPY_COMMANDS COMMAND mv "${repaired_DIR}/${target_name}.libs" "${CMAKE_INSTALL_LIBSDIR}")
    list(REMOVE_ITEM repaired_FILES "${target_name}.libs")
endif()

list(TRANSFORM repaired_FILES PREPEND "${repaired_DIR}/")
list(APPEND COPY_COMMANDS COMMAND cp -rf ${repaired_FILES} "${CMAKE_INSTALL_LIBDIR}/")

execute_process(
    ${COPY_COMMANDS}
    WORKING_DIRECTORY "${CMAKE_INSTALL_PREFIX}"
    COMMAND_ECHO STDERR
    COMMAND_ERROR_IS_FATAL ANY
)

file(REMOVE_RECURSE "${CMAKE_INSTALL_PREFIX}/${repaired_DIR}")

# Set RPATH to $ORIGIN/${target_name}/libs
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/set_rpath.py.in" "${PYPROJECT}/set_rpath.py" @ONLY)
execute_process(
    COMMAND "${Python3_EXECUTABLE}" "${PYPROJECT}/set_rpath.py"
    WORKING_DIRECTORY "${INSTALL_LIBDIR}"
    COMMAND_ECHO STDERR
    COMMAND_ERROR_IS_FATAL ANY
)

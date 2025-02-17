cmake_minimum_required(VERSION 3.25)

macro(cmake_script_append_var content_var)
  foreach(var_name IN ITEMS ${ARGN})
    set(${content_var} "${${content_var}}
set(${var_name} \"${${var_name}}\")
")
  endforeach()
endmacro()

if (NOT ENABLE_REPAIR)
  #
  # Configuration for standalone repair
  #
  set(REPAIR_CONFIG_SCRIPT "")
  cmake_script_append_var(REPAIR_CONFIG_SCRIPT
    target_name
    PROJECT_VERSION
    Python3_EXECUTABLE
    CMAKE_SOURCE_DIR
    CMAKE_CURRENT_SOURCE_DIR
    CMAKE_CURRENT_BINARY_DIR
    TARGET_FILE
  )
  set(repair_rock_config "${CMAKE_SOURCE_DIR}/luarocks/lua_modules/repair_rock_config.cmake")
  file(WRITE "${repair_rock_config}" "${REPAIR_CONFIG_SCRIPT}")

  return()
endif()

include("${CMAKE_SOURCE_DIR}/luarocks/lua_modules/repair_rock_config.cmake")

message(STATUS "PostInstall: CMAKE_SOURCE_DIR = \"${CMAKE_SOURCE_DIR}\"")
message(STATUS "PostInstall: CMAKE_CURRENT_SOURCE_DIR = \"${CMAKE_CURRENT_SOURCE_DIR}\"")
message(STATUS "PostInstall: CMAKE_CURRENT_BINARY_DIR = \"${CMAKE_CURRENT_BINARY_DIR}\"")
message(STATUS "PostInstall: CMAKE_INSTALL_PREFIX = \"${CMAKE_INSTALL_PREFIX}\"")
message(STATUS "PostInstall: CMAKE_INSTALL_LIBDIR = \"${CMAKE_INSTALL_LIBDIR}\"")
message(STATUS "PostInstall: TARGET_FILE = \"${TARGET_FILE}\"")
message(STATUS "PostInstall: PACKAGE_DATA = \"${PACKAGE_DATA}\"")
message(STATUS "PostInstall: CMAKE_INSTALL_LIBSDIR = \"${CMAKE_INSTALL_LIBSDIR}\"")
message(STATUS "PostInstall: target_name = \"${target_name}\"")

# !!! Do not remove, otherwise you may end up deleting you whole OS files
if (NOT (target_name MATCHES "^[A-Za-z0-9_]+$"))
  message(FATAL_ERROR "For security reasons, target name variable cannot be empty and must only contains alpha numeric characters")
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

# get LD_LIBRARY_PATH
execute_process(
  COMMAND readelf -d "${TARGET_FILE}"
  WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
  OUTPUT_VARIABLE ELF_SYMBOLS
  COMMAND_ECHO STDERR
  OUTPUT_STRIP_TRAILING_WHITESPACE
  COMMAND_ERROR_IS_FATAL ANY
)

string(REGEX MATCH "Library r(un)?path: \\[([^]]+)\\]" LD_LIBRARY_PATH "${ELF_SYMBOLS}")
set(LD_LIBRARY_PATH "${CMAKE_MATCH_2}")

message(STATUS "PostInstall: LD_LIBRARY_PATH=\"${LD_LIBRARY_PATH}\"")

set(ENV{LD_LIBRARY_PATH} "${LD_LIBRARY_PATH}")

execute_process(
  COMMAND "${Python3_EXECUTABLE}" -m auditwheel repair "dist/${WHEEL_FILE}"
  WORKING_DIRECTORY "${PYPROJECT}"
  ERROR_VARIABLE AUDITWHEEL_BUILD_LOGS
  COMMAND_ECHO STDERR
  ERROR_STRIP_TRAILING_WHITESPACE
  COMMAND_ERROR_IS_FATAL ANY
)

# get REPAIRED_WHEEL_FILE
string(REGEX MATCH "Fixed-up wheel written to (.+)" REPAIRED_WHEEL_FILE "${AUDITWHEEL_BUILD_LOGS}")
string(LENGTH "Fixed-up wheel written to " REPAIRED_WHEEL_FILE_BEGIN)
string(LENGTH "${REPAIRED_WHEEL_FILE}" REPAIRED_WHEEL_FILE_LENGTH)
math(EXPR REPAIRED_WHEEL_FILE_LENGTH "${REPAIRED_WHEEL_FILE_LENGTH} - ${REPAIRED_WHEEL_FILE_BEGIN}")
string(SUBSTRING "${REPAIRED_WHEEL_FILE}" ${REPAIRED_WHEEL_FILE_BEGIN} ${REPAIRED_WHEEL_FILE_LENGTH} REPAIRED_WHEEL_FILE)

message(STATUS "PostInstall: REPAIRED_WHEEL_FILE=\"${REPAIRED_WHEEL_FILE}\"")

set(REPAIRED_DIR "${CMAKE_INSTALL_LIBDIR}/repaired")
set(FILE_LIST "${target_name}/*")

execute_process(
  COMMAND unzip -l "${REPAIRED_WHEEL_FILE}" "${target_name}.libs/*"
  WORKING_DIRECTORY "${CMAKE_INSTALL_PREFIX}"
  RESULT_VARIABLE HAS_LIBS_ERROR
  OUTPUT_QUIET
  ERROR_QUIET
)

if(HAS_LIBS_ERROR EQUAL "0")
    list(APPEND FILE_LIST "${target_name}.libs/*")
endif()

# Replace shared library with the repaired one
execute_process(
  COMMAND unzip -o -d "${REPAIRED_DIR}" "${REPAIRED_WHEEL_FILE}" ${FILE_LIST}
  WORKING_DIRECTORY "${CMAKE_INSTALL_PREFIX}"
  COMMAND_ECHO STDERR
  COMMAND_ERROR_IS_FATAL ANY
)

file(REMOVE_RECURSE "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBSDIR}")

set(COPY_COMMANDS COMMAND cp -f "${REPAIRED_DIR}/${target_name}/${SONAME}" "${CMAKE_INSTALL_LIBDIR}/")

if (EXISTS "${REPAIRED_DIR}/${target_name}/${target_name}")
  list(APPEND COPY_COMMANDS COMMAND cp -rf "${REPAIRED_DIR}/${target_name}/${target_name}" "${CMAKE_INSTALL_LIBDIR}/")
endif()

if(HAS_LIBS_ERROR EQUAL "0")
  get_filename_component(CMAKE_INSTALL_LIBSDIR_PARENT "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBSDIR}" DIRECTORY)
  file(MAKE_DIRECTORY "${CMAKE_INSTALL_LIBSDIR_PARENT}")
  list(APPEND COPY_COMMANDS COMMAND mv "${REPAIRED_DIR}/${target_name}.libs" "${CMAKE_INSTALL_LIBSDIR}")
endif()

execute_process(
  ${COPY_COMMANDS}
  WORKING_DIRECTORY "${CMAKE_INSTALL_PREFIX}"
  COMMAND_ECHO STDERR
  COMMAND_ERROR_IS_FATAL ANY
)

file(REMOVE_RECURSE "${CMAKE_INSTALL_PREFIX}/${REPAIRED_DIR}")

# Set RPATH to $ORIGIN/${target_name}/libs
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/set_rpath.py.in" "${PYPROJECT}/set_rpath.py" @ONLY)
execute_process(
  COMMAND "${Python3_EXECUTABLE}" "${PYPROJECT}/set_rpath.py"
  WORKING_DIRECTORY "${INSTALL_LIBDIR}"
  COMMAND_ECHO STDERR
  COMMAND_ERROR_IS_FATAL ANY
)

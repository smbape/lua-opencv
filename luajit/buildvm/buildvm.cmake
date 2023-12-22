#
# Build the buildvm for host platform
#

set(DASM_DASC vm_${DASM_ARCH}.dasc)

file(MAKE_DIRECTORY "${LUA_INCLUDE_DIR}/host")
set(BUILDVM_ARCH_HEADER "${LUA_INCLUDE_DIR}/host/buildvm_arch.h")
file(TO_NATIVE_PATH "${DASMDIR}/dynasm.lua" DYNASM)
file(TO_NATIVE_PATH "${BUILDVM_ARCH_HEADER}" BUILDVM_ARCH_HEADER_NATIVE)
add_custom_command(
    OUTPUT "${BUILDVM_ARCH_HEADER}"
    COMMAND "${MINILUA_TARGET_FILE}" "${DYNASM}" -LN ${DASM_FLAGS} -o "${BUILDVM_ARCH_HEADER_NATIVE}" ${DASM_DASC}
    WORKING_DIRECTORY "${luajit_SRC}"
    DEPENDS minilua
    COMMENT "Generate buildvm_arch.h"
)
add_custom_target(gen_buildvm_arch ALL DEPENDS minilua "${BUILDVM_ARCH_HEADER}" "${luajit_SRC}/luajit.h")

if (NOT CMAKE_CROSSCOMPILING)
  add_subdirectory(buildvm)
  set(BUILDVM_TARGET_FILE $<TARGET_FILE:buildvm>)
else()
  if ("${CMAKE_HOST_SYSTEM_NAME}" STREQUAL "Windows")
    set(BUILDVM_EXE buildvm.exe)
  else()
    set(BUILDVM_EXE buildvm)
  endif()

  set(buildvm_BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}/buildvm")
  set(BUILDVM_TARGET_FILE "${buildvm_BINARY_DIR}/bin/${BUILDVM_EXE}")
  file(MAKE_DIRECTORY "${buildvm_BINARY_DIR}")

  add_custom_command(OUTPUT "${BUILDVM_TARGET_FILE}"
    COMMAND "${CMAKE_COMMAND}" "${CMAKE_CURRENT_SOURCE_DIR}/buildvm"
    COMMAND "${CMAKE_COMMAND}" --build .
    DEPENDS gen_buildvm_arch
    WORKING_DIRECTORY "${buildvm_BINARY_DIR}"
  )

  add_custom_target(buildvm ALL DEPENDS "${BUILDVM_TARGET_FILE}")
endif()

add_dependencies(buildvm gen_buildvm_arch)

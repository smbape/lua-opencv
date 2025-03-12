##############################################################################
# Target system detection.
##############################################################################

set(TARGET_TESTARCH_INPUT "
#include <stdio.h>
#include <lj_arch.h>

#define _stringify(s) #s
#define stringify(s) _stringify(s)

static const char * const DEFINES = \"\"")

foreach(variable IN ITEMS
    __AARCH64EB__
    LJ_ABI_SOFTFP
    LJ_ARCH_BITS
    LJ_ABI_PAUTH
    LJ_ARCH_HASFPU
    LJ_ARCH_PPC32ON64
    LJ_ARCH_PPC64
    LJ_ARCH_ROUND
    LJ_TARGET_MIPSR6
    LJ_ARCH_SQRT
    LJ_ARCH_VERSION
    LJ_DUALNUM
    LJ_FR2
    LJ_HASFFI
    LJ_HASJIT
    LJ_LE
    LJ_NO_UNWIND
    LJ_TARGET_ARM
    LJ_TARGET_ARM64
    LJ_TARGET_MIPS
    LJ_TARGET_MIPS64
    LJ_TARGET_PPC
    LJ_TARGET_PS3
    LJ_TARGET_X64
    LJ_TARGET_X86
    MIPSEL
)
    set(TARGET_TESTARCH_INPUT "${TARGET_TESTARCH_INPUT}
        #ifdef ${variable}
        \"#define ${variable} \" stringify(${variable}) \"\\n\"
        #endif
")
endforeach()

set(TARGET_TESTARCH_INPUT "${TARGET_TESTARCH_INPUT};

int main(int argc, char **argv)
{
    puts(DEFINES);
    return 0;
}")

set(testarch_BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}/testarch")
file(MAKE_DIRECTORY "${testarch_BINARY_DIR}")

set(TARGET_TESTARCH_FILE "${testarch_BINARY_DIR}/testarch.c")
string(STRIP "${TARGET_TESTARCH_INPUT}" TARGET_TESTARCH_INPUT)
file(CONFIGURE OUTPUT "${TARGET_TESTARCH_FILE}" CONTENT "${TARGET_TESTARCH_INPUT}\n")

try_run(TARGET_TESTARCH_CODE TARGET_TESTARCH_COMPILED
    SOURCES "${TARGET_TESTARCH_FILE}"
    CMAKE_FLAGS "-DINCLUDE_DIRECTORIES=${LUA_INCLUDE_DIRS}"
    RUN_OUTPUT_VARIABLE TARGET_TESTARCH
    COMPILE_OUTPUT_VARIABLE TARGET_TESTARCH_COMPILE
)

set(TARGET_ARCH "")
set(HOST_XCFLAGS "")

if (NOT TARGET_TESTARCH_COMPILED)
        message(FATAL_ERROR "Unsupported target architecture:\n${TARGET_TESTARCH_COMPILE}")
else()
    if(TARGET_TESTARCH MATCHES " LJ_TARGET_X64 ")
        set(TARGET_LJARCH x64)
    elseif(TARGET_TESTARCH MATCHES " LJ_TARGET_X86 ")
        set(TARGET_LJARCH x86)
    elseif(TARGET_TESTARCH MATCHES " LJ_TARGET_ARM ")
        set(TARGET_LJARCH arm)
    elseif(TARGET_TESTARCH MATCHES " LJ_TARGET_ARM64 ")
        set(TARGET_LJARCH arm64)
        if (TARGET_TESTARCH MATCHES " __AARCH64EB__ ")
            list(APPEND TARGET_ARCH "__AARCH64EB__=1")
        endif()
    elseif(TARGET_TESTARCH MATCHES " LJ_TARGET_PPC ")
        set(TARGET_LJARCH ppc)
        if (TARGET_TESTARCH MATCHES " LJ_LE 1")
            list(APPEND TARGET_ARCH "LJ_ARCH_ENDIAN=LUAJIT_LE")
        else()
            list(APPEND TARGET_ARCH "LJ_ARCH_ENDIAN=LUAJIT_BE")
        endif()
    elseif(TARGET_TESTARCH MATCHES " LJ_TARGET_MIPS ")
        if (TARGET_TESTARCH MATCHES " LJ_TARGET_MIPS64 ")
            set(TARGET_LJARCH mips64)
        else()
            set(TARGET_LJARCH mips)
        endif()
        if (TARGET_TESTARCH MATCHES " MIPSEL ")
            list(APPEND TARGET_ARCH "__MIPSEL__=1")
        endif()
    else()
        message(FATAL_ERROR "Unsupported target architecture:\n${TARGET_TESTARCH}")
    endif()
endif()

if (TARGET_TESTARCH MATCHES " LJ_TARGET_PS3 ")
    set(CMAKE_SYSTEM_NAME PS3)
    list(APPEND TARGET_ARCH "__CELLOS_LV2__")
endif()

list(APPEND TARGET_ARCH "LUAJIT_TARGET=LUAJIT_ARCH_${TARGET_LJARCH}")

if (CMAKE_CROSSCOMPILING)
    if ("${CMAKE_SYSTEM_NAME}" STREQUAL "Android")
        list(APPEND TARGET_ARCH "LUAJIT_OS=LUAJIT_OS_LINUX")
        list(APPEND HOST_XCFLAGS "-malign-double")
    elseif ("${CMAKE_SYSTEM_NAME}" STREQUAL "Windows")
        list(APPEND TARGET_ARCH "LUAJIT_OS=LUAJIT_OS_WINDOWS")
    elseif ("${CMAKE_SYSTEM_NAME}" STREQUAL "iOS")
        list(APPEND TARGET_ARCH "LUAJIT_OS=LUAJIT_OS_OSX" "TARGET_OS_IPHONE=1")
    elseif ("${CMAKE_SYSTEM_NAME}" STREQUAL "Darwin")
        list(APPEND TARGET_ARCH "LUAJIT_OS=LUAJIT_OS_OSX" "TARGET_OS_IPHONE=0")
    elseif ("${CMAKE_SYSTEM_NAME}" STREQUAL "Linux")
        list(APPEND TARGET_ARCH "LUAJIT_OS=LUAJIT_OS_LINUX")
    else()
        list(APPEND TARGET_ARCH "LUAJIT_OS=LUAJIT_OS_OTHER")
    endif()
endif()

set(DASM_FLAGS "")

if (TARGET_TESTARCH MATCHES " LJ_LE 1")
    list(APPEND DASM_FLAGS -D ENDIAN_LE)
else()
    list(APPEND DASM_FLAGS -D ENDIAN_BE)
endif()

if (TARGET_TESTARCH MATCHES " LJ_ARCH_BITS 64")
    list(APPEND DASM_FLAGS -D P64)
endif()

if ((NOT DISABLE_JIT) AND (TARGET_TESTARCH MATCHES " LJ_HASJIT 1"))
    list(APPEND DASM_FLAGS -D JIT)
endif()

if ((NOT DISABLE_FFI) AND (TARGET_TESTARCH MATCHES " LJ_HASFFI 1"))
    list(APPEND DASM_FLAGS -D FFI)
endif()

if (TARGET_TESTARCH MATCHES " LJ_DUALNUM 1")
    list(APPEND DASM_FLAGS -D DUALNUM)
endif()

if (TARGET_TESTARCH MATCHES " LJ_ARCH_HASFPU 1")
    list(APPEND DASM_FLAGS -D FPU)
    list(APPEND TARGET_ARCH "LJ_ARCH_HASFPU=1")
else()
    list(APPEND TARGET_ARCH "LJ_ARCH_HASFPU=0")
endif()

if (TARGET_TESTARCH MATCHES " LJ_ABI_SOFTFP 1")
    list(APPEND TARGET_ARCH "LJ_ABI_SOFTFP=1")
else()
    list(APPEND TARGET_ARCH "LJ_ABI_SOFTFP=0")
    list(APPEND DASM_FLAGS -D HFABI)
endif()

if (TARGET_TESTARCH MATCHES " LJ_NO_UNWIND 1")
    list(APPEND DASM_FLAGS -D NO_UNWIND)
    list(APPEND TARGET_ARCH LUAJIT_NO_UNWIND)
endif()

if (TARGET_TESTARCH MATCHES " LJ_ABI_PAUTH 1")
    list(APPEND DASM_FLAGS -D PAUTH)
    list(APPEND TARGET_ARCH "LJ_ABI_PAUTH=1")
endif()

# TODO : DASM_AFLAGS+= -D VER=$(subst LJ_ARCH_VERSION_,,$(filter LJ_ARCH_VERSION_%,$(subst LJ_ARCH_VERSION ,LJ_ARCH_VERSION_,$(TARGET_TESTARCH))))

if (WIN32 OR CYGWIN OR MINGW)
    list(APPEND DASM_FLAGS -D WIN)
endif()

set(DASM_ARCH ${TARGET_LJARCH})

if ("${TARGET_LJARCH}" STREQUAL "x64")
    if (NOT (TARGET_TESTARCH MATCHES " LJ_FR2 1"))
        set(DASM_ARCH x86)
    endif()
elseif ("${TARGET_LJARCH}" STREQUAL "arm")
    if ("${CMAKE_SYSTEM_NAME}" STREQUAL "iOS")
        list(APPEND DASM_FLAGS -D IOS)
    endif()
elseif ("${TARGET_LJARCH}" STREQUAL "ppc")
    if (TARGET_TESTARCH MATCHES "LJ_TARGET_MIPSR6 ")
        list(APPEND DASM_FLAGS -D MIPSR6)
    endif()

    if (TARGET_TESTARCH MATCHES " LJ_ARCH_SQRT 1")
        list(APPEND DASM_FLAGS -D SQRT)
    endif()
    if (TARGET_TESTARCH MATCHES " LJ_ARCH_ROUND 1")
        list(APPEND DASM_FLAGS -D ROUND)
    endif()
    if (TARGET_TESTARCH MATCHES " LJ_ARCH_PPC32ON64 1")
        list(APPEND DASM_FLAGS -D GPR64)
    endif()
    if ("${CMAKE_SYSTEM_NAME}" STREQUAL "PS3")
        list(APPEND DASM_FLAGS -D PPE -D TOC)
    endif()
    if (TARGET_TESTARCH MATCHES " LJ_ARCH_PPC64 ")
        set(DASM_ARCH ppc64)
    endif()
endif()

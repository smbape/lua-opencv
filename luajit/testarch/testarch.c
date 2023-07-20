#include <stdio.h>
#include <lj_arch.h>

#define _stringify(s) #s
#define stringify(s) _stringify(s)

static const char * const DEFINES = ""
    #ifdef __AARCH64EB__
    "#define __AARCH64EB__ " stringify(__AARCH64EB__) "\n"
    #endif

    #ifdef LJ_ABI_SOFTFP
    "#define LJ_ABI_SOFTFP " stringify(LJ_ABI_SOFTFP) "\n"
    #endif

    #ifdef LJ_ARCH_BITS
    "#define LJ_ARCH_BITS " stringify(LJ_ARCH_BITS) "\n"
    #endif

    #ifdef LJ_ARCH_HASFPU
    "#define LJ_ARCH_HASFPU " stringify(LJ_ARCH_HASFPU) "\n"
    #endif

    #ifdef LJ_ARCH_PPC32ON64
    "#define LJ_ARCH_PPC32ON64 " stringify(LJ_ARCH_PPC32ON64) "\n"
    #endif

    #ifdef LJ_ARCH_PPC64
    "#define LJ_ARCH_PPC64 " stringify(LJ_ARCH_PPC64) "\n"
    #endif

    #ifdef LJ_ARCH_ROUND
    "#define LJ_ARCH_ROUND " stringify(LJ_ARCH_ROUND) "\n"
    #endif

    #ifdef LJ_ARCH_SQRT
    "#define LJ_ARCH_SQRT " stringify(LJ_ARCH_SQRT) "\n"
    #endif

    #ifdef LJ_ARCH_VERSION
    "#define LJ_ARCH_VERSION " stringify(LJ_ARCH_VERSION) "\n"
    #endif

    #ifdef LJ_DUALNUM
    "#define LJ_DUALNUM " stringify(LJ_DUALNUM) "\n"
    #endif

    #ifdef LJ_FR2
    "#define LJ_FR2 " stringify(LJ_FR2) "\n"
    #endif

    #ifdef LJ_HASFFI
    "#define LJ_HASFFI " stringify(LJ_HASFFI) "\n"
    #endif

    #ifdef LJ_HASJIT
    "#define LJ_HASJIT " stringify(LJ_HASJIT) "\n"
    #endif

    #ifdef LJ_LE
    "#define LJ_LE " stringify(LJ_LE) "\n"
    #endif

    #ifdef LJ_NO_UNWIND
    "#define LJ_NO_UNWIND " stringify(LJ_NO_UNWIND) "\n"
    #endif

    #ifdef LJ_TARGET_ARM
    "#define LJ_TARGET_ARM " stringify(LJ_TARGET_ARM) "\n"
    #endif

    #ifdef LJ_TARGET_ARM64
    "#define LJ_TARGET_ARM64 " stringify(LJ_TARGET_ARM64) "\n"
    #endif

    #ifdef LJ_TARGET_MIPS
    "#define LJ_TARGET_MIPS " stringify(LJ_TARGET_MIPS) "\n"
    #endif

    #ifdef LJ_TARGET_MIPS64
    "#define LJ_TARGET_MIPS64 " stringify(LJ_TARGET_MIPS64) "\n"
    #endif

    #ifdef LJ_TARGET_PPC
    "#define LJ_TARGET_PPC " stringify(LJ_TARGET_PPC) "\n"
    #endif

    #ifdef LJ_TARGET_PS3
    "#define LJ_TARGET_PS3 " stringify(LJ_TARGET_PS3) "\n"
    #endif

    #ifdef LJ_TARGET_X64
    "#define LJ_TARGET_X64 " stringify(LJ_TARGET_X64) "\n"
    #endif

    #ifdef LJ_TARGET_X86
    "#define LJ_TARGET_X86 " stringify(LJ_TARGET_X86) "\n"
    #endif

    #ifdef MIPSEL
    "#define MIPSEL " stringify(MIPSEL) "\n"
    #endif
;

int main(int argc, char **argv)
{
    puts(DEFINES);
    return 0;
}

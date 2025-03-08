// Resource script for LuaJIT core DLL.
// Currently only holds version information.
//
#pragma code_page(65001)
#include "winver.h"

#define _stringify(s) #s
#define stringify(s) _stringify(s)

// #define LUAJIT_COMPANY   "Mike Pall"
// #define LUAJIT_COPYRIGHT "Copyright \xA9 2005-2023 Mike Pall."

#define LUAJIT_PRODUCT_VERSION MAJVER, MINVER, RELVER
#define LUAJIT_VERSION stringify(Luajit_VERSION)

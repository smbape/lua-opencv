// Resource script for LuaJIT core DLL.
// Currently only holds version information.
//
#pragma code_page(65001)
#include "winver.h"

#define _stringify(s) #s
#define stringify(s) _stringify(s)

#define LUA_PRODUCT_VERSION MAJVER, MINVER, RELVER
#define LUA_VERSION stringify(Luajit_VERSION)

#include <luadef.hpp>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <queue>
#include <sstream>
#include <Windows.h>

namespace {
	// Lua 5.3 causes a segfault on windows when top == lua_gettop(L)
	// Therefore, don't do anything if the top will not change
	inline void lua_fixtop(lua_State *L, int idx) {
		if (idx != -1 && idx != lua_gettop(L)) {
			lua_replace(L, idx);
			lua_settop(L, idx);
		}
	}
}

/*
** LUA_CSUBSEP is the character that replaces dots in submodule names
** when searching for a C loader.
** LUA_LSUBSEP is the character that replaces dots in submodule names
** when searching for a Lua loader.
*/
#if !defined(LUA_CSUBSEP)
#define LUA_CSUBSEP		LUA_DIRSEP
#endif

#if !defined(LUA_LSUBSEP)
#define LUA_LSUBSEP		LUA_DIRSEP
#endif


/* prefix for open functions in C libraries */
#define LUA_POF		"luaopen_"

/* Cast a ptrdiff_t to size_t, when it is known that the minuend
** comes from the subtrahend (the base)
*/
#define ct_diff2sz(df)  ((size_t)(df))

/*
** {==================================================================
** Configuration for Paths.
** ===================================================================
*/

/*
** LUA_PATH_SEP is the character that separates templates in a path.
** LUA_PATH_MARK is the string that marks the substitution points in a
** template.
*/
#if !defined (LUA_PATH_SEP)
#define LUA_PATH_SEP		";"
#endif
#if !defined (LUA_PATH_MARK)
#define LUA_PATH_MARK		"?"
#endif

#if (LUA_VERSION_NUM > 501) && (LUA_VERSION_NUM < 504)
#define luaL_bufflen(bf)    ((bf)->n)
#define luaL_buffaddr(bf)   ((bf)->b)
#endif

/*
** {======================================================
** 'require' function
** =======================================================
*/

namespace {
#if LUA_VERSION_NUM < 504
	void luaL_addgsub(luaL_Buffer* b, const char* s,
		const char* p, const char* r) {
		const char* wild;
		size_t l = strlen(p);
		while ((wild = strstr(s, p)) != NULL) {
			luaL_addlstring(b, s, ct_diff2sz(wild - s));  /* push prefix */
			luaL_addstring(b, r);  /* push replacement in place of pattern */
			s = wild + l;  /* continue after 'p' */
		}
		luaL_addstring(b, s);  /* push last suffix */
	}
#endif

	int readable(const char* filename) {
		FILE* f = fopen(filename, "r");  /* try to open file */
		if (f == NULL) return 0;  /* open failed */
		fclose(f);
		return 1;
	}


	/*
	** Get the next name in '*path' = 'name1;name2;name3;...', changing
	** the ending ';' to '\0' to create a zero-terminated string. Return
	** NULL when list ends.
	*/
	const char* getnextfilename(char** path, char* end) {
		char* sep;
		char* name = *path;
		if (name == end)
			return NULL;  /* no more names */
		else if (*name == '\0') {  /* from previous iteration? */
			*name = *LUA_PATH_SEP;  /* restore separator */
			name++;  /* skip it */
		}
		sep = strchr(name, *LUA_PATH_SEP);  /* find next separator */
		if (sep == NULL)  /* separator not found? */
			sep = end;  /* name goes until the end */
		*sep = '\0';  /* finish file name */
		*path = sep;  /* will start next search from here */
		return name;
	}


	/*
	** Given a path such as ";blabla.so;blublu.so", pushes the string
	**
	** no file 'blabla.so'
	**  no file 'blublu.so'
	*/
	void pusherrornotfound(lua_State* L, const char* path) {
		luaL_Buffer b;
		luaL_buffinit(L, &b);
		luaL_addstring(&b, "no file '");
		luaL_addgsub(&b, path, LUA_PATH_SEP, "'\n\tno file '");
		luaL_addstring(&b, "'");
		luaL_pushresult(&b);
	}


	const char* searchpath(lua_State* L, const char* name,
		const char* path,
		const char* sep,
		const char* dirsep) {
		luaL_Buffer buff;
		char* pathname;  /* path with name inserted */
		char* endpathname;  /* its end */
#if LUA_VERSION_NUM <= 501
		char* startpathname;  /* its start */
#endif
		const char* filename;
		const int top = lua_gettop(L) + 1;
		/* separator is non-empty and appears in 'name'? */
		if (*sep != '\0' && strchr(name, *sep) != NULL)
			name = luaL_gsub(L, name, sep, dirsep);  /* replace it by 'dirsep' */
		luaL_buffinit(L, &buff);
		/* add path to the buffer, replacing marks ('?') with the file name */
		luaL_addgsub(&buff, path, LUA_PATH_MARK, name);
		luaL_addchar(&buff, '\0');
#if LUA_VERSION_NUM > 501
		pathname = luaL_buffaddr(&buff);  /* writable list of file names */
		endpathname = pathname + luaL_bufflen(&buff) - 1;
#else
		luaL_pushresult(&buff);  /* push path to create error message */
		startpathname = (char*) lua_tostring(L, -1);
		pathname = startpathname;
		endpathname = pathname + strlen(pathname);
#endif
		while ((filename = getnextfilename(&pathname, endpathname)) != NULL) {
			if (readable(filename)) {  /* does file exist and is readable? */
				lua_pushstring(L, filename);  /* save and return name */
				lua_fixtop(L, top);
				return lua_tostring(L, -1);
			}
		}
#if LUA_VERSION_NUM > 501
		luaL_pushresult(&buff);  /* push path to create error message */
#else
		lua_pushstring(L, startpathname);  /* push path to create error message */
#endif
		pusherrornotfound(L, lua_tostring(L, -1));  /* create error message */
		lua_fixtop(L, top);
		return NULL;  /* not found */
	}

	const char* findfile(lua_State* L, const char* name,
		const char* pname,
		const char* dirsep) {
		const char* path;
		const char* filename;
		const int top = lua_gettop(L) + 1;

		lua_getglobal(L, "package"); // get package
		if (lua_isnil(L, -1)) {
			luaL_error(L, "global variable 'package' wast not found");
		}

		lua_getfield(L, -1, pname);
		path = lua_tostring(L, -1);
		if (path == NULL) {
			luaL_error(L, "'package.%s' must be a string", pname);
		}

		filename = searchpath(L, name, path, ".", dirsep);
		lua_fixtop(L, top);
		return filename;
	}
}

// Source - https://stackoverflow.com/questions/18783087/how-to-properly-use-getmodulefilename#54491532
// Posted by Ivan Kolev, modified by community. See post 'Timeline' for change history
// Retrieved 2026-07-27, License - CC BY-SA 4.0

template <
	class CharT,
	typename TGetString,
	class Traits = std::char_traits<CharT>,
	class Allocator = std::allocator<CharT>
>
std::basic_string<CharT, Traits, Allocator> WinAPI_CallGetString(TGetString GetString, int initial_size = 0) {
	if (initial_size <= 0) {
		initial_size = MAX_PATH;
	}

	std::basic_string<CharT, Traits, Allocator> result(initial_size, 0);
	while (true) {
		auto length = GetString(&result[0], result.length());
		if (length == 0) {
			return std::basic_string<CharT, Traits, Allocator>();
		}

		if (length < result.length() - 1) {
			result.resize(length);
			result.shrink_to_fit();
			return result;
		}

		result.resize(result.length() * 2);
	}
}

std::string WinAPI_GetModuleFileNameA(HMODULE hModule) {
	return WinAPI_CallGetString<std::string::value_type>([&](LPSTR lpFilename, DWORD nSize) {
		return GetModuleFileNameA(hModule, lpFilename, nSize);
		});
}

std::wstring WinAPI_GetModuleFileNameW(HMODULE hModule) {
	return WinAPI_CallGetString<std::wstring::value_type>([&](LPWSTR lpFilename, DWORD nSize) {
		return GetModuleFileNameW(hModule, lpFilename, nSize);
		});
}

namespace {
	bool ends_with(const char* str, const char* suffix) {
		if (!str || !suffix) {
			return false;
		}

		size_t lenstr = strlen(str);
		size_t lensuffix = strlen(suffix);

		if (lenstr < lensuffix) {
			return false;
		}

		return strncmp(str + lenstr - lensuffix, suffix, lensuffix) == 0;
	}
}

using namespace std::literals;
namespace fs = std::filesystem;

namespace {
	std::string get_system_error_message() {
		// Retrieve the system error message for the last-error code
		std::string errmsg;

		LPVOID lpMsgBuf;
		DWORD dw = GetLastError();

		if (FormatMessage(
			FORMAT_MESSAGE_ALLOCATE_BUFFER |
			FORMAT_MESSAGE_FROM_SYSTEM |
			FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL,
			dw,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			(LPTSTR)&lpMsgBuf,
			0, NULL) != 0
			) {
			errmsg.assign((LPCTSTR)lpMsgBuf);
			LocalFree(lpMsgBuf);
		}

		return errmsg;
	}
}

struct HandleDeleter {
	~HandleDeleter() {
		if (!CloseHandle(hObject)) {
			errmsg << "\n\t" << msg << " " << filepath << " : " << get_system_error_message();
		}
	}

	HANDLE hObject;
	fs::path filepath;
	std::ostringstream& errmsg;
	std::string msg;
};

struct MapViewOfFileDeleter {
	using pointer = LPVOID;

	void operator()(pointer lpBaseAddress) const
	{
		if (!UnmapViewOfFile(lpBaseAddress)) {
			errmsg << "\n\tfailed to unmap file map view of file " << filepath << " : " << get_system_error_message();
		}
	}

	fs::path filepath;
	std::ostringstream& errmsg;
};

struct LibraryDeleter {
	~LibraryDeleter() {
		FreeLibrary(hLibModule);
	}
	HMODULE hLibModule;
};

namespace {
	// Source - https://stackoverflow.com/questions/45420985/c-get-native-dll-dependencies-without-loading-it-in-process#45481743
	// Posted by Dmitry Katkevich
	// Retrieved 2026-07-27, License - CC BY-SA 3.0

	//Defining in which section particular RVA address actually located (section number)
	DWORD RVAtoRAW(DWORD rva, PIMAGE_SECTION_HEADER sectionHeaderRAW, WORD sectionsCount)
	{
		int sectionNo;
		for (sectionNo = 0; sectionNo < sectionsCount; ++sectionNo)
		{
			auto sectionBeginRVA = sectionHeaderRAW[sectionNo].VirtualAddress;
			auto sectionEndRVA = sectionBeginRVA + sectionHeaderRAW[sectionNo].Misc.VirtualSize;
			if (sectionBeginRVA <= rva && rva <= sectionEndRVA)
				break;
		}
		//Evaluating RAW address from section & RVA
		auto sectionRAW = sectionHeaderRAW[sectionNo].PointerToRawData;
		auto sectionRVA = sectionHeaderRAW[sectionNo].VirtualAddress;
		auto raw = sectionRAW + rva - sectionRVA;

		return raw;
	}

	bool add_dependcy(std::set<fs::path>& dependencies, const fs::path& dependency) {
		if (!fs::exists(dependency)) {
			return false;
		}

		const auto canonical = fs::canonical(dependency);
		const auto filename = canonical.filename().string();
		dependencies.insert(canonical);
		return true;
	}

	bool add_module_dependency(lua_State* L, std::vector<std::string>& module_deps, const char* name) {
		const char* filename = findfile(L, name, "cpath", LUA_CSUBSEP);
		bool added = filename != NULL;
		if (added) {
			module_deps.push_back(name);
		}
		lua_pop(L, 1);
		return added;
	}

	void GetDllDependencyTree(
		lua_State* L,
		const std::vector<fs::path>& dll_directories,
		std::unordered_map<fs::path, std::set<fs::path>>& dependency_tree,
		std::vector<std::string>& module_deps,
		std::ostringstream& errmsg,
		const fs::path& filepath
	) {
		if (dependency_tree.contains(filepath)) {
			return;
		}

		dependency_tree.emplace(std::make_pair(filepath, std::set<fs::path>()));
		decltype(auto) dependencies = dependency_tree.at(filepath);

		// Create the test file. Open it "Create Always" to overwrite any
		// existing file. The data is re-created below
		auto hFile = CreateFileW(filepath.wstring().c_str(),
			GENERIC_READ,
			0,
			NULL,
			OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL,
			NULL);

		if (hFile == INVALID_HANDLE_VALUE) {
			errmsg << "\n\tfailed read file " << filepath << " : " << get_system_error_message();
			return;
		}
		HandleDeleter hFileDeleter{ .hObject = hFile, .filepath = filepath, .errmsg = errmsg, .msg = "failed to close handle to file" };

		// Create a file mapping object for the file
		// Note that it is a good idea to ensure the file size is not zero
		auto hMapFile = CreateFileMappingW(hFile,          // current file handle
			NULL,           // default security
			PAGE_READONLY,  // read permission
			0,              // size of mapping object, high
			0,              // size of mapping object, low
			NULL);          // name of mapping object

		if (hMapFile == NULL) {
			errmsg << "\n\tfailed to create file mapping for " << filepath << " : " << get_system_error_message();
			return;
		}
		HandleDeleter hMapFileDeleter{ .hObject = hMapFile, .filepath = filepath, .errmsg = errmsg, .msg = "failed to close handle to file mapping of" };

		// Map the view and test the results.
		auto lpMapAddress = MapViewOfFile(hMapFile, // handle to mapping object
			FILE_MAP_READ, // read only
			0,             // high-order 32 bits of file offset
			0,             // low-order 32 bits of file offset
			0);            // number of bytes to map

		if (lpMapAddress == NULL) {
			errmsg << "\n\tfailed to create file map view of file " << filepath << " : " << get_system_error_message();
			return;
		}
		MapViewOfFileDeleter lpMapViewOfFileDeleter{ .filepath = filepath, .errmsg = errmsg };
		std::unique_ptr<LPVOID, MapViewOfFileDeleter&> hMapViewOfFileUniquePtr(lpMapAddress, lpMapViewOfFileDeleter);

		// RAW - offset from beginnig of the file (absolute "address" within file)
		auto baseRAW = (char*)lpMapAddress;
		auto dosHeaderRAW = (PIMAGE_DOS_HEADER)baseRAW;
		auto peHeaderRAW = (PIMAGE_NT_HEADERS)(baseRAW + dosHeaderRAW->e_lfanew);
		auto sectionHeaderRAW = (PIMAGE_SECTION_HEADER)(baseRAW + dosHeaderRAW->e_lfanew + sizeof(IMAGE_NT_HEADERS));

		auto sectionsCount = peHeaderRAW->FileHeader.NumberOfSections;

		//RVA - Relative Virtual Address - relative (to ImageBase) address within virtual address space of the process which loads this DLL
		auto importTableRVA = peHeaderRAW->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress;
		auto importTableRAW = RVAtoRAW(importTableRVA, sectionHeaderRAW, sectionsCount);
		auto importTable = (PIMAGE_IMPORT_DESCRIPTOR)(baseRAW + importTableRAW);

		const auto parent_path = fs::canonical(filepath).parent_path();

		while (importTableRVA && importTable->OriginalFirstThunk) {
			auto nameRAW = RVAtoRAW(importTable->Name, sectionHeaderRAW, sectionsCount);
			auto importedModuleName = (char*)(DWORD_PTR)(nameRAW + baseRAW);
			auto found = false;

			// 1 : search in package.cpath
			if (!found && ends_with(importedModuleName, "_lua.dll")) {
				const std::string name(importedModuleName, strlen(importedModuleName) - sizeof(".dll") + 1);
				found = add_module_dependency(L, module_deps, name.c_str());
			}

			// 2 : search in the directory that contains the DLL
			if (!found) {
				found = add_dependcy(dependencies, parent_path / importedModuleName);
			}

			// 3 : search in the user directories
			if (!found) {
				for (const auto& dll_directory : dll_directories) {
					if (add_dependcy(dependencies, dll_directory / importedModuleName)) {
						found = true;
					}
				}
			}

			importTable++;
		}

		for (const auto& dll_path : dependencies) {
			GetDllDependencyTree(L, dll_directories, dependency_tree, module_deps, errmsg, dll_path);
		}
	}

	void GetOrderedDependencies(
		const std::unordered_map<fs::path, std::set<fs::path>>& dependency_tree,
		std::vector<fs::path>& dependencies
	) {
		std::unordered_set<fs::path> remaining;
		std::unordered_map<fs::path, std::size_t> dependency_count;
		std::unordered_map<fs::path, std::vector<fs::path>> reverse_graph;

		for (const auto& [file, deps] : dependency_tree) {
			remaining.insert(file);
			dependency_count.try_emplace(file, 0);
			dependency_count[file] += deps.size();

			for (const auto& dep : deps) {
				dependency_count.try_emplace(dep, 0);
				reverse_graph[dep].push_back(file);
			}
		}

		while (!remaining.empty()) {
			size_t min_deps = 0;
			fs::path min_file;
			std::queue<fs::path> queue;

			for (const auto& file : remaining) {
				if (dependency_count[file] == 0) {
					queue.push(file);
				}
				else if (min_deps == 0 || min_deps > dependency_count[file]) {
					min_deps = dependency_count[file];
					min_file = file;
				}
			}

			if (queue.empty()) {
				// cyclic dependency detected
				// choose the first file with the least dependencies
				queue.push(min_file);
			}

			while (!queue.empty()) {
				auto file = queue.front(); queue.pop();
				dependencies.push_back(file);
				remaining.erase(file);

				for (const auto& dependent : reverse_graph[file]) {
					if (--dependency_count[dependent] == 0) {
						queue.push(dependent);
					}
				}
			}
		}
	}

	bool load_module(
		lua_State* L,
		std::vector<std::shared_ptr<LibraryDeleter>>& loaded,
		std::ostringstream& errmsg
	) {
		errmsg << "module '" LUA_MODULE_NAME_STR "' not found:";

		const char* filename = findfile(L, LUA_MODULE_NAME_STR, "cpath", LUA_CSUBSEP);
		if (filename == NULL) {
			errmsg << "\n\t" << lua_tostring(L, -1); /* get error message */
			lua_pop(L, 1);
			return false;
		}

		const fs::path module_filename(filename);
		lua_pop(L, 1);

		std::vector<fs::path> dll_directories;

		auto dll_dirtory = fs::canonical(module_filename).parent_path() / LUA_MODULE_NAME_STR / "libs";
		if (fs::exists(dll_dirtory)) {
			dll_directories.push_back(dll_dirtory);
		}
		else {
			errmsg << "\n\tno directory " << dll_dirtory;
		}

		std::unordered_map<fs::path, std::set<fs::path>> dependency_tree;
		std::vector<std::string> module_deps;
		GetDllDependencyTree(L, dll_directories, dependency_tree, module_deps, errmsg, module_filename);

		std::vector<fs::path> dependencies;
		GetOrderedDependencies(dependency_tree, dependencies);

		bool has_error = false;

		for (const auto& modulename : module_deps) {
			auto top = lua_gettop(L);
			lua_getglobal(L, "require");
			lua_pushstring(L, modulename.c_str());
			lua_call(L, 1, 0);  /* call 'require(modulename)' */
		}

		for (const auto& dependency : dependencies) {
			auto hLibModule = LoadLibraryW(dependency.wstring().c_str());

			if (hLibModule == NULL) {
				has_error = true;
				errmsg << "\n\tfailed to load file " << dependency << " : " << get_system_error_message();
				continue;
			}

			loaded.emplace_back(std::make_shared<LibraryDeleter>(hLibModule));
		}

		if (has_error) {
			loaded.clear();
		}

		return !has_error;
	}
}

LUAAPI(int) LUA_MODULE_LUAOPEN(lua_State* L) {
	using LP_LUA_OPEN = int (*) (lua_State* L);

	static std::vector<std::shared_ptr<LibraryDeleter>> loaded;

	if (loaded.empty()) {
		std::ostringstream errmsg;

		if (!load_module(L, loaded, errmsg)) {
			luaL_error(L, "%s", errmsg.str().c_str());
			return 0;
		}
	}

	const auto hLibModule = loaded.back()->hLibModule;
	const auto luaopen_lib = reinterpret_cast<LP_LUA_OPEN>(GetProcAddress(hLibModule, LUA_POF LUA_MODULE_NAME_STR));
	if (luaopen_lib == NULL) {
		std::ostringstream errmsg; errmsg << "module '" LUA_MODULE_NAME_STR "' not found:";
		const auto filename = WinAPI_GetModuleFileNameA(hLibModule);
		errmsg << "\n\tfailed to retrieve fthe address of 'luaopen_" LUA_MODULE_NAME_STR "' from '" << filename << "' : " << get_system_error_message();
		luaL_error(L, "%s", errmsg.str().c_str());
		return 0;
	}

	return luaopen_lib(L);
}

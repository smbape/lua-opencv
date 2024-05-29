#pragma once

#if defined(ENABLE_WIDE_CHAR) && defined(_MSC_VER)

#ifndef NOMINMAX
#define NOMINMAX
#endif

#ifndef STRICT
#define STRICT
#endif

#include <algorithm>
#include <memory>
#include <string>
#include <windows.h>

namespace anonymous {
	template <typename char_type>
	inline bool null_or_empty(const char_type* s) {
		return s == nullptr || *s == 0;
	}

	/**
	 * Maps a character string to a UTF-16 (wide character) string. The character string is not necessarily from a multibyte character set.
	 *
	 * @param codePage  [in]  Code page to use in performing the conversion.
	 * @param c_str     [in]  Pointer to the character string to convert.
	 * @param length    [in]  Size, in bytes, of the string indicated by the c_str parameter. Alternatively, this parameter can be set to -1 if the string is null-terminated.
	 * @param wstr      [out] Pointer to a buffer that receives the converted string.
	 * @return 	        The number of characters written to the buffer pointed to by wstr.
	 * @see             https://stackoverflow.com/questions/6693010/how-do-i-use-multibytetowidechar/59617138#59617138
	 *                  https://learn.microsoft.com/en-us/windows/win32/api/stringapiset/nf-stringapiset-multibytetowidechar
	 */
	inline int mbs_to_wcs(UINT codePage, const char* c_str, size_t length, std::wstring& wstr) {
		if (null_or_empty(c_str)) {
			wstr.clear();
			return 0;
		}

		int size = MultiByteToWideChar(codePage, 0, c_str, length, nullptr, 0);
		wstr.assign(size, 0);
		return MultiByteToWideChar(codePage, 0, c_str, length, &wstr[0], size + 1);
	}

	/**
	 * Maps a character string to a UTF-16 (wide character) string. The character string is not necessarily from a multibyte character set.
	 *
	 * @param codePage  [in]  Code page to use in performing the conversion.
	 * @param  str      [in]  The string to convert.
	 * @param  wstr     [out] Pointer to a buffer that receives the converted string.
	 * @see             https://stackoverflow.com/questions/6693010/how-do-i-use-multibytetowidechar/59617138#59617138
	 *                  https://learn.microsoft.com/en-us/windows/win32/api/stringapiset/nf-stringapiset-multibytetowidechar
	 */
	inline int mbs_to_wcs(UINT codePage, const std::string& str, std::wstring& wstr) {
		return mbs_to_wcs(codePage, str.c_str(), str.length(), wstr);
	}

	/**
	 * Maps a UTF-16 (wide character) string to a new character string. The new character string is not necessarily from a multibyte character set.
	 *
	 * @param  codePage Code page to use in performing the conversion.
	 * @param  c_wstr   Pointer to the Unicode string to convert.
	 * @param  length   Size, in characters, of the string indicated by c_wstr parameter.
	 * @param  str      Pointer to a buffer that receives the converted string.
	 * @return          The number of bytes written to the buffer pointed to by c_str.
	 * @see             https://learn.microsoft.com/en-us/windows/win32/api/stringapiset/nf-stringapiset-widechartomultibyte
	 */
	inline int wcs_to_mbs(UINT codePage, const WCHAR* c_wstr, size_t length, std::string& str) {
		if (null_or_empty(c_wstr)) {
			str.clear();
			return 0;
		}

		int size = WideCharToMultiByte(codePage, 0, c_wstr, length, nullptr, 0, nullptr, nullptr);
		str.assign(size, 0);
		return WideCharToMultiByte(codePage, 0, c_wstr, length, &str[0], size + 1, nullptr, nullptr);
	}

	/**
	 * Maps a UTF-16 (wide character) string to a new character string. The new character string is not necessarily from a multibyte character set.
	 *
	 * @param  codePage Code page to use in performing the conversion.
	 * @param  wstr     Pointer to the Unicode string to convert.
	 * @param  str      Pointer to a buffer that receives the converted string.
	 * @return          The number of bytes written to the buffer pointed to by str.
	 * @see             https://learn.microsoft.com/en-us/windows/win32/api/stringapiset/nf-stringapiset-widechartomultibyte
	 */
	inline int wcs_to_mbs(UINT codePage, const std::wstring& wstr, std::string& str) {
		return wcs_to_mbs(codePage, wstr.c_str(), wstr.length(), str);
	}

	/**
	 * Maps a character string to a UTF-16 (wide character) string. The character string is not necessarily from a multibyte character set.
	 *
	 * @param c_str     [in]  Pointer to the character string to convert.
	 * @param length    [in]  Size, in bytes, of the string indicated by the c_str parameter. Alternatively, this parameter can be set to -1 if the string is null-terminated.
	 * @param wstr      [out] Pointer to a buffer that receives the converted string.
	 * @return 	        The number of characters written
	 * @see             https://stackoverflow.com/questions/6693010/how-do-i-use-multibytetowidechar/59617138#59617138
	 *                  https://learn.microsoft.com/en-us/windows/win32/api/stringapiset/nf-stringapiset-multibytetowidechar
	 */
	inline int utf8_to_wcs(const char* c_str, size_t length, std::wstring& wstr) {
		return mbs_to_wcs(CP_UTF8, c_str, length, wstr);
	}

	/**
	 * Maps a character string to a UTF-16 (wide character) string. The character string is not necessarily from a multibyte character set.
	 *
	 * @param  str      [in]  The string to convert.
	 * @param  wstr     [out] Pointer to a buffer that receives the converted string.
	 * @see             https://stackoverflow.com/questions/6693010/how-do-i-use-multibytetowidechar/59617138#59617138
	 *                  https://learn.microsoft.com/en-us/windows/win32/api/stringapiset/nf-stringapiset-multibytetowidechar
	 */
	inline int utf8_to_wcs(const std::string& str, std::wstring& wstr) {
		return mbs_to_wcs(CP_UTF8, str, wstr);
	}

	inline int wcs_to_utf8(const WCHAR* c_wstr, size_t length, std::string& str) {
		return wcs_to_mbs(CP_UTF8, c_wstr, length, str);
	}

	inline int wcs_to_utf8(const std::wstring& wstr, std::string& str) {
		return wcs_to_mbs(CP_UTF8, wstr, str);
	}

}

#ifndef UNICODE

#undef CallWindowProc
#define CallWindowProc CallWindowProcW

#undef CreateWindow
#define CreateWindow ::anonymous::_CreateWindow

#undef CreateWindowEx
#define CreateWindowEx ::anonymous::_CreateWindowEx

#undef DefWindowProc
#define DefWindowProc DefWindowProcW

#undef DispatchMessage
#define DispatchMessage DispatchMessageW

#undef GetMessage
#define GetMessage GetMessageW

#undef GetModuleHandle
#define GetModuleHandle ::anonymous::_GetModuleHandle

#undef GetMonitorInfo
#define GetMonitorInfo GetMonitorInfoW

#undef GetObject
#define GetObject GetObjectW

#undef GetSaveFileName
#define GetSaveFileName ::anonymous::_GetSaveFileName

#undef GetWindowLong
#define GetWindowLong GetWindowLongW

#undef GetWindowLongPtr
#define GetWindowLongPtr GetWindowLongPtrW

#undef GetWindowText
#define GetWindowText ::anonymous::_GetWindowText

#undef LoadCursor
#define LoadCursor ::anonymous::_LoadCursor

#undef LoadIcon
#define LoadIcon ::anonymous::_LoadIcon

#undef PeekMessage
#define PeekMessage PeekMessageW

#undef RegCreateKeyEx
#define RegCreateKeyEx ::anonymous::_RegCreateKeyEx

#undef RegDeleteKey
#define RegDeleteKey ::anonymous::_RegDeleteKey

#undef RegEnumKeyEx
#define RegEnumKeyEx ::anonymous::_RegEnumKeyEx

#undef RegisterClass
#define RegisterClass ::anonymous::_RegisterClass

#undef RegOpenKeyEx
#define RegOpenKeyEx ::anonymous::_RegOpenKeyEx

#undef RegQueryValueEx
#define RegQueryValueEx ::anonymous::_RegQueryValueEx

#undef RegSetValueEx
#define RegSetValueEx ::anonymous::_RegSetValueEx

#undef SendMessage
#define SendMessage SendMessageW

#undef SetWindowLong
#define SetWindowLong SetWindowLongW

#undef SetWindowLongPtr
#define SetWindowLongPtr SetWindowLongPtrW

#undef SetWindowText
#define SetWindowText ::anonymous::_SetWindowText

#undef UnregisterClass
#define UnregisterClass ::anonymous::_UnregisterClass

namespace anonymous {

	/**
	 * Creates an overlapped, pop-up, or child window with an extended window style; otherwise, this function is identical to the CreateWindow function.
	 *
	 * @param  dwExStyle    The extended window style of the window being created. For a list of possible values, see Extended Window Styles.
	 * @param  lpClassName  A null-terminated string or a class atom created by a previous call to the RegisterClass or RegisterClassEx function.
	 * @param  lpWindowName The window name.
	 * @param  dwStyle      The style of the window being created.
	 * @param  x            The initial horizontal position of the window.
	 * @param  y            The initial vertical position of the window.
	 * @param  nWidth       The width, in device units, of the window.
	 * @param  nHeight      The height, in device units, of the window.
	 * @param  hWndParent   A handle to the parent or owner window of the window being created.
	 * @param  hMenu        A handle to a menu, or specifies a child-window identifier depending on the window style.
	 * @param  hInstance    A handle to the instance of the module to be associated with the window.
	 * @param  lpParam      A pointer to a value to be passed to the window through the CREATESTRUCT structure (lpCreateParams member) pointed to by the lParam param of the WM_CREATE message.
	 * @return              If the function succeeds, the return value is a handle to the new window.
	 *                      If the function fails, the return value is NULL. To get extended error information, call GetLastError.
	 * @see                 https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-createwindowexa
	 */
	inline HWND _CreateWindowEx(
		_In_ DWORD dwExStyle,
		_In_opt_ LPCSTR lpClassName,
		_In_opt_ LPCSTR lpWindowName,
		_In_ DWORD dwStyle,
		_In_ int x,
		_In_ int y,
		_In_ int nWidth,
		_In_ int nHeight,
		_In_opt_ HWND hWndParent,
		_In_opt_ HMENU hMenu,
		_In_opt_ HINSTANCE hInstance,
		_In_opt_ LPVOID lpParam
	) {
		std::wstring wClassName; ::anonymous::utf8_to_wcs(lpClassName, -1, wClassName);
		std::wstring wWindowName; ::anonymous::utf8_to_wcs(lpWindowName, -1, wWindowName);
		return CreateWindowExW(
			dwExStyle,
			lpClassName == nullptr ? nullptr : wClassName.c_str(),
			lpWindowName == nullptr ? nullptr : wWindowName.c_str(),
			dwStyle,
			x,
			y,
			nWidth,
			nHeight,
			hWndParent,
			hMenu,
			hInstance,
			lpParam
		);
	}

	/**
	 * Creates an overlapped, pop-up, or child window.
	 *
	 * @param  lpClassName  A null-terminated string or a class atom created by a previous call to the RegisterClass or RegisterClassEx function.
	 * @param  lpWindowName The window name.
	 * @param  dwStyle      The style of the window being created.
	 * @param  x            The initial horizontal position of the window.
	 * @param  y            The initial vertical position of the window.
	 * @param  nWidth       The width, in device units, of the window.
	 * @param  nHeight      The height, in device units, of the window.
	 * @param  hWndParent   A handle to the parent or owner window of the window being created.
	 * @param  hMenu        A handle to a menu, or specifies a child-window identifier depending on the window style.
	 * @param  hInstance    A handle to the instance of the module to be associated with the window.
	 * @param  lpParam      A pointer to a value to be passed to the window through the CREATESTRUCT structure (lpCreateParams member) pointed to by the lParam param of the WM_CREATE message.
	 * @return              If the function succeeds, the return value is a handle to the new window.
	 *                      If the function fails, the return value is NULL. To get extended error information, call GetLastError.
	 * @see                 https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-createwindowa
	 */
	inline HWND _CreateWindow(
		_In_opt_ LPCSTR lpClassName,
		_In_opt_ LPCSTR lpWindowName,
		_In_ DWORD dwStyle,
		_In_ int x,
		_In_ int y,
		_In_ int nWidth,
		_In_ int nHeight,
		_In_opt_ HWND hWndParent,
		_In_opt_ HMENU hMenu,
		_In_opt_ HINSTANCE hInstance,
		_In_opt_ LPVOID lpParam
	) {
		std::wstring wClassName; ::anonymous::utf8_to_wcs(lpClassName, -1, wClassName);
		std::wstring wWindowName; ::anonymous::utf8_to_wcs(lpWindowName, -1, wWindowName);
		return CreateWindowW(
			lpClassName == nullptr ? nullptr : wClassName.c_str(),
			lpWindowName == nullptr ? nullptr : wWindowName.c_str(),
			dwStyle,
			x,
			y,
			nWidth,
			nHeight,
			hWndParent,
			hMenu,
			hInstance,
			lpParam
		);
	}

	/**
	 * Copies the text of the specified window's title bar (if it has one) into a buffer.
	 *
	 * @param  hWnd      A handle to the window or control containing the text.
	 * @param  lpString  The buffer that will receive the text.
	 * @param  nMaxCount The maximum number of characters to copy to the buffer, including the null character.
	 * @return           If the function succeeds, the return value is the length, in characters, of the copied string, not including the terminating null character.
	 * @see              https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-getwindowtexta
	 */
	inline int _GetWindowText(
		_In_ HWND hWnd,
		_Out_writes_(nMaxCount) LPSTR lpString,
		_In_ int nMaxCount
	) {
		if (lpString == nullptr) {
			return GetWindowTextW(hWnd, nullptr, nMaxCount);
		}

		std::unique_ptr<WCHAR[]> wstr(new WCHAR[nMaxCount]);
		int size = GetWindowTextW(hWnd, wstr.get(), nMaxCount);
		return WideCharToMultiByte(CP_UTF8, 0, wstr.get(), size, lpString, nMaxCount, nullptr, nullptr);
	}

	/**
	 * Retrieves a module handle for the specified module. The module must have been loaded by the calling process.
	 *
	 * @param  lpModuleName The name of the loaded module (either a .dll or .exe file).
	 * @return              If the function succeeds, the return value is a handle to the specified module.
	 *                      If the function fails, the return value is NULL. To get extended error information, call GetLastError.
	 * @see                 https://learn.microsoft.com/en-us/windows/win32/api/libloaderapi/nf-libloaderapi-getmodulehandlea
	 */
	inline HMODULE _GetModuleHandle(
		_In_opt_ LPCSTR lpModuleName
	) {
		if (lpModuleName == nullptr) {
			return GetModuleHandleW(nullptr);
		}

		std::wstring wModuleName; ::anonymous::utf8_to_wcs(lpModuleName, -1, wModuleName);
		return GetModuleHandleW(wModuleName.c_str());
	}

	/**
	 * Creates a Save dialog box that lets the user specify the drive, directory, and name of a file to save.
	 *
	 * @param  lpOpenFileName A pointer to an OPENFILENAME structure that contains information used to initialize the dialog box.
	 * @return                If the user specifies a file name and clicks the OK button and the function is successful, the return value is nonzero.
	 * @see                   https://learn.microsoft.com/en-us/windows/win32/api/commdlg/nf-commdlg-getsavefilenamew
	 */
	inline BOOL _GetSaveFileName(
		_Inout_opt_ LPOPENFILENAMEA lpOpenFileName
	) {
		if (lpOpenFileName == nullptr) {
			return GetSaveFileNameW(nullptr);
		}

		OPENFILENAMEW ofn;
		ZeroMemory(&ofn, sizeof(ofn));

#ifdef OPENFILENAME_SIZE_VERSION_400
		// we are not going to use new fields any way
		ofn.lStructSize = OPENFILENAME_SIZE_VERSION_400;
#else
		ofn.lStructSize = sizeof(ofn);
#endif

		ofn.hwndOwner = lpOpenFileName->hwndOwner;
		ofn.hInstance = lpOpenFileName->hInstance;

		std::wstring wstrFilter;
		if (lpOpenFileName->lpstrFilter != nullptr) {
			::anonymous::utf8_to_wcs(lpOpenFileName->lpstrFilter, -1, wstrFilter);
			ofn.lpstrFilter = wstrFilter.c_str();
		}

		std::unique_ptr<WCHAR[]> wstrCustomFilter;
		if (lpOpenFileName->lpstrCustomFilter != nullptr) {
			wstrCustomFilter.reset(new WCHAR[lpOpenFileName->nMaxCustFilter]);
			ofn.lpstrCustomFilter = wstrCustomFilter.get();

			if (lpOpenFileName->nMaxCustFilter != 0) {
				ofn.nMaxCustFilter = lpOpenFileName->nMaxCustFilter;
				MultiByteToWideChar(CP_UTF8, 0, lpOpenFileName->lpstrCustomFilter, -1, ofn.lpstrCustomFilter, ofn.nMaxCustFilter);
			}
		}

		ofn.nFilterIndex = lpOpenFileName->nFilterIndex;

		std::unique_ptr<WCHAR[]> wstrFile;
		if (lpOpenFileName->lpstrFile != nullptr) {
			wstrFile.reset(new WCHAR[lpOpenFileName->nMaxFile]);
			ofn.lpstrFile = wstrFile.get();

			if (lpOpenFileName->nMaxFile != 0) {
				ofn.nMaxFile = lpOpenFileName->nMaxFile;
				MultiByteToWideChar(CP_UTF8, 0, lpOpenFileName->lpstrFile, -1, ofn.lpstrFile, ofn.nMaxFile);
			}
		}

		std::unique_ptr<WCHAR[]> wstrFileTitle;
		if (lpOpenFileName->lpstrFileTitle != nullptr) {
			wstrFileTitle.reset(new WCHAR[lpOpenFileName->nMaxFileTitle]);
			ofn.lpstrFileTitle = wstrFileTitle.get();

			if (lpOpenFileName->nMaxFileTitle != 0) {
				ofn.nMaxFileTitle = lpOpenFileName->nMaxFileTitle;
				MultiByteToWideChar(CP_UTF8, 0, lpOpenFileName->lpstrFileTitle, -1, ofn.lpstrFileTitle, ofn.nMaxFileTitle);
			}
		}

		std::wstring wstrInitialDir;
		if (lpOpenFileName->lpstrInitialDir != nullptr) {
			::anonymous::utf8_to_wcs(lpOpenFileName->lpstrInitialDir, -1, wstrInitialDir);
			ofn.lpstrInitialDir = wstrInitialDir.c_str();
		}

		std::wstring wstrTitle;
		if (lpOpenFileName->lpstrTitle != nullptr) {
			::anonymous::utf8_to_wcs(lpOpenFileName->lpstrTitle, -1, wstrTitle);
			ofn.lpstrTitle = wstrTitle.c_str();
		}

		ofn.Flags = lpOpenFileName->Flags;
		ofn.nFileOffset = lpOpenFileName->nFileOffset;
		ofn.nFileExtension = lpOpenFileName->nFileExtension;

		std::wstring wstrDefExt;
		if (lpOpenFileName->lpstrDefExt != nullptr) {
			::anonymous::utf8_to_wcs(lpOpenFileName->lpstrDefExt, -1, wstrDefExt);
			ofn.lpstrDefExt = wstrDefExt.c_str();
		}

		ofn.lCustData = lpOpenFileName->lCustData;
		ofn.lpfnHook = lpOpenFileName->lpfnHook;

		std::wstring wTemplateName;
		if (lpOpenFileName->lpTemplateName != nullptr) {
			::anonymous::utf8_to_wcs(lpOpenFileName->lpTemplateName, -1, wTemplateName);
			ofn.lpTemplateName = wTemplateName.c_str();
		}

#ifdef _MAC
		ofn.lpEditInfo = lpOpenFileName->lpEditInfo;
		ofn.lpstrPrompt = lpOpenFileName->lpstrPrompt;
#endif
#if (_WIN32_WINNT >= 0x0500)
		ofn.pvReserved = lpOpenFileName->pvReserved;
		ofn.dwReserved = lpOpenFileName->dwReserved;
		ofn.FlagsEx = lpOpenFileName->FlagsEx;
#endif // (_WIN32_WINNT >= 0x0500)

		if (!GetSaveFileNameW(&ofn)) {
			return false;
		}

		if (ofn.lpstrCustomFilter != nullptr && ofn.nMaxCustFilter != 0) {
			std::string strCustomFilter;
			auto nMaxCustFilter = ::anonymous::wcs_to_utf8(ofn.lpstrCustomFilter, -1, strCustomFilter);
			memcpy(lpOpenFileName->lpstrCustomFilter, strCustomFilter.c_str(), std::min(ofn.nMaxCustFilter, static_cast<DWORD>(nMaxCustFilter)));
		}

		if (ofn.lpstrFile != nullptr && ofn.nMaxFile != 0) {
			std::string strFile;
			auto nMaxFile = ::anonymous::wcs_to_utf8(ofn.lpstrFile, -1, strFile);
			memcpy(lpOpenFileName->lpstrFile, strFile.c_str(), std::min(ofn.nMaxFile, static_cast<DWORD>(nMaxFile)));
		}

		if (ofn.lpstrFileTitle != nullptr && ofn.nMaxFileTitle != 0) {
			std::string strFileTitle;
			auto nMaxFileTitle = ::anonymous::wcs_to_utf8(ofn.lpstrFileTitle, -1, strFileTitle);
			memcpy(lpOpenFileName->lpstrFileTitle, strFileTitle.c_str(), std::min(ofn.nMaxFileTitle, static_cast<DWORD>(nMaxFileTitle)));
		}

		return true;
	}

	/**
	 * Loads the specified cursor resource from the executable (.EXE) file associated with an application instance.
	 *
	 * @param  hInstance    A handle to the module of either a DLL or executable (.exe) file that contains the cursor to be loaded.
	 * @param  lpCursorName If hInstance is non-NULL, lpCursorName specifies the cursor resource either by name or ordinal.
	 * @return              If the function succeeds, the return value is the handle to the newly loaded cursor.
	 *                      If the function fails, the return value is NULL. To get extended error information, call GetLastError.
	 * @see                 https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-loadcursora
	 */
	inline HCURSOR _LoadCursor(
		_In_opt_ HINSTANCE hInstance,
		_In_ LPCSTR lpCursorName
	) {
		if (lpCursorName == nullptr) {
			return LoadCursorW(hInstance, nullptr);
		}

		if (IS_INTRESOURCE(lpCursorName)) {
			return LoadCursorW(hInstance, MAKEINTRESOURCEW(lpCursorName));
		}

		std::wstring wCursorName; ::anonymous::utf8_to_wcs(lpCursorName, -1, wCursorName);
		return LoadCursorW(hInstance, wCursorName.c_str());
	}

	/**
	 * Loads the specified icon resource from the executable (.exe) file associated with an application instance.
	 *
	 * @param  hInstance  A handle to the module of either a DLL or executable (.exe) file that contains the icon to be loaded.
	 * @param  lpIconName If hInstance is non-NULL, lpIconName specifies the icon resource either by name or ordinal.
	 * @return            If the function succeeds, the return value is a handle to the newly loaded icon.
	 *                    If the function fails, the return value is NULL. To get extended error information, call GetLastError.
	 * @see               https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-loadicona
	 */
	inline HCURSOR _LoadIcon(
		_In_opt_ HINSTANCE hInstance,
		_In_ LPCSTR lpIconName
	) {
		if (lpIconName == nullptr) {
			return LoadIconW(hInstance, nullptr);
		}

		if (IS_INTRESOURCE(lpIconName)) {
			return LoadIconW(hInstance, MAKEINTRESOURCEW(lpIconName));
		}
		std::wstring wIconName; ::anonymous::utf8_to_wcs(lpIconName, -1, wIconName);
		return LoadIconW(hInstance, wIconName.c_str());
	}

	/**
	 * Creates the specified registry key. If the key already exists, the function opens it. Note that key names are not case sensitive.
	 *
	 * @param  hKey                 A handle to an open registry key. The calling process must have KEY_CREATE_SUB_KEY access to the key.
	 * @param  lpSubKey             The name of a subkey that this function opens or creates
	 * @param  Reserved             This parameter is reserved and must be zero.
	 * @param  lpClass              The user-defined class type of this key. This parameter may be ignored. This parameter can be NULL.
	 * @param  dwOptions            This parameter can be one of the following values.
	 * @param  samDesired           A mask that specifies the access rights for the key to be created.
	 * @param  lpSecurityAttributes A pointer to a SECURITY_ATTRIBUTES structure that determines whether the returned handle can be inherited by child processes
	 * @param  phkResult            A pointer to a variable that receives a handle to the opened or created key.
	 * @param  lpdwDisposition      A pointer to a variable that receives one of the following disposition values.
	 * @return                      If the function succeeds, the return value is ERROR_SUCCESS.
	 * @see                         https://learn.microsoft.com/en-us/windows/win32/api/winreg/nf-winreg-regcreatekeyexa
	 */
	inline LSTATUS _RegCreateKeyEx(
		_In_ HKEY hKey,
		_In_ LPCSTR lpSubKey,
		_Reserved_ DWORD Reserved,
		_In_opt_ LPSTR lpClass,
		_In_ DWORD dwOptions,
		_In_ REGSAM samDesired,
		_In_opt_ CONST LPSECURITY_ATTRIBUTES lpSecurityAttributes,
		_Out_ PHKEY phkResult,
		_Out_opt_ LPDWORD lpdwDisposition
	) {
		std::wstring wSubKey; ::anonymous::utf8_to_wcs(lpSubKey, -1, wSubKey);
		std::wstring wClass; ::anonymous::utf8_to_wcs(lpClass, -1, wClass);
		return RegCreateKeyExW(
			hKey,
			lpSubKey == nullptr ? nullptr : wSubKey.c_str(),
			Reserved,
			lpClass == nullptr ? nullptr : &wClass[0],
			dwOptions,
			samDesired,
			lpSecurityAttributes,
			phkResult,
			lpdwDisposition
		);
	}

	/**
	 * Deletes a subkey and its values. Note that key names are not case sensitive.
	 *
	 * @param  hKey                 A handle to an open registry key. The access rights of this key do not affect the delete operation.
	 * @param  lpSubKey             The name of the key to be deleted. It must be a subkey of the key that hKey identifies, but it cannot have subkeys.
	 * @return                      If the function succeeds, the return value is ERROR_SUCCESS.
	 * @see                         https://learn.microsoft.com/en-us/windows/win32/api/winreg/nf-winreg-regdeletekeya
	 */
	inline LSTATUS _RegDeleteKey(
		_In_ HKEY hKey,
		_In_ LPCSTR lpSubKey
	) {
		if (lpSubKey == nullptr) {
			return RegDeleteKeyW(hKey, nullptr);
		}

		std::wstring wSubKey; ::anonymous::utf8_to_wcs(lpSubKey, -1, wSubKey);
		return RegDeleteKeyW(hKey, wSubKey.c_str());
	}

	/**
	 * Enumerates the subkeys of the specified open registry key. The function retrieves information about one subkey each time it is called.
	 *
	 * @param  hKey              A handle to an open registry key. The key must have been opened with the KEY_ENUMERATE_SUB_KEYS access right.
	 * @param  dwIndex           The index of the subkey to retrieve. This parameter should be zero for the first call to the RegEnumKeyEx function and then incremented for subsequent calls.
	 * @param  lpName            A pointer to a buffer that receives the name of the subkey, including the terminating null character.
	 * @param  lpcchName         A pointer to a variable that specifies the size of the buffer specified by the lpName parameter, in characters.
	 * @param  lpReserved        This parameter is reserved and must be NULL.
	 * @param  lpClass           A pointer to a buffer that receives the user-defined class of the enumerated subkey. This parameter can be NULL.
	 * @param  lpcchClass        A pointer to a variable that specifies the size of the buffer specified by the lpClass parameter, in characters.
	 * @param  lpftLastWriteTime A pointer to FILETIME structure that receives the time at which the enumerated subkey was last written.
	 * @return                   If the function succeeds, the return value is ERROR_SUCCESS.
	 * @see                      https://learn.microsoft.com/en-us/windows/win32/api/winreg/nf-winreg-regenumkeyexa
	 */
	inline LSTATUS _RegEnumKeyEx(
		_In_ HKEY hKey,
		_In_ DWORD dwIndex,
		_Out_writes_to_opt_(*lpcchName, *lpcchName + 1) LPSTR lpName,
		_Inout_ LPDWORD lpcchName,
		_Reserved_ LPDWORD lpReserved,
		_Out_writes_to_opt_(*lpcchClass, *lpcchClass + 1) LPSTR lpClass,
		_Inout_opt_ LPDWORD lpcchClass,
		_Out_opt_ PFILETIME lpftLastWriteTime
	) {
		std::unique_ptr<WCHAR[]> wName;
		DWORD cchName = lpcchName == nullptr ? 0 : *lpcchName;
		if (lpcchName != nullptr) {
			wName.reset(new WCHAR[*lpcchName]);
			if (cchName != 0) {
				MultiByteToWideChar(CP_UTF8, 0, lpName, -1, wName.get(), cchName);
			}
		}

		std::unique_ptr<WCHAR[]> wClass;
		DWORD cchClass = lpcchClass == nullptr ? 0 : *lpcchClass;
		if (lpcchClass != nullptr) {
			wClass.reset(new WCHAR[*lpcchClass]);
			if (cchClass != 0) {
				MultiByteToWideChar(CP_UTF8, 0, lpClass, -1, wClass.get(), cchClass);
			}
		}

		auto status = RegEnumKeyExW(
			hKey,
			dwIndex,
			wName.get(),
			lpcchName,
			lpReserved,
			wClass.get(),
			lpcchClass,
			lpftLastWriteTime
		);

		if (cchName != 0 && *lpcchName != 0) {
			*lpcchName = WideCharToMultiByte(CP_UTF8, 0, wName.get(), *lpcchName, lpName, cchName, nullptr, nullptr);
			if (*lpcchName < cchName) {
				lpName[*lpcchName + 1] = '\0';
			}
		}

		if (cchClass != 0 && *lpcchClass != 0) {
			*lpcchClass = WideCharToMultiByte(CP_UTF8, 0, wClass.get(), *lpcchClass, lpClass, cchClass, nullptr, nullptr);
			if (*lpcchClass < cchClass) {
				lpClass[*lpcchClass + 1] = '\0';
			}
		}

		return status;
	}

	/**
	 * Registers a window class for subsequent use in calls to the CreateWindow or CreateWindowEx function.
	 *
	 * @param  lpWndClass A pointer to a WNDCLASS structure.
	 * @return            If the function succeeds, the return value is a class atom that uniquely identifies the class being registered.
	 * @see               https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-registerclassa
	 */
	inline ATOM _RegisterClass(_In_ CONST WNDCLASSA* lpWndClass) {
		if (lpWndClass == nullptr) {
			return RegisterClassW(nullptr);
		}

		WNDCLASSW wndc;

		wndc.style = lpWndClass->style;
		wndc.lpfnWndProc = lpWndClass->lpfnWndProc;
		wndc.cbClsExtra = lpWndClass->cbClsExtra;
		wndc.cbWndExtra = lpWndClass->cbWndExtra;
		wndc.hInstance = lpWndClass->hInstance;
		wndc.hIcon = lpWndClass->hIcon;
		wndc.hCursor = lpWndClass->hCursor;
		wndc.hbrBackground = lpWndClass->hbrBackground;

		std::wstring wszMenuName;
		if (lpWndClass->lpszMenuName != nullptr) {
			::anonymous::utf8_to_wcs(lpWndClass->lpszMenuName, -1, wszMenuName);
			wndc.lpszMenuName = wszMenuName.c_str();
		}

		std::wstring wszClassName;
		if (lpWndClass->lpszClassName != nullptr) {
			::anonymous::utf8_to_wcs(lpWndClass->lpszClassName, -1, wszClassName);
			wndc.lpszClassName = wszClassName.c_str();
		}

		return RegisterClassW(&wndc);
	}

	/**
	 * Opens the specified registry key. Note that key names are not case sensitive.
	 *
	 * @param  hKey       A handle to an open registry key.
	 * @param  lpSubKey   The name of the registry subkey to be opened.
	 * @param  ulOptions  Specifies the option to apply when opening the key.
	 * @param  samDesired A mask that specifies the desired access rights to the key to be opened.
	 * @param  phkResult  A pointer to a variable that receives a handle to the opened key.
	 * @return            If the function succeeds, the return value is ERROR_SUCCESS.
	 * @see               https://learn.microsoft.com/en-us/windows/win32/api/winreg/nf-winreg-regopenkeyexa
	 */
	inline LSTATUS _RegOpenKeyEx(
		_In_ HKEY hKey,
		_In_opt_ LPCSTR lpSubKey,
		_In_opt_ DWORD ulOptions,
		_In_ REGSAM samDesired,
		_Out_ PHKEY phkResult
	) {
		std::wstring wSubKey; ::anonymous::utf8_to_wcs(lpSubKey, -1, wSubKey);
		return RegOpenKeyExW(
			hKey,
			lpSubKey == nullptr ? nullptr : wSubKey.c_str(),
			ulOptions,
			samDesired,
			phkResult
		);
	}

	/**
	 * Retrieves the type and data for the specified value name associated with an open registry key.
	 *
	 * @param  hKey        A handle to an open registry key
	 * @param  lpValueName The name of the registry value.
	 * @param  lpReserved  This parameter is reserved and must be NULL.
	 * @param  lpType      A pointer to a variable that receives a code indicating the type of data stored in the specified value.
	 * @param  lpData      A pointer to a buffer that receives the value's data.
	 * @param  lpcbData    A pointer to a variable that specifies the size of the buffer pointed to by the lpData parameter, in bytes.
	 * @return             If the function succeeds, the return value is ERROR_SUCCESS.
	 * @see                https://learn.microsoft.com/en-us/windows/win32/api/winreg/nf-winreg-regopenkeyexa
	 */
	inline LSTATUS _RegQueryValueEx(
		_In_ HKEY hKey,
		_In_opt_ LPCSTR lpValueName,
		_Reserved_ LPDWORD lpReserved,
		_Out_opt_ LPDWORD lpType,
		_Out_writes_bytes_to_opt_(*lpcbData, *lpcbData) __out_data_source(REGISTRY) LPBYTE lpData,
		_When_(lpData == NULL, _Out_opt_) _When_(lpData != NULL, _Inout_opt_) LPDWORD lpcbData
	) {
		std::wstring wValueName; ::anonymous::utf8_to_wcs(lpValueName, -1, wValueName);
		return RegQueryValueExW(
			hKey,
			lpValueName == nullptr ? nullptr : wValueName.c_str(),
			lpReserved,
			lpType,
			lpData,
			lpcbData
		);
	}

	/**
	 * Sets the data and type of a specified value under a registry key.
	 *
	 * @param  hKey        A handle to an open registry key.
	 * @param  lpValueName The name of the value to be set.
	 * @param  Reserved    This parameter is reserved and must be zero.
	 * @param  dwType      The type of data pointed to by the lpData parameter.
	 * @param  lpData      The data to be stored.
	 * @param  cbData      The size of the information pointed to by the lpData parameter, in bytes.
	 * @return             If the function succeeds, the return value is ERROR_SUCCESS.
	 * @see                https://learn.microsoft.com/en-us/windows/win32/api/winreg/nf-winreg-regsetvalueexa
	 */
	inline LSTATUS _RegSetValueEx(
		_In_ HKEY hKey,
		_In_opt_ LPCSTR lpValueName,
		_Reserved_ DWORD Reserved,
		_In_ DWORD dwType,
		_In_reads_bytes_opt_(cbData) CONST BYTE* lpData,
		_In_ DWORD cbData
	) {
		std::wstring wValueName; ::anonymous::utf8_to_wcs(lpValueName, -1, wValueName);
		return RegSetValueExW(
			hKey,
			lpValueName == nullptr ? nullptr : wValueName.c_str(),
			Reserved,
			dwType,
			lpData,
			cbData
		);
	}

	/**
	 * Changes the text of the specified window's title bar (if it has one).
	 *
	 * @param  hWnd     A handle to the window or control whose text is to be changed.
	 * @param  lpString The new title or control text.
	 * @return          If the function succeeds, the return value is nonzero.
	 *                  If the function fails, the return value is zero. To get extended error information, call GetLastError.
	 * @see             https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-setwindowtexta
	 */
	inline BOOL _SetWindowText(
		_In_ HWND hWnd,
		_In_opt_ LPCSTR lpString
	) {
		if (lpString == nullptr) {
			return SetWindowTextW(hWnd, nullptr);
		}

		std::wstring wString; ::anonymous::utf8_to_wcs(lpString, -1, wString);
		return SetWindowTextW(hWnd, wString.c_str());
	}

	/**
	 * Unregisters a window class, freeing the memory required for the class.
	 *
	 * @param  lpClassName A null-terminated string or a class atom. If lpClassName is a string, it specifies the window class name.
	 * @param  hInstance   A handle to the instance of the module that created the class.
	 * @return             If the function succeeds, the return value is nonzero.
	 * @see                https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-unregisterclassa
	 */
	inline BOOL _UnregisterClass(
		_In_ LPCSTR lpClassName,
		_In_opt_ HINSTANCE hInstance
	) {
		if (lpClassName == nullptr) {
			return UnregisterClassW(nullptr, hInstance);
		}

		std::wstring wClassName; ::anonymous::utf8_to_wcs(lpClassName, -1, wClassName);
		return UnregisterClassW(wClassName.c_str(), hInstance);
	}
}

#endif

namespace cv {
	/**
	 * Get status information on a file.
	 *
	 * @param  path   Pointer to a string containing the path of existing file or directory.
	 * @param  buffer Pointer to structure that stores results.
	 * @return        Returns 0 if the file-status information is obtained.
	 * @see           https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/stat-functions?view=msvc-170
	 */
	inline int stat(const char* path, struct stat* buffer) {
		std::wstring wpath; ::anonymous::utf8_to_wcs(path, -1, wpath);
		struct _stat _buffer;
		auto ret = ::_wstat(wpath.c_str(), &_buffer);

		buffer->st_dev = _buffer.st_dev;
		buffer->st_ino = _buffer.st_ino;
		buffer->st_mode = _buffer.st_mode;
		buffer->st_nlink = _buffer.st_nlink;
		buffer->st_uid = _buffer.st_uid;
		buffer->st_gid = _buffer.st_gid;
		buffer->st_rdev = _buffer.st_rdev;
		buffer->st_size = _buffer.st_size;
		buffer->st_atime = _buffer.st_atime;
		buffer->st_mtime = _buffer.st_mtime;
		buffer->st_ctime = _buffer.st_ctime;

		return ret;
	}

	/**
	 * Opens a file.
	 *
	 * @param  filename File name.
	 * @param  mode     Kind of access that's enabled.
	 * @return          A pointer to the open file.
	 * @see             https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/fopen-wfopen?view=msvc-170
	 */
	inline FILE* fopen(const char* filename, const char* mode) {
		std::wstring wfilename; ::anonymous::utf8_to_wcs(filename, -1, wfilename);
		std::wstring wmode; ::anonymous::utf8_to_wcs(mode, -1, wmode);
		return _wfopen(wfilename.c_str(), wmode.c_str());
	}
}


#endif

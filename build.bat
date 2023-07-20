@ECHO OFF
SETLOCAL enabledelayedexpansion

PUSHD "%~dp0"
CD /d %CD%
SET "PATH=%CD%;%PATH%"
SET "CWD=%CD%"

SET skip_build=0
SET skip_config=0
SET has_prefix=0
SET has_install=0
SET has_source=0
SET TARGET=ALL_BUILD
SET CMAKE_GENERATOR=
SET SOURCE=
SET PREFIX=

SET nparms=20

:GET_OPTS
IF %nparms% == 0 GOTO :MAIN
IF [%1] == [-g] SET skip_build=1
IF [%1] == [--build] SET skip_config=1
IF [%1] == [--install] SET has_install=1
IF [%1] == [-C] (
    SET has_source=1
    SET SOURCE=%2
    SET /a nparms -=1
    SHIFT
    IF %nparms% == 0 GOTO :MAIN
)
IF [%1] == [--prefix] (
    SET has_prefix=1
    SET PREFIX=%2
    SET /a nparms -=1
    SHIFT
    IF %nparms% == 0 GOTO :MAIN
)
IF [%1] == [--target] (
    SET TARGET=%2
    SET /a nparms -=1
    SHIFT
    IF %nparms% == 0 GOTO :MAIN
)
IF [%1] == [-G] (
    IF [%2-%TARGET%] == [Ninja-ALL_BUILD] SET TARGET=all
    SET CMAKE_GENERATOR=-G %2
    SET /a nparms -=1
    SHIFT
    IF %nparms% == 0 GOTO :MAIN
)
IF [%1] == [-A] (
    SET CMAKE_GENERATOR=%CMAKE_GENERATOR% -A %2
    SET /a nparms -=1
    SHIFT
    IF %nparms% == 0 GOTO :MAIN
)
SET /a nparms -=1
SHIFT
GOTO GET_OPTS

:MAIN
IF [%has_source%] == [1] CD /D "%SOURCE%"
SET BUILD_FOLDER=build_x64
IF NOT [%has_prefix%] == [1] SET "PREFIX=%CD%\%BUILD_FOLDER%\install"

IF NOT DEFINED CMAKE_BUILD_TYPE SET CMAKE_BUILD_TYPE=Release
SET EXTRA_CMAKE_OPTIONS=%EXTRA_CMAKE_OPTIONS% -DCMAKE_BUILD_TYPE="%CMAKE_BUILD_TYPE%" -DCMAKE_INSTALL_PREFIX=install

::Find CMake
SET CMAKE="cmake.exe"
IF EXIST "%PROGRAMFILES_DIR_X86%\CMake\bin\cmake.exe" SET CMAKE="%PROGRAMFILES_DIR_X86%\CMake\bin\cmake.exe"
IF EXIST "%PROGRAMFILES_DIR%\CMake\bin\cmake.exe" SET CMAKE="%PROGRAMFILES_DIR%\CMake\bin\cmake.exe"
IF EXIST "%PROGRAMW6432%\CMake\bin\cmake.exe" SET CMAKE="%PROGRAMW6432%\CMake\bin\cmake.exe"

:MAKE
SET ERROR=0

:MAKE_CONFIG
IF [%skip_config%] == [1] GOTO BUILD

IF NOT EXIST %BUILD_FOLDER% MKDIR %BUILD_FOLDER%
cd %BUILD_FOLDER%

%CMAKE% %CMAKE_GENERATOR% %EXTRA_CMAKE_OPTIONS% "%CWD%"
SET ERROR=%ERRORLEVEL%
IF [%ERROR%] == [0] GOTO BUILD
GOTO END

:BUILD
IF [%skip_build%] == [1] GOTO END
IF NOT [%CD%] == [%CWD%\%BUILD_FOLDER%] CD /D "%CWD%\%BUILD_FOLDER%"
%CMAKE% --build . --config %CMAKE_BUILD_TYPE% --target %TARGET%
SET ERROR=%ERRORLEVEL%
IF [%ERROR%] == [0] GOTO INSTALL
GOTO END

:INSTALL
IF NOT [%has_install%] == [1] GOTO END
IF NOT [%CD%] == [%CWD%\%BUILD_FOLDER%] CD /D "%CWD%\%BUILD_FOLDER%"
%CMAKE% --install . --prefix "%PREFIX%"

:END
POPD
EXIT /B %ERROR%

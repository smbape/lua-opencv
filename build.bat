@ECHO OFF
SETLOCAL enabledelayedexpansion

SET WORKING_DIR=%CD%

PUSHD "%~dp0"
CD /D %CD%
SET "PATH=%CD%\bin;%PATH%"
SET "SCRIPTPATH=%CD%"

CD /D "%WORKING_DIR%"

SET skip_build=0
SET skip_config=0
SET has_install=0
SET has_prefix=0
SET is_dry_run=0
SET has_test=0
SET TARGET=ALL_BUILD
SET CMAKE_GENERATOR=

SET nparms=20

:GET_OPTS
IF %nparms% == 0 GOTO :MAIN
IF [%1] == [--dry-run] SET is_dry_run=1
IF [%1] == [--no-build] SET skip_build=1
IF [%1] == [--no-config] SET skip_config=1
IF [%1] == [--test] SET has_test=1
IF [%1] == [-g] (
    SET skip_build=1
    SET skip_config=0
)
IF [%1] == [--build] (
    SET skip_build=0
    SET skip_config=1
)
IF [%1] == [--install] SET has_install=1
IF [%1] == [--prefix] (
    SET has_prefix=1
    SET CMAKE_INSTALL_PREFIX=%2
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
IF NOT DEFINED CMAKE_BUILD_TYPE SET CMAKE_BUILD_TYPE=Debug
IF NOT DEFINED BUILD_FOLDER SET BUILD_FOLDER=%CD%\out\build\x64-%CMAKE_BUILD_TYPE%
IF NOT DEFINED CMAKE_INSTALL_PREFIX SET CMAKE_INSTALL_PREFIX=%CD%\out\install\x64-%CMAKE_BUILD_TYPE%

SET EXTRA_CMAKE_OPTIONS=%EXTRA_CMAKE_OPTIONS% "-DCMAKE_BUILD_TYPE:STRING=%CMAKE_BUILD_TYPE%" "-DCMAKE_INSTALL_PREFIX:PATH=%CMAKE_INSTALL_PREFIX%"

::Find CMake
FOR %%X IN (cmake.exe) do (set CMAKE="%%~$PATH:X")
IF NOT DEFINED CMAKE (
    IF EXIST "%PROGRAMFILES_DIR_X86%\CMake\bin\cmake.exe" SET CMAKE="%PROGRAMFILES_DIR_X86%\CMake\bin\cmake.exe"
    IF EXIST "%PROGRAMFILES_DIR%\CMake\bin\cmake.exe" SET CMAKE="%PROGRAMFILES_DIR%\CMake\bin\cmake.exe"
    IF EXIST "%PROGRAMW6432%\CMake\bin\cmake.exe" SET CMAKE="%PROGRAMW6432%\CMake\bin\cmake.exe"
)

::Find Visual Studio
IF NOT DEFINED VSCMD_VER (
    FOR /F "usebackq tokens=* USEBACKQ" %%F IN (`vswhere.exe -version [16.0^,^) -property installationPath -latest`) DO (
        CALL "%%F\VC\Auxiliary\Build\vcvars64.bat"
        GOTO MAKE
        EXIT /B %ERRORLEVEL%
    )

    FOR /F "usebackq tokens=* USEBACKQ" %%F IN (`vswhere.exe -version [15.0^,16.0^) -property installationPath -latest`) DO (
        CALL "%%F\VC\Auxiliary\Build\vcvars64.bat"
        GOTO MAKE
        EXIT /B %ERRORLEVEL%
    )

    FOR /F "usebackq tokens=* USEBACKQ" %%F IN (`vswhere.exe -legacy -version [10.0^,15.0^) -property installationPath -latest`) DO (
        CALL "%%F\VC\vcvarsall.bat" x64
        GOTO MAKE
        EXIT /B %ERRORLEVEL%
    )
)

:MAKE
SET ERROR=0
SET TRY_RUN=
IF [%is_dry_run%] == [1] SET "TRY_RUN=@ECHO "
IF [%is_dry_run%] == [1] SET "CMAKE=@ECHO %CMAKE%"

:MAKE_CONFIG
IF NOT EXIST %BUILD_FOLDER% MKDIR %BUILD_FOLDER%
%TRY_RUN%CD /D "%BUILD_FOLDER%"

IF [%skip_config%] == [1] GOTO BUILD
%CMAKE% %CMAKE_GENERATOR% %EXTRA_CMAKE_OPTIONS% "%SCRIPTPATH%"
SET ERROR=%ERRORLEVEL%
IF [%ERROR%] == [0] GOTO BUILD
GOTO END

:BUILD
IF [%skip_build%] == [1] GOTO INSTALL
%CMAKE% --build . --config %CMAKE_BUILD_TYPE% --target %TARGET%
SET ERROR=%ERRORLEVEL%
IF [%ERROR%] == [0] GOTO INSTALL
GOTO END

:INSTALL
IF NOT [%has_install%] == [1] GOTO TEST
%CMAKE% --install . --prefix "%CMAKE_INSTALL_PREFIX%"
IF [%ERROR%] == [0] GOTO TEST
GOTO END

:TEST
IF NOT [%has_test%] == [1] GOTO END
%TRY_RUN%%CMAKE:cmake.exe=ctest.exe% -C %CMAKE_BUILD_TYPE%
%TRY_RUN%"%CMAKE_INSTALL_PREFIX%\bin\luajit.exe" "%SCRIPTPATH%\test\test.lua"

:END
POPD
EXIT /B %ERROR%

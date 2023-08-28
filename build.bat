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
SET has_generator=1
SET CMAKE_GENERATOR=-G Ninja
SET TARGET=all

SET nparms=20

:GET_OPTS
IF %nparms% == 0 GOTO :MAIN
IF [%1] == [-d] SET CMAKE_BUILD_TYPE=Debug
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
    SET has_generator=1
    SET /a nparms -=1
    SHIFT
    IF %nparms% == 0 GOTO :MAIN
)
IF [%1] == [-A] (
    SET CMAKE_GENERATOR_PLATFORM=-A %2
    SET /a nparms -=1
    SHIFT
    IF %nparms% == 0 GOTO :MAIN
)
IF [%1] == [--config] (
    SET CONFIG_NAME=%2
    SET /a nparms -=1
    SHIFT
    IF %nparms% == 0 GOTO :MAIN
)
SET /a nparms -=1
SHIFT
GOTO GET_OPTS

:MAIN
IF ["%CMAKE_GENERATOR%-%TARGET%"] == ["-G Ninja-ALL_BUILD"] SET TARGET=all
IF NOT DEFINED CMAKE_BUILD_TYPE SET CMAKE_BUILD_TYPE=Release
IF NOT DEFINED CONFIG_NAME SET CONFIG_NAME=x64-%CMAKE_BUILD_TYPE%
IF NOT DEFINED BUILD_FOLDER SET BUILD_FOLDER=%CD%\out\build\%CONFIG_NAME%
IF NOT DEFINED PREFIX SET PREFIX=%CD%\out\install\%CONFIG_NAME%

SET EXTRA_CMAKE_OPTIONS=%EXTRA_CMAKE_OPTIONS% "-DCMAKE_BUILD_TYPE:STRING=%CMAKE_BUILD_TYPE%" "-DCMAKE_INSTALL_PREFIX:PATH=%PREFIX%"

IF DEFINED VSCMD_VER GOTO Set_Generator

::Find Visual Studio
FOR /F "usebackq tokens=* USEBACKQ" %%F IN (`vswhere.exe -legacy -version [10.0^,^) -property installationVersion -latest`) DO SET VS_VERSION=%%F
FOR /F "usebackq tokens=* USEBACKQ" %%F IN (`vswhere.exe -legacy -version [10.0^,^) -property catalog_productLineVersion -latest`) DO SET VS_PRODUCT_VERSION=%%F
FOR /F "usebackq tokens=* USEBACKQ" %%F IN (`vswhere.exe -version [16.0^,^) -property installationPath -latest`) DO (
    IF NOT [%has_generator%] == [1] (
        SET CMAKE_GENERATOR=-G "Visual Studio %VS_VERSION:~0,2% %VS_PRODUCT_VERSION%"
        SET CMAKE_GENERATOR_PLATFORM=-A x64
    )
    SET has_generator=1
    CALL "%%F\VC\Auxiliary\Build\vcvars64.bat"
    GOTO MAKE
    EXIT /B %ERRORLEVEL%
)

FOR /F "usebackq tokens=* USEBACKQ" %%F IN (`vswhere.exe -version [15.0^,16.0^) -property installationPath -latest`) DO (
    IF NOT [%has_generator%] == [1] (
        SET CMAKE_GENERATOR=-G "Visual Studio %VS_VERSION:~0,2% %VS_PRODUCT_VERSION%""
        SET CMAKE_GENERATOR_PLATFORM=Win64"
    )
    SET has_generator=1
    CALL "%%F\VC\Auxiliary\Build\vcvars64.bat"
    GOTO MAKE
    EXIT /B %ERRORLEVEL%
)

FOR /F "usebackq tokens=* USEBACKQ" %%F IN (`vswhere.exe -legacy -version [10.0^,15.0^) -property installationPath -latest`) DO (
    IF NOT [%has_generator%] == [1] (
        SET CMAKE_GENERATOR=-G "Visual Studio %VS_VERSION:~0,2% %VS_PRODUCT_VERSION%""
        SET CMAKE_GENERATOR_PLATFORM=Win64"
    )
    SET has_generator=1
    CALL "%%F\VC\vcvarsall.bat" x64
    GOTO MAKE
    EXIT /B %ERRORLEVEL%
)

:Set_Generator
IF NOT DEFINED CMAKE_GENERATOR SET CMAKE_GENERATOR_PLATFORM=-A %VSCMD_ARG_TGT_ARCH%
IF NOT DEFINED CMAKE_GENERATOR SET CMAKE_GENERATOR=-G "Visual Studio %VSCMD_VER:~0,2%"

:MAKE
::Find CMake
REM CMAKE="%DevEnvDir%\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe"
SET "PATH=%DevEnvDir%\CommonExtensions\Microsoft\CMake\CMake\bin;%PATH%"
FOR %%X IN (cmake.exe) DO (set CMAKE="%%~$PATH:X")
IF NOT DEFINED CMAKE (
    IF EXIST "%PROGRAMFILES_DIR_X86%\CMake\bin\cmake.exe" SET CMAKE="%PROGRAMFILES_DIR_X86%\CMake\bin\cmake.exe"
    IF EXIST "%PROGRAMFILES_DIR%\CMake\bin\cmake.exe" SET CMAKE="%PROGRAMFILES_DIR%\CMake\bin\cmake.exe"
    IF EXIST "%PROGRAMW6432%\CMake\bin\cmake.exe" SET CMAKE="%PROGRAMW6432%\CMake\bin\cmake.exe"
)

::Find Ninja
REM -DCMAKE_MAKE_PROGRAM="%DevEnvDir%\CommonExtensions\Microsoft\CMake\Ninja\ninja.exe"
SET "PATH=%DevEnvDir%\CommonExtensions\Microsoft\CMake\Ninja;%PATH%"
FOR %%X IN (ninja.exe) DO (set NINJA="%%~$PATH:X")
IF ["%CMAKE_GENERATOR%"] == ["-G Ninja"] SET EXTRA_CMAKE_OPTIONS=%EXTRA_CMAKE_OPTIONS% -DCMAKE_MAKE_PROGRAM=%NINJA%

SET ERROR=0
SET TRY_RUN=
IF [%is_dry_run%] == [1] SET "TRY_RUN=@ECHO "
IF [%is_dry_run%] == [1] SET "CMAKE=@ECHO %CMAKE%"

:MAKE_CONFIG
IF NOT EXIST %BUILD_FOLDER% MKDIR %BUILD_FOLDER%
%TRY_RUN%CD /D "%BUILD_FOLDER%"

IF [%skip_config%] == [1] GOTO BUILD
%CMAKE% %CMAKE_GENERATOR% %CMAKE_GENERATOR_PLATFORM% %EXTRA_CMAKE_OPTIONS% "%SCRIPTPATH%"
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
%CMAKE% --install . --prefix "%PREFIX%"
SET ERROR=%ERRORLEVEL%
IF [%ERROR%] == [0] GOTO TEST
GOTO END

:TEST
IF NOT [%has_test%] == [1] GOTO END
%TRY_RUN%%CMAKE:cmake.exe=ctest.exe% -C %CMAKE_BUILD_TYPE% -R test_build
SET ERROR=%ERRORLEVEL%
IF NOT [%ERROR%] == [0] GOTO END
IF EXIST "%PREFIX%\bin\luajit.exe" %TRY_RUN%%CMAKE:cmake.exe=ctest.exe% -C %CMAKE_BUILD_TYPE% -R test_install
SET ERROR=%ERRORLEVEL%
IF NOT [%ERROR%] == [0] GOTO END

:END
POPD
EXIT /B %ERROR%

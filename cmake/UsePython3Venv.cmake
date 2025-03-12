# https://discourse.cmake.org/t/possible-to-create-a-python-virtual-env-from-cmake-and-then-find-it-with-findpython3/1132
set(VIRTUAL_ENV "${CMAKE_CURRENT_BINARY_DIR}/.venv")

if (NOT EXISTS "${VIRTUAL_ENV}")
    set(VIRTUAL_ENV_created TRUE)
elseif(NOT DEFINED VIRTUAL_ENV_created)
    set(VIRTUAL_ENV_created FALSE)
endif()

find_package(Python3 "3.9...<3.13" COMPONENTS Interpreter REQUIRED)
execute_process (COMMAND "${Python3_EXECUTABLE}" -m venv "${VIRTUAL_ENV}")

## update the environment with VIRTUAL_ENV variable (mimic the activate script)
set(ENV{VIRTUAL_ENV} "${VIRTUAL_ENV}")
# file(TO_NATIVE_PATH "${VIRTUAL_ENV}/Scripts" CMAKE_MSVCIDE_RUN_PATH)

## change the context of the search
set(Python3_FIND_VIRTUALENV FIRST)

## unset Python3_EXECUTABLE because it is also an input variable (see documentation, Artifacts Specification section)
unset(Python3_EXECUTABLE)

## Launch a new search
find_package(Python3 COMPONENTS Interpreter REQUIRED)

if (VIRTUAL_ENV_created)
    execute_process(COMMAND "${Python3_EXECUTABLE}" -m pip install --upgrade pip)
endif()

# Make sure custom commands use venv for python executable
cmake_path(GET Python3_EXECUTABLE PARENT_PATH Python3_BINDIR)
cmake_path(CONVERT "$ENV{PATH}" TO_NATIVE_PATH_LIST Python3_PATH NORMALIZE)
cmake_path(NATIVE_PATH Python3_BINDIR NORMALIZE Python3_BINDIR)
list(PREPEND Python3_PATH "${Python3_BINDIR}")
if (NOT WIN32)
    string(REPLACE ";" ":" Python3_PATH "${Python3_PATH}")
endif()
set(ENV{PATH} "${Python3_PATH}")

execute_process(COMMAND "${Python3_EXECUTABLE}" -m pip list -l --format=freeze OUTPUT_VARIABLE Pip_MODULES)
string(REPLACE "\n" ";" Pip_MODULES "${Pip_MODULES}")
list(TRANSFORM Pip_MODULES REPLACE "=.+" "")

function(pip_install)
    set(to_install)

    foreach(module IN LISTS ARGN)
        if (NOT module IN_LIST Pip_MODULES)
            list(APPEND to_install ${module})
        endif()
    endforeach()

    if (to_install)
        execute_process(
            COMMAND "${Python3_EXECUTABLE}" -m pip install ${to_install}
            COMMAND_ECHO STDERR
            COMMAND_ERROR_IS_FATAL ANY
        )
        list(APPEND Pip_MODULES ${to_install})
    endif()
endfunction()

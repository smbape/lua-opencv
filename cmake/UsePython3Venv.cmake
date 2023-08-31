# https://discourse.cmake.org/t/possible-to-create-a-python-virtual-env-from-cmake-and-then-find-it-with-findpython3/1132
set(VIRTUAL_ENV "${CMAKE_CURRENT_BINARY_DIR}/.venv")
if (NOT EXISTS "${VIRTUAL_ENV}")
    set(VIRTUAL_ENV_created TRUE)
else()
    set(VIRTUAL_ENV_created FALSE)
endif()
find_package(Python3 COMPONENTS Interpreter REQUIRED)
execute_process (COMMAND "${Python3_EXECUTABLE}" -m venv "${VIRTUAL_ENV}")

## update the environment with VIRTUAL_ENV variable (mimic the activate script)
set(ENV{VIRTUAL_ENV} "${VIRTUAL_ENV}")
# file(TO_NATIVE_PATH "${VIRTUAL_ENV}/Scripts" CMAKE_MSVCIDE_RUN_PATH)

## change the context of the search
set(Python3_FIND_VIRTUALENV FIRST)

## unset Python3_EXECUTABLE because it is also an input variable (see documentation, Artifacts Specification section)
unset(Python3_EXECUTABLE)

## Launch a new search
find_package(Python3 COMPONENTS Interpreter Development REQUIRED)

string(REPLACE / // PYTHON_BIN_PATH "${Python3_EXECUTABLE}")

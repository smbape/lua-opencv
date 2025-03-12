find_package(Git)

# opencv/cmake/OpenCVUtils.cmake#1985
macro(lua_bindings_git_describe var_name path)
    if(GIT_FOUND)
        execute_process(COMMAND "${GIT_EXECUTABLE}" describe --tags --exact-match --dirty
            WORKING_DIRECTORY "${path}"
            OUTPUT_VARIABLE ${var_name}
            RESULT_VARIABLE GIT_RESULT
            ERROR_QUIET
            OUTPUT_STRIP_TRAILING_WHITESPACE
        )
        if(NOT GIT_RESULT EQUAL 0)
            execute_process(COMMAND "${GIT_EXECUTABLE}" describe --tags --always --dirty --match "[0-9].[0-9].[0-9]*" --exclude "[^-]*-cvsdk"
                WORKING_DIRECTORY "${path}"
                OUTPUT_VARIABLE ${var_name}
                RESULT_VARIABLE GIT_RESULT
                ERROR_QUIET
                OUTPUT_STRIP_TRAILING_WHITESPACE
            )
            if(NOT GIT_RESULT EQUAL 0)  # --exclude is not supported by 'git'
                # match only tags with complete OpenCV versions (ignores -alpha/-beta/-rc suffixes)
                execute_process(COMMAND "${GIT_EXECUTABLE}" describe --tags --always --dirty --match "[0-9].[0-9]*[0-9]"
                    WORKING_DIRECTORY "${path}"
                    OUTPUT_VARIABLE ${var_name}
                    RESULT_VARIABLE GIT_RESULT
                    ERROR_QUIET
                    OUTPUT_STRIP_TRAILING_WHITESPACE
                )
                if(NOT GIT_RESULT EQUAL 0)
                    set(${var_name} "unknown")
                endif()
            endif()
        endif()
    else()
        set(${var_name} "unknown")
    endif()
endmacro()

lua_bindings_git_describe(PROJECT_VCSVERSION "${CMAKE_CURRENT_SOURCE_DIR}/..")

execute_process(
    COMMAND "${NODE_EXECUTABLE}" -pe "require('./package').version"
    WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
    OUTPUT_VARIABLE PROJECT_VERSION
    COMMAND_ECHO STDERR
    OUTPUT_STRIP_TRAILING_WHITESPACE
    COMMAND_ERROR_IS_FATAL ANY
)

if (PROJECT_VCSVERSION MATCHES "${PROJECT_VERSION}.*")
    set(LUA_MODULE_VERSION "${PROJECT_VCSVERSION}")
else()
    set(LUA_MODULE_VERSION "${PROJECT_VERSION}-${PROJECT_VCSVERSION}")
endif()

set(LUA_MODULE_VERSION "${PROJECT_VERSION}")

string(REPLACE "." ";" PROJECT_VERSION_LIST ${PROJECT_VERSION})
list(GET PROJECT_VERSION_LIST 0 PROJECT_VERSION_MAJOR)
list(GET PROJECT_VERSION_LIST 1 PROJECT_VERSION_MINOR)
list(GET PROJECT_VERSION_LIST 2 PROJECT_VERSION_PATCH)

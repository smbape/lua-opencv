message(STATUS "PostInstall: ${TARGET_FILE}")

# Make sure this was requested.
if(CMAKE_INSTALL_PREFIX)
    set(msg)
    # CMake builds a manifest of all files it has installed.
    foreach(file ${CMAKE_INSTALL_MANIFEST_FILES})
        # Make a list of installed files to compare.
        get_filename_component(nm ${file} NAME)
        list(APPEND fileindex ${nm})
        set(msg "${msg}\n    ${file}")
    endforeach()
    message(STATUS "PostInstall: ${msg}")
endif()

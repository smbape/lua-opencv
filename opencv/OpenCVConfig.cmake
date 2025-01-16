if ((NOT DEFINED OpenCV_DIR) AND (DEFINED ENV{OpenCV_DIR}))
  find_path (
    OpenCV_DIR
    "OpenCVConfig.cmake"
    PATHS ENV OpenCV_DIR
    PATH_SUFFIXES
      lib64/cmake/opencv4
      lib/cmake/opencv4
    DOC "Root directory of OpenCV"
    REQUIRED
  )
endif()

if (UNIX AND (NOT APPLE))
  set(WITH_QT "5" CACHE STRING "Build with Qt Backend support")
endif()

if ((NOT DEFINED OpenCV_DIR) AND (NOT DEFINED OpenCV_LIBS))
  # Compute the subdirectory prefix relative to this file.
  get_filename_component(_IMPORT_PREFIX "${CMAKE_CURRENT_LIST_FILE}" DIRECTORY)
  add_subdirectory("${_IMPORT_PREFIX}")

  set(OpenCV_LINK_LIBRARIES ${OpenCV_LIBS})
  set(OpenCV_SOURCE_OBJECTS)

  # We want the opencv lua module to be used as a replacement of opencv
  # when another lua module depends on opencv.
  # Linux lua expects shared libraries as modules (.so)
  # Windows lua exepects a .dll, which can be a static or a shared library.
  # For the opencv lua module to be used as a replcement of opencv,
  # it neeeds to have all the exported opencv functions.
  # One way to achieve that purpose is to build opencv as a static library.
  # However, on Windows, that will force every dependant module to also be a static library,
  # Which will lead to a repetitions of opencv functions beteween the lua opencv module and it's dependants.
  # Which will also lead to an increase of the size of the dependant modules.
  # The solution choosen is to directly embed the opencv objects in the opencv lua module.
  # That will allow the opencv lua module shared library to have all the exported opencv functions,
  # and more importantly, work on Linux and Windows
  if (BUILD_SHARED_LIBS)
    foreach(the_module ${OpenCV_LIBS})
      get_target_property(__cmake_binary_dir ${the_module} BINARY_DIR)
      get_target_property(__cmake_source_dir ${the_module} SOURCE_DIR)
      get_target_property(__cmake_sources ${the_module} SOURCES)
      get_target_property(__cmake_imported ${the_module} IMPORTED)

      set(__cmake_c_compiled_objects ${__cmake_sources})

      # Include files with c extensions
      list(FILTER __cmake_c_compiled_objects INCLUDE REGEX "\\.c(c|pp|xx)?$")

      # Exclude files that are intended for standalone executables
      if (WIN32
        AND NOT (
          "${the_module}" STREQUAL "opencv_core" OR
          "${the_module}" STREQUAL "opencv_world" OR
          "${the_module}" STREQUAL "opencv_cudev"
        )
        AND (BUILD_SHARED_LIBS AND NOT "x${OPENCV_MODULE_TYPE}" STREQUAL "xSTATIC")
        AND NOT OPENCV_SKIP_DLLMAIN_GENERATION
      )
        list(FILTER __cmake_c_compiled_objects EXCLUDE REGEX "/${the_module}_main.cpp$")
      endif()

      # File have their object paths relative to the binary dir
      list_cmake_path(RELATIVE_PATH __cmake_c_compiled_objects BASE_DIRECTORY ${__cmake_source_dir} OUTPUT_VARIABLE)

      # Files that are already in the binary dir keep their object path relative to the binary dir
      cmake_path(RELATIVE_PATH __cmake_binary_dir BASE_DIRECTORY ${__cmake_source_dir} OUTPUT_VARIABLE __cmake_binary_dir_relative)
      list_string(REPLACE "${__cmake_binary_dir_relative}/" "" __cmake_c_compiled_objects)

      # The ../ is replaced by __/ when generating the objects files
      list_string(REPLACE "../" "__/" __cmake_c_compiled_objects)

      # Object files are generated with ${CMAKE_C_OUTPUT_EXTENSION} extensions
      list(TRANSFORM __cmake_c_compiled_objects APPEND ${CMAKE_C_OUTPUT_EXTENSION})

      # The object file path is relative to ${__cmake_binary_dir}/CMakeFiles/${the_module}.dir/
      list(TRANSFORM __cmake_c_compiled_objects PREPEND "${__cmake_binary_dir}/CMakeFiles/${the_module}.dir/")

      # Mark the object files as GENERATED because they will not exists until opencv is built
      list(APPEND OpenCV_SOURCE_OBJECTS ${__cmake_c_compiled_objects})
      set_source_files_properties(${__cmake_c_compiled_objects} PROPERTIES GENERATED TRUE)
      unset(__cmake_c_compiled_objects)

      # Keep object files already in the sources
      set(__cmake_imported_objects ${__cmake_sources})
      list(FILTER __cmake_imported_objects INCLUDE REGEX "\\${CMAKE_C_OUTPUT_EXTENSION}$")
      list(APPEND OpenCV_SOURCE_OBJECTS ${__cmake_imported_objects})
      unset(__cmake_imported_objects)

      foreach(__property_name LINK_LIBRARIES)
        get_target_property(__property_value ${the_module} INTERFACE_${__property_name})
        if("${__property_value}" STREQUAL "__property_value-NOTFOUND")
          get_target_property(__property_value ${the_module} ${__property_name})
          if("${__property_value}" STREQUAL "__property_value-NOTFOUND")
            unset(__property_value)
            continue()
          endif()
        endif()

        list(APPEND OpenCV_${__property_name} ${__property_value})
        unset(__property_value)
      endforeach()

      unset(__cmake_binary_dir)
      unset(__cmake_source_dir)
      unset(__cmake_sources)
    endforeach()

    # Exclude OpenCV_LIBS from LINK_LIBRARIES
    list(JOIN OpenCV_LIBS "|" __link_libraries_excluded)
    list(FILTER OpenCV_LINK_LIBRARIES EXCLUDE REGEX "^(${__link_libraries_excluded})$")
    unset(__link_libraries_excluded)

    list(REMOVE_DUPLICATES OpenCV_LINK_LIBRARIES)
    list(REMOVE_DUPLICATES OpenCV_SOURCE_OBJECTS)

    source_group("OpenCV Source Objects" FILES ${OpenCV_SOURCE_OBJECTS})

    set(__imported_libraries)
    set(__namespace)

    foreach(target_name IN LISTS OpenCV_LINK_LIBRARIES)
      if (TARGET "${target_name}")
        get_target_property(__lib_imported "${target_name}" IMPORTED)
        if (__lib_imported)
          set(__library_opts "${__namespace}${target_name}")

          get_target_property(__lib_imported_type "${target_name}" TYPE)
          string(REPLACE "_LIBRARY" "" __lib_imported_type "${__lib_imported_type}")
          list( APPEND __library_opts "${__lib_imported_type}")
          unset(__lib_imported_type)

          list(APPEND __library_opts IMPORTED)

          get_target_property(__lib_imported_global "${target_name}" IMPORTED_GLOBAL)
          if (__lib_imported_global)
            list(APPEND __library_opts GLOBAL)
          endif()
          unset(__lib_imported_global)

          string(REPLACE ";" " " __library_opts "${__library_opts}")

          list(APPEND __imported_libraries "") # empty line
          list(APPEND __imported_libraries "# Create imported target ${__namespace}${target_name}")
          list(APPEND __imported_libraries "add_library(${__library_opts})")
          list(APPEND __imported_libraries "set_target_properties(${__namespace}${target_name} PROPERTIES")

          foreach(__property IN ITEMS
            IMPORTED_CONFIGURATIONS
            IMPORTED_IMPLIB
            IMPORTED_LINK_INTERFACE_LIBRARIES
            IMPORTED_LOCATION
            INTERFACE_COMPILE_DEFINITIONS
            INTERFACE_INCLUDE_DIRECTORIES
            INTERFACE_LINK_LIBRARIES
            INTERFACE_SYSTEM_INCLUDE_DIRECTORIES
          )
            get_target_property(__variable ${target_name} ${__property})
            if(NOT "${__variable}" STREQUAL "__variable-NOTFOUND")
              string(REPLACE "\"" "\\\"" __variable "${__variable}")
              string(REPLACE ";" "$<SEMICOLON>" __variable "${__variable}")
              list(APPEND __imported_libraries "  ${__property} \"${__variable}\"")
            endif()
            unset(__variable)
          endforeach()

          get_target_property(__configurations ${target_name} IMPORTED_CONFIGURATIONS)
          if(NOT "${__configurations}" STREQUAL "__configurations-NOTFOUND")
            foreach(__configuration IN LISTS __configurations)
              foreach(__property IN ITEMS
                IMPORTED_IMPLIB
                IMPORTED_LINK_DEPENDENT_LIBRARIES
                IMPORTED_LOCATION
              )
                get_target_property(__variable ${target_name} ${__property}_${__configuration})
                if(NOT "${__variable}" STREQUAL "__variable-NOTFOUND")
                  string(REPLACE "\"" "\\\"" __variable "${__variable}")
                  string(REPLACE ";" "$<SEMICOLON>" __variable "${__variable}")
                  list(APPEND __imported_libraries "  ${__property}_${__configuration} \"${__variable}\"")
                endif()
                unset(__variable)
              endforeach()
            endforeach()
          endif()
          unset(__configurations)

          list(APPEND __imported_libraries ")")
        else()
          install(TARGETS "${target_name}"
            EXPORT OpenCVLinkedTargets
            LIBRARY DESTINATION objects/lib
            ARCHIVE DESTINATION objects/lib
          )
        endif()
        unset(__lib_imported)
      endif()
    endforeach()

    list(JOIN __imported_libraries "\n" __imported_libraries)
    string(REPLACE "$<SEMICOLON>" ";" __imported_libraries "${__imported_libraries}")
    file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/OpenCVImportedTargets.cmake" "${__imported_libraries}\n")
    install(FILES "${CMAKE_CURRENT_BINARY_DIR}/OpenCVImportedTargets.cmake" DESTINATION objects)

    unset(__namespace)
    unset(__imported_libraries)

    # Install link libraries that are targets
    install(EXPORT OpenCVLinkedTargets
      FILE OpenCVLinkedTargets.cmake
      DESTINATION objects
    )

    set(__opencv_objects_targets
      "include(\"\${CMAKE_CURRENT_LIST_DIR}/OpenCVLinkedTargets.cmake\")"
      "include(\"\${CMAKE_CURRENT_LIST_DIR}/OpenCVImportedTargets.cmake\")"
    )

    # Export OBJECTS library to allow usage outside
    list(APPEND __opencv_objects_targets "") # empty line
    list(APPEND __opencv_objects_targets "set(OpenCV_SOURCE_OBJECTS")
    set(__lines ${OpenCV_SOURCE_OBJECTS})
    list_double_quote(__lines)
    list(JOIN __lines "\n    " __lines)
    list(APPEND __opencv_objects_targets "    ${__lines}")
    list(APPEND __opencv_objects_targets ")")
    unset(__lines)

    # Keep reference to other link libraries
    list(APPEND __opencv_objects_targets "") # empty line
    list(APPEND __opencv_objects_targets "set(OpenCV_LIBS")
    set(__lines ${OpenCV_LINK_LIBRARIES})
    list_double_quote(__lines)
    list(JOIN __lines "\n    " __lines)
    list(APPEND __opencv_objects_targets "    ${__lines}")
    list(APPEND __opencv_objects_targets ")")
    unset(__lines)

    list(JOIN __opencv_objects_targets "\n" __opencv_objects_targets)
    file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/OpenCVObjects.cmake" "${__opencv_objects_targets}\n")
    install(FILES "${CMAKE_CURRENT_BINARY_DIR}/OpenCVObjects.cmake" DESTINATION objects)

    unset(__opencv_objects_targets)

    set(OpenCV_LIBS ${OpenCV_LINK_LIBRARIES})
    unset(OpenCV_LINK_LIBRARIES)
  endif()
else()
  macro(ocv_find_package_Qt4)
    find_package(Qt4 COMPONENTS QtCore QtGui QtTest ${ARGN})
    if(QT4_FOUND)
      set(QT_FOUND 1)
    endif()
  endmacro()

  macro(ocv_find_package_Qt OCV_QT_VER)
    find_package(Qt${OCV_QT_VER} COMPONENTS Core Gui Widgets Test Concurrent ${ARGN} NO_MODULE)
    if(Qt${OCV_QT_VER}_FOUND)
      set(QT_FOUND 1)
    endif()
  endmacro()

  if(WITH_QT)
    if(NOT WITH_QT GREATER 0)
      # BUG: Qt5Config.cmake script can't handle components properly: find_package(QT NAMES Qt6 Qt5 REQUIRED NO_MODULE COMPONENTS Core Gui Widgets Test Concurrent)
      ocv_find_package_Qt(6 QUIET)
      if(NOT QT_FOUND)
        ocv_find_package_Qt(5 QUIET)
      endif()
      if(NOT QT_FOUND)
        ocv_find_package_Qt4(REQUIRED)
      endif()
    elseif(WITH_QT EQUAL 4)
      ocv_find_package_Qt4(REQUIRED)
    else()  # WITH_QT=<major version>
      ocv_find_package_Qt("${WITH_QT}" REQUIRED)
    endif()
  endif()

  if (BUILD_contrib)
    find_package(Iconv QUIET)
  endif()

  find_package(OpenCV REQUIRED)

  set(OpenCV_SOURCE_OBJECTS)
  if (BUILD_SHARED_LIBS AND EXISTS "${OpenCV_DIR}/objects/OpenCVObjects.cmake")
    include("${OpenCV_DIR}/objects/OpenCVObjects.cmake")
    source_group("OpenCV Source Objects" FILES ${OpenCV_SOURCE_OBJECTS})


  # When built as a static library,
  # only opencv_world library is created
  # when opencv_world is part of the modules
  elseif (NOT OpenCV_SHARED AND ";${OpenCV_LIBS};" MATCHES ";opencv_world;")
    set(_OpenCV_LIBS opencv_world)

    set(opencv_sfm_IS_NOT_PART_OF_WORLD TRUE)
    set(opencv_img_hash_IS_NOT_PART_OF_WORLD TRUE)

    foreach(the_module ${OpenCV_LIBS})
      if(${the_module}_IS_NOT_PART_OF_WORLD)
        list(APPEND _OpenCV_LIBS ${the_module})
      endif()
    endforeach()
    set(OpenCV_LIBS ${_OpenCV_LIBS})
    unset(_OpenCV_LIBS)
  endif()
endif()

# message(STATUS "OpenCV library status:")
# message(STATUS "    config: ${OpenCV_DIR}")
# message(STATUS "    version: ${OpenCV_VERSION}")
# list_print(OpenCV_LIBS "    libraries: " "\n                    ")
# list_print(OpenCV_INCLUDE_DIRS "    include path: " "\n                       ")

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

function(get_c_sources output _c_sources)
  # Include files with c extensions
  set(c_sources "${${_c_sources}}")
  list(FILTER c_sources INCLUDE REGEX "\\.c(c|pp|xx)?$")

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
    list(FILTER c_sources EXCLUDE REGEX "/${the_module}_main.cpp$")
  endif()

  set(${output} "${c_sources}" PARENT_SCOPE)
endfunction()

function(get_object_file output_var the_module c_file source_dir binary_dir)
  set(c_object "${c_file}")

  set(DUPLICATED FALSE)
  if (ARGC GREATER 5)
    set(DUPLICATED "${ARGV5}")
  endif()

  if (CMAKE_GENERATOR MATCHES "Visual Studio" AND NOT DUPLICATED)
    # Only the filename is relevant
    cmake_path(GET c_object FILENAME c_object_filename)

    # The extension is removed
    string(REGEX REPLACE "\\.c(c|pp|xx)?$" "" c_object_filename "${c_object_filename}")

    # The object file path is relative to ${binary_dir}/${the_module}.dir/${CMAKE_BUILD_TYPE}/
    set(c_object "${binary_dir}/${the_module}.dir/${CMAKE_BUILD_TYPE}/${c_object_filename}")
  else()
    # File have their object paths relative to the binary dir
    cmake_path(RELATIVE_PATH c_object BASE_DIRECTORY "${source_dir}" OUTPUT_VARIABLE c_object)

    # Files that are already in the binary dir keep their object path relative to the binary dir
    cmake_path(RELATIVE_PATH binary_dir BASE_DIRECTORY "${source_dir}" OUTPUT_VARIABLE binary_dir_relative)
    string(REPLACE "${binary_dir_relative}/" "" c_object "${c_object}")

    # The ../ is replaced by __/ when generating the objects files
    string(REPLACE "../" "__/" c_object "${c_object}")

    if (CMAKE_GENERATOR MATCHES "Visual Studio")
      # The object file path is relative to ${binary_dir}/${the_module}.dir/${CMAKE_BUILD_TYPE}/
      set(c_object "${binary_dir}/${the_module}.dir/${CMAKE_BUILD_TYPE}/${c_object}")
    else()
      # The object file path is relative to ${binary_dir}/CMakeFiles/${the_module}.dir/
      set(c_object "${binary_dir}/CMakeFiles/${the_module}.dir/${c_object}")
    endif()
  endif()

  # Object files are generated with ${CMAKE_C_OUTPUT_EXTENSION} extensions
  set(${output_var} "${c_object}${CMAKE_C_OUTPUT_EXTENSION}" PARENT_SCOPE)
endfunction()

function(transform_to_object_file
  _c_objects_unique
  the_module
  _c_files
  source_dir
  binary_dir

  _c_objects_duplicated
  _sources_unique
  _filenames_duplicated
  _filenames_unique
)
  set(c_objects_unique "${${_c_objects_unique}}")

  if (CMAKE_GENERATOR MATCHES "Visual Studio")
    set(c_objects_duplicated "${${_c_objects_duplicated}}")
    set(sources_unique "${${_sources_unique}}")
    set(filenames_duplicated "${${_filenames_duplicated}}")
    set(filenames_unique "${${_filenames_unique}}")

    foreach(c_file IN LISTS ${_c_files})
      cmake_path(GET c_file FILENAME c_file_filename)
      list(FIND filenames_duplicated "${c_file_filename}" index_duplicated)
      list(FIND filenames_unique "${c_file_filename}" index_unique)

      set(duplicated TRUE)
      if (index_duplicated EQUAL -1 AND index_unique EQUAL -1)
        set(duplicated FALSE)
      endif()

      get_object_file(c_object ${the_module} "${c_file}" "${source_dir}" "${binary_dir}" ${duplicated})

      # If the new file name is already duplicated
      if (NOT index_duplicated EQUAL -1)
        # add it to duplicated
        list(APPEND c_objects_duplicated "${c_object}")

      # Else If the new file name is newly duplicated
      elseif (NOT index_unique EQUAL -1)
        list(GET sources_unique ${index_unique} source_duplicated)
        get_object_file(c_object_duplicated ${the_module} "${source_duplicated}" "${source_dir}" "${binary_dir}" TRUE)

        # remove the old file from unique
        list(REMOVE_AT c_objects_unique ${index_unique})
        list(REMOVE_AT sources_unique ${index_unique})
        list(REMOVE_AT filenames_unique ${index_unique})

        # add the old file to duplicated
        list(APPEND c_objects_duplicated "${c_object_duplicated}")

        # add the new file to duplicated
        list(APPEND c_objects_duplicated "${c_object}")
        list(APPEND filenames_duplicated "${c_file_filename}")

      # Else add it to unique
      else()
        list(APPEND c_objects_unique "${c_object}")
        list(APPEND sources_unique "${c_file}")
        list(APPEND filenames_unique "${c_file_filename}")
      endif()
    endforeach()

    set(${_c_objects_duplicated} "${c_objects_duplicated}" PARENT_SCOPE)
    set(${_sources_unique} "${sources_unique}" PARENT_SCOPE)
    set(${_filenames_duplicated} "${filenames_duplicated}" PARENT_SCOPE)
    set(${_filenames_unique} "${filenames_unique}" PARENT_SCOPE)
  else()
    foreach(c_file IN LISTS ${_c_files})
      get_object_file(c_object ${the_module} "${c_file}" "${source_dir}" "${binary_dir}")
      list(APPEND c_objects_unique "${c_object}")
    endforeach()
  endif()

  set(${_c_objects_unique} "${c_objects_unique}" PARENT_SCOPE)
endfunction()

function(save_variable lines_output_var variable_name)
  list(APPEND ${lines_output_var} " ") # empty line
  list(APPEND ${lines_output_var} "set(${variable_name}")
  set(__lines "${${variable_name}}")
  list_double_quote(__lines)
  list(JOIN __lines "\n    " __lines)
  list(APPEND ${lines_output_var} "    ${__lines}")
  list(APPEND ${lines_output_var} ")")
  unset(__lines)
  set(${lines_output_var} "${${lines_output_var}}" PARENT_SCOPE)
endfunction()

if (UNIX AND (NOT APPLE))
  set(WITH_QT "5" CACHE STRING "Build with Qt Backend support")
endif()

if (WITH_LUA_ROCKS)
  set(OPENCV_INCLUDE_INSTALL_PATH "lib/opencv_lua/devel/include" CACHE STRING "Include directory destination")
else()
  set(OPENCV_INCLUDE_INSTALL_PATH "${CMAKE_INSTALL_INCLUDEDIR}" CACHE STRING "Include directory destination")
endif()

if ((NOT DEFINED OpenCV_DIR) AND (NOT DEFINED OpenCV_LIBS))
  # Compute the subdirectory prefix relative to this file.
  get_filename_component(_IMPORT_PREFIX "${CMAKE_CURRENT_LIST_FILE}" DIRECTORY)
  add_subdirectory("${_IMPORT_PREFIX}")

  list(REMOVE_DUPLICATES OpenCV_LIBS)
  set(OpenCV_LINK_LIBRARIES)
  unset(OpenCV_DEPENDENCIES)
  unset(OpenCV_SOURCE_OBJECTS)

  # We want the opencv lua module to be used as a replacement of opencv
  # when another lua module depends on opencv.
  # Linux lua expects shared libraries as modules (.so)
  # Windows lua exepects a .dll file, which can be a static or a shared library.
  # For the opencv lua module to be used as a replcement of opencv,
  # it neeeds to have all the exported opencv functions.
  # One way to achieve that purpose is to build opencv as a static library.
  # However, on Windows, that will force every dependant module to also be a static library,
  # Which will lead to a repetitions of opencv functions beteween the lua opencv module and it's dependants.
  # Which will also lead to an increase of the size of the dependant modules.
  # The solution choosen is to directly embed the opencv objects in the opencv lua module.
  # That will allow the opencv lua module shared library to have all the exported opencv functions.
  # Moreover, it works on Linux and Windows
  if (BUILD_SHARED_LIBS)
    set(OpenCV_DEPENDENCIES "${OpenCV_LIBS}")

    foreach(the_module IN LISTS OpenCV_LIBS)
      get_target_property(__cmake_binary_dir "${the_module}" BINARY_DIR)
      get_target_property(__cmake_source_dir "${the_module}" SOURCE_DIR)
      get_target_property(__cmake_sources "${the_module}" SOURCES)

      # Keep object files already in the sources
      set(__cmake_imported_objects ${__cmake_sources})
      list(FILTER __cmake_imported_objects INCLUDE REGEX "(^\\\$<TARGET_OBJECTS:|\\${CMAKE_C_OUTPUT_EXTENSION}$)")
      list(APPEND OpenCV_SOURCE_OBJECTS ${__cmake_imported_objects})
      unset(__cmake_imported_objects)

      unset(c_objects_unique)
      unset(c_objects_duplicated)
      unset(sources_unique)
      unset(filenames_duplicated)
      unset(filenames_unique)

      # Get c files objects
      get_c_sources(__cmake_c_compiled_objects __cmake_sources)
      transform_to_object_file(
        c_objects_unique
        "${the_module}"
        __cmake_c_compiled_objects
        "${__cmake_source_dir}"
        "${__cmake_binary_dir}"

        c_objects_duplicated
        sources_unique
        filenames_duplicated
        filenames_unique
      )

      # Mark the c object files as GENERATED because they will not exists until opencv is built
      set(c_objects "${c_objects_unique}" "${c_objects_duplicated}")
      set_source_files_properties("${c_objects}" PROPERTIES GENERATED TRUE)

      # Add the c object files to source files
      list(APPEND OpenCV_SOURCE_OBJECTS "${c_objects}")

      unset(c_objects_unique)
      unset(c_objects_duplicated)
      unset(sources_unique)
      unset(filenames_duplicated)
      unset(filenames_unique)

      # Linked libraries should be preserved in order to link source objects
      list(APPEND OpenCV_LINK_LIBRARIES "$<TARGET_PROPERTY:${the_module},LINK_LIBRARIES>")

      # Qt mocs should be added to source objects in order to define Qt symbols
      get_target_property(__cmake_automoc ${the_module} AUTOMOC)
      if (__cmake_automoc)
        if (CMAKE_GENERATOR MATCHES "Visual Studio")
          set(__mocs_compilation "${__cmake_binary_dir}/${the_module}.dir/${CMAKE_BUILD_TYPE}/${the_module}_autogen/mocs_compilation.cpp${CMAKE_C_OUTPUT_EXTENSION}")
        else()
          set(__mocs_compilation "${__cmake_binary_dir}/CMakeFiles/${the_module}.dir/${the_module}_autogen/mocs_compilation.cpp${CMAKE_C_OUTPUT_EXTENSION}")
        endif()
        set_source_files_properties("${__mocs_compilation}" PROPERTIES GENERATED TRUE)
        list(APPEND OpenCV_SOURCE_OBJECTS "${__mocs_compilation}")
      endif()
      unset(__cmake_automoc)

      unset(__cmake_binary_dir)
      unset(__cmake_source_dir)
      unset(__cmake_sources)
    endforeach()

    list(REMOVE_DUPLICATES OpenCV_SOURCE_OBJECTS)

    unset(OpenCV_LINKED_TARGETS)
    foreach(the_module IN LISTS OpenCV_LIBS)
      get_target_property(__link_libraries "${the_module}" LINK_LIBRARIES)
      if("${__link_libraries}" STREQUAL "__link_libraries-NOTFOUND")
        get_target_property(__link_libraries "${the_module}" INTERFACE_LINK_LIBRARIES)
      endif()

      if(NOT "${__link_libraries}" STREQUAL "__link_libraries-NOTFOUND")
        list(APPEND OpenCV_LINKED_TARGETS "${__link_libraries}")
      endif()
    endforeach()

    # Exclude OpenCV_LIBS from LINK_LIBRARIES
    list(JOIN OpenCV_LIBS "|" __link_libraries_excluded)
    list(FILTER OpenCV_LINKED_TARGETS EXCLUDE REGEX "^(${__link_libraries_excluded})$")
    unset(__link_libraries_excluded)
    list(REMOVE_DUPLICATES OpenCV_LINKED_TARGETS)

    unset(__imported_libraries)
    unset(__namespace)
    set(__has_imported_targets FALSE)
    set(__has_linked_targets FALSE)

    foreach(target_name IN LISTS OpenCV_LINKED_TARGETS)
      if (TARGET "${target_name}")
        if (target_name STREQUAL "Threads::Threads")
          list(APPEND __imported_libraries " ") # empty line
          list(APPEND __imported_libraries "find_package(${target_name} REQUIRED)")
          continue()
        endif()

        set(__is_alias FALSE)
        unset(__aliased_libraries)
        set(__alias_target "${target_name}")
        get_target_property(__aliased_target "${__alias_target}" ALIASED_TARGET)
        while(__aliased_target)
          set(__is_alias TRUE)
          list(APPEND __aliased_libraries "add_library(${__alias_target} ALIAS ${__aliased_target})")
          set(__alias_target "${__aliased_target}")
          get_target_property(__aliased_target "${__alias_target}" ALIASED_TARGET)
          set(target_name "${__aliased_target}")
        endwhile()
        unset(__aliased_target)
        unset(__alias_target)

        get_target_property(__lib_imported "${target_name}" IMPORTED)
        if (__lib_imported)
          set(__has_imported_targets TRUE)
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

          list(APPEND __imported_libraries " ") # empty line
          list(APPEND __imported_libraries "# Create imported target ${__namespace}${target_name}")
          list(APPEND __imported_libraries "add_library(${__library_opts})")

          set(__has_imported_properties FALSE)
          unset(__imported_properties)

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
              if (NOT __has_imported_properties)
                set(__has_imported_properties TRUE)
                list(APPEND __imported_libraries "set_target_properties(${__namespace}${target_name} PROPERTIES")
              endif()
              string(REPLACE "\"" "\\\"" __variable "${__variable}")
              string(REPLACE ";" "$<SEMICOLON>" __variable "${__variable}")
              list(APPEND __imported_libraries "  ${__property} \"${__variable}\"")
            endif()
            unset(__variable)
          endforeach()

          unset(__has_imported_properties)

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
          set(__has_linked_targets TRUE)
          install(TARGETS "${target_name}"
            EXPORT OpenCVLinkedTargets
            LIBRARY DESTINATION objects/lib
            ARCHIVE DESTINATION objects/lib
          )
        endif()
        unset(__lib_imported)

        if (__is_alias)
          list(REVERSE __aliased_libraries)
          list(APPEND __imported_libraries "${__aliased_libraries}")
        endif()
        unset(__is_alias)
      endif()
    endforeach()

    if (__has_imported_targets)
      list(JOIN __imported_libraries "\n" __imported_libraries)
      string(REPLACE "$<SEMICOLON>" ";" __imported_libraries "${__imported_libraries}")
      file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/OpenCVImportedTargets.cmake" "${__imported_libraries}\n")
      install(FILES "${CMAKE_CURRENT_BINARY_DIR}/OpenCVImportedTargets.cmake" DESTINATION objects)
    endif()

    unset(__namespace)
    unset(__imported_libraries)

    unset(__opencv_objects_targets)

    if (__has_imported_targets)
      list(APPEND __opencv_objects_targets "include(\"\${CMAKE_CURRENT_LIST_DIR}/OpenCVImportedTargets.cmake\")")
    endif()

    # Install link libraries that are targets
    if (__has_linked_targets)
      list(APPEND __opencv_objects_targets "include(\"\${CMAKE_CURRENT_LIST_DIR}/OpenCVLinkedTargets.cmake\")")
      install(EXPORT OpenCVLinkedTargets
        FILE OpenCVLinkedTargets.cmake
        DESTINATION objects
      )
    endif()
    unset(__has_linked_targets)

    set(OpenCV_LIBS ${OpenCV_LINK_LIBRARIES})
    unset(OpenCV_LINK_LIBRARIES)

    save_variable(__opencv_objects_targets OpenCV_DEPENDENCIES)
    save_variable(__opencv_objects_targets OpenCV_LIBS)
    save_variable(__opencv_objects_targets OpenCV_SOURCE_OBJECTS)

    list(JOIN __opencv_objects_targets "\n" __opencv_objects_targets)
    file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/OpenCVObjects.cmake" "${__opencv_objects_targets}\n")
    install(FILES "${CMAKE_CURRENT_BINARY_DIR}/OpenCVObjects.cmake" DESTINATION objects)
    unset(__opencv_objects_targets)
  else()
    unset(OpenCV_SOURCE_OBJECTS)
    unset(OpenCV_LIBS)
    unset(OpenCV_WHOLE_ARCHIVE)

    foreach(target_name IN LISTS OpenCV_LINK_LIBRARIES)
      if (TARGET "${target_name}")
        list(APPEND OpenCV_WHOLE_ARCHIVE "${target_name}")
      else()
        list(APPEND OpenCV_LIBS "${target_name}")
      endif()
    endforeach()

    # https://gitlab.kitware.com/cmake/cmake/-/issues/24504#note_1321969
    string(REPLACE ";" "," OpenCV_WHOLE_ARCHIVE "${OpenCV_WHOLE_ARCHIVE}")
    list(APPEND "$<LINK_GROUP:RESCAN,$<LINK_LIBRARY:WHOLE_ARCHIVE,${OpenCV_WHOLE_ARCHIVE}>>")
    unset(OpenCV_WHOLE_ARCHIVE)

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

  unset(OpenCV_SOURCE_OBJECTS)
  if (BUILD_SHARED_LIBS AND EXISTS "${OpenCV_DIR}/objects/OpenCVObjects.cmake")
    include("${OpenCV_DIR}/objects/OpenCVObjects.cmake")

  # When built as a static library,
  # only opencv_world library is created
  # when opencv_world is part of the modules
  elseif (NOT OpenCV_SHARED AND ";${OpenCV_LIBS};" MATCHES ";opencv_world;")
    set(_OpenCV_LIBS opencv_world)

    set(opencv_sfm_IS_NOT_PART_OF_WORLD TRUE)
    set(opencv_img_hash_IS_NOT_PART_OF_WORLD TRUE)

    foreach(the_module IN LISTS OpenCV_LIBS)
      if(${the_module}_IS_NOT_PART_OF_WORLD)
        list(APPEND _OpenCV_LIBS ${the_module})
      endif()
    endforeach()
    set(OpenCV_LIBS ${_OpenCV_LIBS})
    unset(_OpenCV_LIBS)
  endif()

  set(OpenCV_LINK_LIBRARIES ${OpenCV_LIBS})
  foreach(target_name IN LISTS OpenCV_LINK_LIBRARIES)
    if (TARGET "${target_name}")
      list(APPEND OpenCV_LIBS "$<LINK_LIBRARY:WHOLE_ARCHIVE,${target_name}>")
    else()
      list(APPEND OpenCV_LIBS "${target_name}")
    endif()
  endforeach()
  unset(OpenCV_LINK_LIBRARIES)
endif()

if (DEFINED OpenCV_SOURCE_OBJECTS)
  source_group("OpenCV Source Objects" FILES ${OpenCV_SOURCE_OBJECTS})
endif()

# message(STATUS "OpenCV library status:")
# message(STATUS "    config: ${OpenCV_DIR}")
# message(STATUS "    version: ${OpenCV_VERSION}")
# list_print(OpenCV_LIBS "    libraries: " "\n                    ")
# list_print(OpenCV_INCLUDE_DIRS "    include path: " "\n                       ")

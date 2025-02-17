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
  # We want the opencv lua module to be used as a replacement of opencv
  # when another lua module depends on opencv.
  # 
  # Linux lua expects shared libraries as modules (.so)
  # Windows lua exepects a .dll file, which can be a static or a shared library.
  # 
  # For the opencv lua module to be used as a replcement of opencv,
  # it needs to have all the exported opencv functions also exported.
  # 
  # One way to achieve that purpose is to build opencv as a static library
  # and then use target_link_libraries(shared_lib PRIVATE "$<LINK_LIBRARY:WHOLE_ARCHIVE,static_lib1;static_lib2>")
  # @see: https://cmake.org/cmake/help/latest/manual/cmake-generator-expressions.7.html#genex:LINK_LIBRARY
  # However, on Windows, set(OPENCV_MODULE_TYPE "STATIC") will cause all dependent libraries to be also static.
  # We do not want that because in other for opencv_lua to be linked against, it must be a shared library
  # 
  # The solution choosen is to directly embed the opencv objects in the opencv lua module.
  # That will allow the opencv lua module shared library to have all the exported opencv functions.
  set(OPENCV_MODULE_TYPE "OBJECT")

  # Compute the subdirectory prefix relative to this file.
  get_filename_component(_IMPORT_PREFIX "${CMAKE_CURRENT_LIST_FILE}" DIRECTORY)
  add_subdirectory("${_IMPORT_PREFIX}")

  list(REMOVE_DUPLICATES OpenCV_LIBS)

  # Keep object files already in the sources because they are not propagated
  unset(OpenCV_SOURCE_OBJECTS)
  foreach(the_module IN LISTS OpenCV_LIBS)
    get_target_property(__cmake_sources "${the_module}" SOURCES)
    list(FILTER __cmake_sources INCLUDE REGEX "(^\\\$<TARGET_OBJECTS:|\\${CMAKE_C_OUTPUT_EXTENSION}$)")
    list(APPEND OpenCV_SOURCE_OBJECTS "${__cmake_sources}")
    unset(__cmake_sources)
  endforeach()
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

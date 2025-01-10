function(list_set_filename_component Component PathListVar)
  set(ARGS)
  if (ARGC GREATER 2)
    list(SUBLIST ARGV 2 -1 ARGS)
  endif()

  set(index 0)
  foreach(PathVar IN LISTS ${PathListVar})
    get_filename_component(PathVar "${PathVar}" ${Component} ${ARGS})
    list(REMOVE_AT ${PathListVar} ${index})
    list(INSERT ${PathListVar} ${index} "${PathVar}")
    math(EXPR index "${index} + 1")
  endforeach()

  set(${PathListVar} ${${PathListVar}} PARENT_SCOPE)
endfunction()

function(list_cmake_convert SubCommand PathListVar)
  set(ARGS)
  if (ARGC GREATER 2)
    list(SUBLIST ARGV 2 -1 ARGS)
  endif()

  set(index 0)
  foreach(PathVar IN LISTS ${PathListVar})
    file(${SubCommand} "${PathVar}" PathVar ${ARGS})
    list(REMOVE_AT ${PathListVar} ${index})
    list(INSERT ${PathListVar} ${index} "${PathVar}")
    math(EXPR index "${index} + 1")
  endforeach()

  set(${PathListVar} ${${PathListVar}} PARENT_SCOPE)
endfunction()

function(list_cmake_relative_path PathListVar Directory)
  set(index 0)
  foreach(PathVar IN LISTS ${PathListVar})
    file(RELATIVE_PATH PathVar "${Directory}" "${PathVar}")
    list(REMOVE_AT ${PathListVar} ${index})
    list(INSERT ${PathListVar} ${index} "${PathVar}")
    math(EXPR index "${index} + 1")
  endforeach()

  set(${PathListVar} ${${PathListVar}} PARENT_SCOPE)
endfunction()

function(list_cmake_path SubCommand PathListVar)
  set(ARGS)
  if (ARGC GREATER 2)
    list(SUBLIST ARGV 2 -1 ARGS)
  endif()

  set(index 0)
  foreach(PathVar IN LISTS ${PathListVar})
    cmake_path(${SubCommand} PathVar ${ARGS} PathVar)
    list(REMOVE_AT ${PathListVar} ${index})
    list(INSERT ${PathListVar} ${index} "${PathVar}")
    math(EXPR index "${index} + 1")
  endforeach()

  set(${PathListVar} ${${PathListVar}} PARENT_SCOPE)
endfunction()

# To get the longest common path:
# path1 = /a/b/d/e
# path2 = /a/b/g
# RELATIVE_PATH path1 path2 = ../../g
# The common path is /a/b/d/e/../../ = /a/b
function(cmake_path_longest_common output path1 path2)
  # __longest_common_path = ../../
  file(RELATIVE_PATH __longest_common_path "${ARGV1}" "${ARGV2}")

  # __longest_common_path = ../../
  string(REGEX MATCH "^([./]+)" __longest_common_path "${__longest_common_path}")

  # __longest_common_path = NORMALIZE /a/b/d/e/../../ = /a/b
  cmake_path(SET __longest_common_path NORMALIZE "${ARGV1}/${__longest_common_path}")

  math(EXPR ARGS_REMAING "${ARGC} - 3")
  if (ARGS_REMAING GREATER 0)
    list(SUBLIST ARGV 3 ${ARGS_REMAING} ARGS)
    cmake_path_longest_common(__longest_common_path "${__longest_common_path}" ${ARGS})
  endif()

  # Remove trailling slash
  string(REGEX REPLACE "/$" "" __longest_common_path "${__longest_common_path}")

  set(${output} "${__longest_common_path}" PARENT_SCOPE)
endfunction()

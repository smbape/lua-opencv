cmake_minimum_required(VERSION 3.19)

project(test_sol)

add_executable(test_sol test.cpp)
target_link_libraries(test_sol PRIVATE sol2::sol2 ${LUA_LIBRARIES})
if (DEFINED LUA_INCLUDE_DIR)
  target_include_directories(test_sol PRIVATE ${LUA_INCLUDE_DIR})
endif()

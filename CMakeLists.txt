cmake_minimum_required(VERSION 3.10)

project(condition VERSION 0.1 LANGUAGES CXX)

set(CMAKE_BUILD_TYPE Debug)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)


if ( CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX )
    set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror")
endif()
if ( MSVC )
    set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} /W4")
endif()


add_executable(condition
  ${CMAKE_CURRENT_SOURCE_DIR}/src/condition.cpp
  )

add_executable(shared
  ${CMAKE_CURRENT_SOURCE_DIR}/src/shared.cpp
  )

add_executable(shared_timed
  ${CMAKE_CURRENT_SOURCE_DIR}/src/shared_timed.cpp
  )

add_executable(posix
  ${CMAKE_CURRENT_SOURCE_DIR}/src/posix.cpp
  )

#include_directories(
#  "${CMAKE_CURRENT_SOURCE_DIR}/src"
#)

target_link_libraries(condition pthread)
target_link_libraries(shared pthread)
target_link_libraries(shared_timed pthread)
target_link_libraries(posix pthread)

cmake_minimum_required (VERSION 2.8.1)

project (luabind)

add_definitions("-std=gnu99 -g3")


include_directories(
	${CMAKE_SOURCE_DIR}/lua
)

link_directories(
	${CMAKE_SOURCE_DIR}/lua
)

add_executable(luabind luabind.c)
target_link_libraries(luabind m lua pthread dl)
set(EXECUTABLE_OUTPUT_PATH "${CMAKE_CURRENT_LIST_DIR}/../../bin/Debug")

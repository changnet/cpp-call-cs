cmake_minimum_required (VERSION 2.8.1)

project (luabind)

add_definitions("-std=gnu99 -g3 -D__STDC_NO_ATOMICS__")


include_directories(
	${CMAKE_SOURCE_DIR}/lua
	/home/xzc/skynet/3rd/jemalloc/include/jemalloc
)

link_directories(
	${CMAKE_SOURCE_DIR}/lua
	/home/xzc/skynet/3rd/jemalloc/lib
)

add_executable(luabind malloc_hook.c luabind.c)
target_link_libraries(luabind jemalloc_pic.a m lua pthread dl)
set(EXECUTABLE_OUTPUT_PATH "${CMAKE_CURRENT_LIST_DIR}/../../bin/Debug")

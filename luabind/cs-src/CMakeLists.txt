cmake_minimum_required (VERSION 2.8.1)

project (cs)

add_definitions("-std=c++11 -g3 -fPIC")


include_directories(
	${CMAKE_SOURCE_DIR}/lua
	/usr/share/dotnet/packs/Microsoft.NETCore.App.Host.linux-x64/6.0.32/runtimes/linux-x64/native/
)

link_directories(
	${CMAKE_SOURCE_DIR}/lua
	/usr/share/dotnet/packs/Microsoft.NETCore.App.Host.linux-x64/6.0.32/runtimes/linux-x64/native/
)

add_library(cs SHARED cs.cpp)
target_link_libraries(cs nethost pthread dl)
set(LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/../bin/Debug")

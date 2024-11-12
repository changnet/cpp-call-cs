cmake_minimum_required (VERSION 2.8.1)

project (je_nativehost)

set(CMAKE_CXX_STANDARD 11)
add_definitions("-std=c++11 -g3 -D__STDC_NO_ATOMICS__")


include_directories(
	/usr/share/dotnet/packs/Microsoft.NETCore.App.Host.linux-x64/6.0.32/runtimes/linux-x64/native/
	/usr/share/dotnet/packs/Microsoft.NETCore.App.Host.linux-x64/8.0.10/runtimes/linux-x64/native/
	/home/xzc/jemalloc-5.3.0/include/jemalloc
)

link_directories(
	/usr/share/dotnet/packs/Microsoft.NETCore.App.Host.linux-x64/6.0.32/runtimes/linux-x64/native/
	/usr/share/dotnet/packs/Microsoft.NETCore.App.Host.linux-x64/8.0.10/runtimes/linux-x64/native/
	/home/xzc/jemalloc-5.3.0/lib
)

add_executable(je_nativehost src/malloc_hook.c src/je_nativehost.cpp)
target_link_libraries(je_nativehost jemalloc_pic.a nethost pthread dl)
set(EXECUTABLE_OUTPUT_PATH "${CMAKE_CURRENT_LIST_DIR}/../bin/Debug")

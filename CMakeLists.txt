cmake_minimum_required (VERSION 2.8.1)

project (nativehost)

set(CMAKE_CXX_STANDARD 11)
add_definitions("-std=c++11 -g3")


include_directories(
	/usr/share/dotnet/packs/Microsoft.NETCore.App.Host.linux-x64/6.0.32/runtimes/linux-x64/native/
)

file(GLOB_RECURSE SRC_LIST "src/*.cpp")

link_directories(
	/usr/share/dotnet/packs/Microsoft.NETCore.App.Host.linux-x64/6.0.32/runtimes/linux-x64/native/
)

add_executable(nativehost ${SRC_LIST})
target_link_libraries(nativehost nethost pthread dl)

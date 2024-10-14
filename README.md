# cpp-call-cs
A demo to test c++ call C#

# Usage
1. Visual Studio Open project `CSharpPlugin.sln` and build the project.
2. build C++ nativehost
```
cd nativehost
mkdir -p build
cd build
cmake ..
make
```
3. run nativehost
```
cd ../../bin/Debug
./nativehost
```

# Kill
```
ps -ef | grep 'nativehost' | grep -v grep | awk '{print $2}' | xargs -r kill -15
```

# build luabind
```
wget https://www.lua.org/ftp/lua-5.4.7.tar.gz
cd lua-5.4.7
make linux
make install
ldconfig -v
```

cd src && mkdir -p /usr/local/bin /usr/local/include /usr/local/lib /usr/local/man/man1 /usr/local/share/lua/5.4 /usr/local/lib/lua/5.4
cd src && install -p -m 0755 lua luac /usr/local/bin
cd src && install -p -m 0644 lua.h luaconf.h lualib.h lauxlib.h lua.hpp /usr/local/include
cd src && install -p -m 0644 liblua.a /usr/local/lib
cd doc && install -p -m 0644 lua.1 luac.1 /usr/local/man/man1
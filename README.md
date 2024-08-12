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
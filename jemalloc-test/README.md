# Usage
1. install jemalloc with prefix=je_
```
wget -O jemalloc-5.3.0.tar.gz https://github.com/jemalloc/jemalloc/archive/refs/tags/5.3.0.tar.gz
tar -zxvf jemalloc-5.3.0.tar.gz
cd jemalloc-5.3.0
./autogen.sh --with-jemalloc-prefix=je_ --enable-prof
make
make install
```

2. build je_nativehost
```
cd jemalloc-test
mkdir -p build
cd build
cmake ..
make
```

3. run test
```
cd ../../bin/Debug
./je_nativehost
```


# micromath

[![CircleCI](https://circleci.com/gh/dacunni/micromath.svg?style=svg&circle-token=cf3a0be8b051eb44c03081cc0737947324858e92)](https://circleci.com/gh/dacunni/micromath)

## Setup and Build
Fetch third party dependencies
```
git submodule init
git submodule update --remote
```

Setup the build
```
mkdir build
cd build
cmake -DBUILD_PYTHON_BINDINGS:BOOL=OFF ..
```

Build and unit test the code
```
# In the 'build' directory
make -j
# Run unit tests
ctest
```
### Building With Another Version of GCC

If you have installed a newer version of GCC, export the CC and CXX
environment variables prior to running cmake so it picks up the new ones.
```
export CC=/usr/local/bin/gcc
export CXX=/usr/local/bin/g++
# cmake ...
```


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

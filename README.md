# fluxrt

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
cmake ..
```

Build and unit test the code
```
# In the 'build' directory
make -j install
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

## Tracing a Scene

If a scene description uses relative paths for resource specification,
you will need to set environment variables pointing to the location
of those resources. Absolute paths beginning with '/' will be respected,
even if these environment variables are set.

```
export MESH_PATH=/Volumes/Extra/data/3dmodels
export ENV_MAP_PATH=/Volumes/Extra/data/envmaps
export SCENE_PATH=../../scenes/toml
cd app
./trace_scene -s 10 -d 5 -t 3 ../../scenes/toml/mitsuba-sphere-envmap.toml
```


# Building on a Raspberry Pi

## Install Compilers Etc
```
sudo apt install -y git cmake make python3 gcc g++ clang swig
```

## Installing Google Test / Benchmark Suites
```
mkdir tps
cd tps
```

### Build / Install Google Test
```
git clone -q https://github.com/google/googletest.git googletest
mkdir -p googletest/build
pushd googletest/build
cmake .. && make -j 2
sudo make install
popd
```

### Build / Install Google Benchmark
```
git clone -q https://github.com/google/benchmark.git googlebenchmark
mkdir -p googlebenchmark/build
pushd googlebenchmark/build
git checkout `git rev-list -n 1 --before="2019-04-01 00:00" master`
cmake .. && make -j 2
sudo make install
popd
```

## Checkout
```
git clone git@github.com:dacunni/micromath.git
# optional: git checkout raspberrypi
git submodule init
git submodule update --remote
```

## Build / Unit Test
```
mkdir build
cd build
cmake ..
make -j 2
ctest
```





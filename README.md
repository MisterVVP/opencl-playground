# opencl-playground
OpenCL playground to try out GPU-based code execution via [OpenCL](https://github.com/KhronosGroup/OpenCL-Guide).
I'm using `AMD Ryzen 9 5900X CPU` and `AMD Radeon RX 6900 XT GPU`, so for now everything in this repository is tested with this AMD hardware stack.
However, same concepts can be applied to NVIDIA and Intel hardware as well.

## Sub folders

### hello-world
Simple hello world app example taken from https://github.com/KhronosGroup/OpenCL-Guide/blob/main/chapters/getting_started_linux.md#invoking-the-compiler-manually

Quickstart:
```
APP_NAME=hello-world
docker build . -t $APP_NAME:latest --build-arg APP_NAME=$APP_NAME
docker run --device=/dev/dri --entrypoint /app/$APP_NAME $APP_NAME:latest
```

### simplified-rbi-system

A couple of microservices emulating the system which:
- collects metrics from devices
- periodically calculates risks and generates reports
- exposes simple api to query reports

#### Requirements
- Ubuntu 24.04
- C++ 20
- AMD ROCm, OpenCL
- Docker

#### Quickstart
##### Run metrics-api, sensors (emulator), postgreSQL and pgAdmin via docker compose
```
cd simplified-rbi-system
docker compose build
docker compose up
```
##### Build `rbi` application
Make sure to install all dependencies, e.g. for Ubuntu
```
apt update && apt upgrade && apt install -y cmake git ninja-build opencl-headers opencl-clhpp-headers ocl-icd-opencl-dev libpq-dev build-essential
```

Build app in new shell (includes installation of hiredis and msgpack)
```
cd simplified-rbi-system/rbi
cmake . -G Ninja -DCMAKE_BUILD_TYPE=Release -B ./build \
    && cd ./build/_deps/hiredis-src && sudo make install \
    && cd ../msgpack-src && cmake -DMSGPACK_CXX20=ON . \
    && sudo cmake --build . --target install \
    && cd ../../../build \
    && ninja
```

Run it from host machine
```
export POSTGRES_CONNECTION_STRING=postgresql://postgres:postgres@localhost:5432/metrics \
export USE_DOCKERIZED_POSTGRES=1 \
export REDIS_PORT=6379 \
export REDIS_HOST=localhost \
&& ./src/rbi
```

## TODO
- Smaller docker images. Debian?
- Find if it's possible to run simplified-rbi-system/rbi inside Docker as well (now it runs on host due to GPU dependency)
- Check how to use package managers for C++ (e.g. https://conan.io/center/recipes/msgpack-cxx)
- More coding samples
- Compare calculation performance between GPU and CPU for various tasks
- Write something to execute OpenCL on AMD Ryzen CPU. This may require POCL instead https://portablecl.org/
- Check python-based openCL as well as possibilities to utilise Golang for similar tasks
- Get Nvidia card and Intel CPU to try other techs in new github repo

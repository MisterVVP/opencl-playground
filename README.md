# opencl-playground
OpenCL dev playground


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

Quickstart:
```
cd simplified-rbi-system
docker compose build
docker compose up
```

Install dependencies, e.g. for Ubuntu
```
apt update && apt upgrade && apt install -y cmake git ninja-build opencl-headers opencl-clhpp-headers ocl-icd-opencl-dev libpq-dev build-essential
```

Build app in new shell
```
cd simplified-rbi-system/rbi
cmake . -G Ninja -DCMAKE_BUILD_TYPE=Release -B ./build \
    && cd ./build \
    && ninja
```

Run it from host machine
```
export POSTGRES_CONNECTION_STRING=postgresql://postgres:postgres@localhost:5432/metrics \
export USE_DOCKERIZED_POSTGRES=1 \
&& ./src/rbi
```

## TODO
- Smaller docker images. Debian?
- Find if it's possible to run simplified-rbi-system/rbi inside Docker as well (now it runs on host due to GPU dependency)
- More coding samples
- Compare calculation performance between GPU and CPU for various tasks
- Write something to execute OpenCL on AMD Ryzen CPU. This may require POCL instead https://portablecl.org/
- Check python-based openCL as well as possibilities to utilise Golang for similar tasks
- Get Nvidia card and Intel CPU to try other techs in new github repo
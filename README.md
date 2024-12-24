# opencl-playground
OpenCL dev playground


## Quickstart

APP_NAME=hello-world
docker build . -t $APP_NAME:latest --build-arg APP_NAME=$APP_NAME
docker run --entrypoint /app/$APP_NAME $APP_NAME:latest


## TODO
- Smaller docker images. Debian?
- Try more complex coding example
- Try to re-use similar dockerimage for llvm playground
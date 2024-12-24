FROM ubuntu:24.04 AS build

RUN apt update && apt upgrade && apt install -y cmake git ninja-build opencl-headers ocl-icd-opencl-dev

ARG APP_NAME

WORKDIR /src
COPY $APP_NAME .

RUN cmake . -G Ninja -DCMAKE_BUILD_TYPE=Release -B ./build \
    && cd ./build \
    && ninja 

FROM ubuntu:24.04

RUN apt update && apt install -y wget \
    && wget https://repo.radeon.com/amdgpu-install/6.3.1/ubuntu/noble/amdgpu-install_6.3.60301-1_all.deb \
    && apt install -y ./amdgpu-install_6.3.60301-1_all.deb \
    && apt update \
    && rm ./amdgpu-install_6.3.60301-1_all.deb \
    && amdgpu-install -y --usecase=opencl \
    && apt clean \
    && apt autoremove --purge 

WORKDIR /app
ARG APP_NAME
ARG APP_ID=10001
ENV APP_NAME=$APP_NAME
COPY --from=build /src/build/$APP_NAME /app

RUN groupadd --gid $APP_ID notroot \
    && useradd --uid $APP_ID --gid notroot --no-create-home --home-dir /app --shell /bin/false $APP_NAME \
    && usermod -a -G video $APP_NAME

USER $APP_ID
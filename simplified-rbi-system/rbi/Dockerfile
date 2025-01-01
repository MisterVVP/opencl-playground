FROM ubuntu:24.04 AS build

RUN apt update && apt upgrade && apt install -y cmake git ninja-build opencl-headers opencl-clhpp-headers ocl-icd-opencl-dev libpq-dev build-essential libboost-all-dev

WORKDIR /tmp
COPY . .

RUN cmake . -G Ninja -DCMAKE_BUILD_TYPE=Release -B ./build
# Install hiredis
RUN cd ./build/_deps/hiredis-src && make install
# Install msgpack
RUN cd ./build/_deps/msgpack-src && cmake -DMSGPACK_CXX20=ON . && cmake --build . --target install
# Build application and cleanup installation files
RUN cd ./build && ninja && rm -rf /tmp/build/_deps 

FROM ubuntu:24.04

WORKDIR /app
COPY --from=build /tmp/build/src/rbi /app
COPY --from=build /usr/local/include/msgpack /usr/local/include/msgpack
COPY --from=build /usr/local/include/hiredis /usr/local/include/hiredis

RUN apt update \
    && apt install -y "linux-headers-$(uname -r)" "linux-modules-extra-$(uname -r)" \
    && apt install -y wget build-essential libpq-dev opencl-headers opencl-clhpp-headers ocl-icd-opencl-dev python3-setuptools python3-wheel libpython3.12 libboost-all-dev \
    && wget https://repo.radeon.com/amdgpu-install/6.3.1/ubuntu/noble/amdgpu-install_6.3.60301-1_all.deb \
    && apt install -y ./amdgpu-install_6.3.60301-1_all.deb \
    && apt update \
    && rm ./amdgpu-install_6.3.60301-1_all.deb \
    && amdgpu-install -y --usecase=rocm \
    && apt clean \
    && apt autoremove --purge 

COPY rocm.conf /etc/ld.so.conf.d/rocm.conf

RUN chmod 644 /etc/ld.so.conf.d/rocm.conf && ldconfig && groupadd --gid 10001 notroot \
    && useradd --uid 10001 --gid notroot --no-create-home --home-dir /app --shell /bin/false rbi \
    && usermod -a -G video,render rbi

USER 10001

ENTRYPOINT ["/app/rbi"]
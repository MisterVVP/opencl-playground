FROM alpine:latest AS build
RUN apk update && apk upgrade && apk add cmake build-base git postgresql-dev

WORKDIR /app
COPY . .

RUN mkdir build && cd build && cmake .. -G"Unix Makefiles" -DCMAKE_BUILD_TYPE=Release
RUN cd /app/build && cmake --build .


FROM alpine:latest
RUN apk update && apk upgrade && apk add libstdc++ postgresql-dev

COPY --from=build /app/build/src/sensors /app/sensors

ENTRYPOINT ["/app/sensors"]
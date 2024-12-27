FROM alpine:latest AS build
RUN apk update && apk upgrade && apk add cmake build-base git zlib-dev postgresql-dev nlohmann-json

WORKDIR /app
COPY . .

RUN mkdir build && cd build && cmake .. -G"Unix Makefiles" -DCMAKE_BUILD_TYPE=Release
RUN cd /app/build/_deps/uwebsockets-src && make install && cd /app/build/_deps/uwebsockets-src/uSockets && make
RUN cd /app/build && cmake --build .


FROM alpine:latest
RUN apk update && apk upgrade && apk add libstdc++ postgresql-dev nlohmann-json

COPY --from=build /app/build/src/metrics-api /app/metrics-api
EXPOSE 9000
ENTRYPOINT ["/app/metrics-api"]
FROM alpine:latest AS build
RUN apk update && apk upgrade && apk add cmake build-base git clang19 ninja opencl-icd-loader-dev opencl-headers

ARG APP_NAME

WORKDIR /src
COPY $APP_NAME .

RUN cmake . -G Ninja -DCMAKE_BUILD_TYPE=Release -B ./build \
    && cd ./build \
    && ninja 


FROM alpine:latest
WORKDIR /app
RUN apk update && apk upgrade && apk add  opencl-icd-loader-dev opencl-headers
COPY --from=build /src/build /app

ARG APP_NAME
ARG APP_ID=10001

RUN addgroup --gid $APP_ID notroot \
    && adduser --uid $APP_ID --ingroup notroot --disabled-password --home /app --gecos '' $APP_NAME \
    && addgroup $APP_NAME video

USER $APP_ID

ENTRYPOINT ["./$APP_NAME"]
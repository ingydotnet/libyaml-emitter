FROM alpine:3.4

RUN apk update \
 && apk add \
        build-base \
 && true

RUN apk add \
        autoconf \
        automake \
        git \
        libc6-compat \
        libtool \
 && true

COPY $PWD/ /libyaml-emitter/

RUN \
(   cd /libyaml-emitter/  \
 && export LD_LIBRARY_PATH=$PWD/libyaml/src/.libs \
 && make clean build \
)

ENTRYPOINT ["/libyaml-emitter/libyaml-emitter"]

ENV LD_LIBRARY_PATH=/libyaml-emitter/libyaml/src/.libs

WORKDIR /cwd

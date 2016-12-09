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

COPY $PWD/ /libyaml-parser/

RUN \
(   cd /libyaml-parser/  \
 && export LD_LIBRARY_PATH=$PWD/libyaml/src/.libs \
 && make clean build \
)

ENTRYPOINT ["/libyaml-parser/libyaml-parser"]

ENV LD_LIBRARY_PATH=/libyaml-parser/libyaml/src/.libs

WORKDIR /cwd

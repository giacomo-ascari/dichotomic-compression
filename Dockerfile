FROM ubuntu:20.04
WORKDIR /dc-core
RUN apt-get update
RUN apt-get install -y build-essential
COPY bin/ ./bin/
COPY src/ ./src/
RUN gcc ./src/main.c \
    ./src/dc/dc.c \
    ./src/dc/stb/stb_vorbis.c \
    -lm \
    -o ./bin/dc_core -O
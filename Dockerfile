FROM gcc:latest
WORKDIR /dc-core
COPY bin/ ./bin/
COPY src/ ./src/
RUN gcc ./src/main.c \
    ./src/dc/dc.c \
    ./src/dc/stb/stb_vorbis.c \
    -lm \
    -o ./bin/dc_core -O

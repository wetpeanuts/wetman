clang \
    -std=c11 \
    -pedantic \
    -Wall -Wextra \
    -I ./src \
    -o build/wetman_server \
    src/wetman/server/main.c

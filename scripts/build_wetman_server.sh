clang \
    -DWETMAN_ENABLE_MEMORY_TRACKER \
    -std=c11 \
    -pedantic \
    -D_DEFAULT_SOURCE \
    -Wall -Wextra \
    -I ./src \
    -o build/wetman_server \
    src/wetman/server/main.c

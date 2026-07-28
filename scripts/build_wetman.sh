clang \
    -std=c11 \
    -pedantic \
    -Wall -Wextra \
    -I ./src \
    -o build/wetman \
    src/wetman/client/main.c

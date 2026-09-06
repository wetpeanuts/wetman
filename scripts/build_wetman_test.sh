clang \
    -std=c11 \
    -pedantic \
    -D_DEFAULT_SOURCE \
    -Wall -Wextra \
    -I ./src \
    -o build/wetman_test \
    test/main.c

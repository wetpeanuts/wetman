build:
    ./scripts/build_wetman.sh
    ./scripts/build_wetman_server.sh
    ./scripts/build_wetman_test.sh

run-server:
    ./build/wetman_server

run-client:
    ./build/wetman

test:
    ./build/wetman_test


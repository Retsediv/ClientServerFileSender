#include <iostream>

#include "server/server.h"

int main(int argc, char *argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: ./server <port> <destination>\n";
        return -1;
    }

    size_t NUM_THREADS = 4;
    server s{argv[1], argv[2], NUM_THREADS};

    s.run();

    return 0;
}
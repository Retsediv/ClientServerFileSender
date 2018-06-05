#include <iostream>

#include "server/server.h"
#include "helpers.h"

int main(int argc, char *argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: ./server <port> <destination>\n";
        return -1;
    }

    if (!file_exists(argv[2])) {
        std::cerr << "You path is incorrect! Such folder is not exists!" << std::endl;
        return EXIT_FAILURE;
    }

    if (!is_directory(argv[2])) {
        std::cerr << "You path is incorrect! It's not a directory" << std::endl;
        return EXIT_FAILURE;
    }


    size_t NUM_THREADS = 4;
    server s{argv[1], argv[2], NUM_THREADS};

    s.run();

    return 0;
}
#include "client/client.h"
#include "helpers.h"

int main(int argc, char *argv[]) {
    if (argc != 4) {
        std::cerr << "Usage: " << argv[0] << " ./client <ip> <port> <file path>" << std::endl;
        std::cerr << "sample: " << argv[0] << " ./client 127.0.0.1 1234 ./from/1.txt" << std::endl;
        return EXIT_FAILURE;
    }

    if (!file_exists(argv[3])) {
        std::cerr << "Failed to open file(check if it exists): " << argv[3] << std::endl;
        return EXIT_FAILURE;
    }

    if (!is_regular_file(argv[3])) {
        std::cout << "It's not a regular file: " << argv[3] << std::endl;
        return EXIT_FAILURE;
    }

    boost::asio::io_context io_context;
    client c(io_context, argv[1], argv[2], argv[3]);
    io_context.run();

    return 0;
}

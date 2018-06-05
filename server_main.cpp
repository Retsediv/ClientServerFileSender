#include <iostream>

#include "server.h"

int main(int argc, char *argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: ./server <port> <destination>\n";
        return -1;
    }

    size_t NUM_THREADS = 4;

//    boost::asio::io_context io_context;
//    boost::asio::io_context::work some_work(io_context);
//    boost::thread_group threads;
//
//    for (int i = 0; i < NUM_THREADS; ++i) {
//        threads.create_thread(boost::bind(&io_context::run, &io_context));
//    }

    server s{argv[1], argv[2], NUM_THREADS};

    s.run();
//    io_context.run();
//    threads.join_all();

    return 0;
}
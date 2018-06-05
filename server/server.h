#ifndef EZLOTESTTASK_SERVER_H
#define EZLOTESTTASK_SERVER_H

#include <ctime>
#include <iostream>
#include <boost/asio.hpp>
#include <boost/thread/thread.hpp>
#include <boost/array.hpp>
#include <boost/filesystem/path.hpp>
#include "boost/filesystem.hpp"

#include "io_context_pool.h"
#include "session.h"

class server {
public:
    server(const std::string &port, const std::string &dest, size_t io_context_pool_size);
    void run() { io_context_pool_.run(); }

private:
    void start_accept();
    void handle_accept(session *new_session, const boost::system::error_code &error);
    void handle_stop() { io_context_pool_.stop(); }

    io_context_pool io_context_pool_;
    boost::asio::ip::tcp::acceptor acceptor_;
    boost::filesystem::path destination_;
};


#endif
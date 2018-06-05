#ifndef EZLOTESTTASK_SESSION_H
#define EZLOTESTTASK_SESSION_H

#include <cstdlib>
#include <iostream>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <fstream>
#include <boost/filesystem.hpp>
#include <boost/filesystem/path.hpp>

class session {
public:
    session(boost::asio::io_context &io_context, const boost::filesystem::path &destination) : socket_(io_context),
                                                                                               destination_(destination) {}

    boost::asio::ip::tcp::socket &socket() { return socket_; }

    void proceed() { handle_read(); }

private:
    boost::asio::ip::tcp::socket socket_;

    boost::array<char, 1024> buf_{};
    boost::filesystem::path destination_;

    std::string file_path_;
    size_t file_size_ = 0;

    void handle_read();

    void send_response(const std::string &res);

    void get_file_properties();

};

#endif

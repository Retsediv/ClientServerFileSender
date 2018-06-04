#ifndef EZLOTESTTASK_CLIENT_H
#define EZLOTESTTASK_CLIENT_H

#include <iostream>
#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <boost/filesystem.hpp>
#include <boost/bind.hpp>

class client {
public:
    client(boost::asio::io_context &io_context, const std::string &ip, const std::string &port, const std::string &path)
            : resolver_(io_context),
              socket_(io_context),
              ip_(ip),
              port_(port),
              filepath(path) {
        std::ostream request_stream(&request_);

        resolver_.async_resolve(ip, "http", boost::bind(
                &client::handle_resolve, this,
                boost::asio::placeholders::error,
                boost::asio::placeholders::results)
        );
    }

private:
    boost::asio::ip::tcp::resolver resolver_;
    boost::asio::ip::tcp::socket socket_;
    boost::asio::streambuf request_;
    boost::asio::streambuf response_;
    std::string filepath;
    std::string ip_;
    std::string port_;

    void handle_resolve(const boost::system::error_code &err,
                        const boost::asio::ip::tcp::resolver::results_type &endpoints);

    void handle_connect(const boost::system::error_code &err);

    void handle_response();
};

#endif
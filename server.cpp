#include <ctime>
#include <iostream>
#include <string>
#include <boost/asio.hpp>
#include <boost/thread/thread.hpp>
#include <boost/bind.hpp>
#include <boost/array.hpp>
#include <string>
#include <memory>
#include "boost/filesystem.hpp"
#include <thread>
#include <vector>
#include <fstream>
#include <sstream>

#include "session.h"

using namespace boost;
using namespace boost::system;
using namespace boost::asio;
using boost::asio::ip::tcp;
namespace fs = boost::filesystem;

class server {
public:
    server(boost::asio::io_context &io_context, short port, const std::string &dest)
            : io_context_(io_context),
              acceptor_(io_context, tcp::endpoint(tcp::v4(), port)),
              destination(fs::path{dest})
    {
        start_accept();
    }

private:
    void start_accept() {
        session *new_session = new session(io_context_, destination);
        acceptor_.async_accept(new_session->socket(),
                               boost::bind(&server::handle_accept, this, new_session,
                                           boost::asio::placeholders::error));
    }

    void handle_accept(session *new_session,
                       const boost::system::error_code &error) {
        if (!error) {
            new_session->proceed();
        } else {
            delete new_session;
        }

        start_accept();
    }

    boost::asio::io_context &io_context_;
    tcp::acceptor acceptor_;
    fs::path destination;
};

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: ./server <port> <destination>\n";
        return -1;
    }

    boost::asio::io_context io_context;

    server s(io_context, std::atoi(argv[1]), argv[2]);

    io_context.run();

    return 0;
}
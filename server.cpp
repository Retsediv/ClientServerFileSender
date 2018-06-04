#include <ctime>
#include <iostream>
#include <string>
#include <boost/asio.hpp>
#include <boost/thread/thread.hpp>
#include <boost/bind.hpp>
#include <boost/array.hpp>
#include "boost/filesystem.hpp"
#include <string>
#include <memory>
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

    uint NUM_THREADS = 4;

    boost::asio::io_context io_context;
    boost::asio::io_context::work some_work(io_context);


//    const ip::tcp::endpoint endpoint{ip::tcp::v4(), std::atoi(argv[1])};
//    ip::tcp::acceptor acceptor{io_context, endpoint};

    server s(io_context, std::atoi(argv[1]), argv[2]);

//    acceptor.listen();

    boost::thread_group threads;
    for (int i = 0; i < NUM_THREADS; ++i) {
        threads.create_thread(boost::bind(&io_context::run, &io_context));
    }

    io_context.run();
    threads.join_all();

    return 0;
}
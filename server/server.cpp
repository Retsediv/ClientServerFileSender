#include "io_context_pool.h"
#include "server.h"

using namespace boost;
using namespace boost::system;
using namespace boost::asio;
using boost::asio::ip::tcp;
namespace fs = boost::filesystem;

server::server(const std::string &port, const std::string &dest, size_t io_context_pool_size) :
        io_context_pool_(io_context_pool_size),
        acceptor_(io_context_pool_.get_io_context()),
        destination_(fs::path{dest})
{

    // Open the acceptor with the option to reuse the address (i.e. SO_REUSEADDR).
    tcp::resolver resolver(acceptor_.get_executor().context());
    tcp::endpoint endpoint = *resolver.resolve("127.0.0.1", port).begin();
    acceptor_.open(endpoint.protocol());
    acceptor_.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
    acceptor_.bind(endpoint);
    acceptor_.listen();

    start_accept();
}

void server::start_accept() {
    session *new_session = new session(io_context_pool_.get_io_context(), destination_);
    acceptor_.async_accept(new_session->socket(),
                           boost::bind(&server::handle_accept, this, new_session,
                                       boost::asio::placeholders::error));
}

void server::handle_accept(session *new_session, const boost::system::error_code &error) {
    if (!error) {
        new_session->proceed();
    } else {
        delete new_session;
    }

    start_accept();
}

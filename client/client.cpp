#include "client.h"

using boost::asio::ip::tcp;
namespace fs = boost::filesystem;

void client::handle_resolve(const boost::system::error_code &err,
                            const boost::asio::ip::tcp::resolver::results_type &endpoints) {
    if (!err) {
        boost::asio::async_connect(socket_, endpoints,
                                   boost::bind(&client::handle_connect, this,
                                               boost::asio::placeholders::error));
    } else {
        std::cout << "Error: " << err.message() << "\n";
    }

}

void client::handle_connect(const boost::system::error_code &err) {
    if (!err) {
        // The connection was successful. Send the request.
        boost::system::error_code error{};

        try {
            tcp::resolver::query query(ip_, port_);
            tcp::resolver::iterator endpoint_iterator = resolver_.resolve(query);
            tcp::resolver::iterator end;
            boost::system::error_code error = boost::asio::error::host_not_found;
            while (error && endpoint_iterator != end) {
                socket_.close();
                socket_.connect(*endpoint_iterator++, error);
            }

            char buf[1024];
            std::ifstream source_file(filepath, std::ios_base::binary | std::ios_base::ate);
            size_t file_size = source_file.tellg();
            source_file.seekg(0);


            // first send file name and file size to server
            boost::asio::streambuf request;
            std::ostream request_stream(&request);
            request_stream << fs::path(filepath).filename().c_str() << "\n\n" << file_size << "\n\n";
            boost::asio::write(socket_, request);
            std::cout << "start sending file content" << std::endl;
            std::cout << "file size : " << file_size << std::endl;

            while (true) {
                if (!source_file.eof()) {
                    source_file.read(buf, sizeof(buf));
                    if (source_file.gcount() <= 0) {
                        std::cerr << "read file error " << std::endl;
                        return;
                    }

                    boost::asio::write(socket_, boost::asio::buffer(buf, source_file.gcount()),
                                       boost::asio::transfer_all(),
                                       error);

                    if (error) {
                        std::cerr << "send error:" << error << std::endl;
                        return;
                    }
                } else {
                    break;
                }
            }
            std::cout << "send file " << filepath << " completed successfully.\n";
        }

        catch (std::exception &e) {
            std::cerr << e.what() << std::endl;
        }

        handle_response();
    } else {
        std::cerr << "Error: " << err.message() << "\n";
    }
}

void client::handle_response() {
    boost::array<char, 1024> buf_;
    socket_.read_some(boost::asio::buffer(buf_));

    std::cout << "Response from server: " << buf_.c_array() << std::endl;
}
#include <iostream>
#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <boost/filesystem.hpp>
#include <boost/bind.hpp>

using boost::asio::ip::tcp;
namespace fs = boost::filesystem;


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
    tcp::resolver resolver_;
    tcp::socket socket_;
    boost::asio::streambuf request_;
    boost::asio::streambuf response_;
    std::string filepath;
    std::string ip_;
    std::string port_;

    void handle_resolve(const boost::system::error_code &err,
                        const tcp::resolver::results_type &endpoints) {
        if (!err) {
            // Attempt a connection to each endpoint in the list until we
            // successfully establish a connection.

            std::cout << "before async_connect " << std::endl;

            boost::asio::async_connect(socket_, endpoints,
                                       boost::bind(&client::handle_connect, this,
                                                   boost::asio::placeholders::error));
        } else {
            std::cout << "Error: " << err.message() << "\n";
        }
    }

    void handle_connect(const boost::system::error_code &err) {
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
                if (!source_file) {
                    std::cout << "failed to open " << filepath << std::endl;
                }

                size_t file_size = source_file.tellg();
                source_file.seekg(0);


                // first send file name and file size to server
                boost::asio::streambuf request;
                std::ostream request_stream(&request);
                request_stream << fs::path(filepath).filename().c_str() << "\n\n" << file_size << "\n\n";
                boost::asio::write(socket_, request);
                std::cout << "start sending file content" << std::endl;
                std::cout << "file size : " << file_size << std::endl;

                uintmax_t sended = 0;

                while (true) {
                    if (!source_file.eof()) {
                        source_file.read(buf, sizeof(buf));
                        if (source_file.gcount() <= 0) {
                            std::cout << "read file error " << std::endl;
//                            return __LINE__;
                        }

                        std::cout << "sending to the socket " << std::endl;
                        boost::asio::write(socket_, boost::asio::buffer(buf, source_file.gcount()),
                                           boost::asio::transfer_all(),
                                           error);

                        std::cout << "send : " << buf << std::endl;
                        std::cout << "send : " << source_file.gcount() << std::endl;
                        if (error) {
                            std::cout << "send error:" << error << std::endl;
//                            return __LINE__;
                        }
                    } else {
                        break;
                    }
                }
                std::cout << "send file " << filepath << " completed successfully.\n";

                // boost::asio::write(socket, boost::asio::buffer("<EOF>", source_file.gcount()), boost::asio::transfer_all(), error);
            }

            catch (std::exception &e) {
                std::cerr << e.what() << std::endl;
            }


//            boost::asio::async_write(socket_, request_,
//                                     boost::bind(&client::handle_write_request, this,
//                                                 boost::asio::placeholders::error));

            handle_response();
        } else {
            std::cout << "Error: " << err.message() << "\n";
        }
    }

    void handle_response() {
        // Read the response status line. The response_ streambuf will
        // automatically grow to accommodate the entire line. The growth may be
        // limited by passing a maximum size to the streambuf constructor.

//        boost::asio::async_read_until(socket_, response_, "\r\n",
//                                      boost::bind(&client::handle_read_status_line, this,
//                                                  boost::asio::placeholders::error));

        std::cout << "HANDLIND RESPONSE!!!!!" << std::endl;

        boost::array<char, 1024> buf_;
        size_t len = socket_.read_some(boost::asio::buffer(buf_));

        std::cout << "DATA: " << buf_.c_array() << std::endl;
        }
};

int main(int argc, char *argv[]) {
    if (argc != 4) {
        std::cerr << "Usage: " << argv[0] << " <ip> <port> <file path>" << std::endl;
        std::cerr << "sample: " << argv[0] << " 127.0.0.1 1234 ./from/1.txt" << std::endl;
        return __LINE__;
    }

    boost::asio::io_context io_context;
    client c(io_context, argv[1], argv[2], argv[3]);
    io_context.run();

//
//    try {
//        std::string server_ip_or_host = argv[1];
//        std::string port_string = argv[2];
//
//        boost::asio::io_service io_service;
//        tcp::resolver resolver(io_service);
//
//        tcp::resolver::query query(server_ip_or_host, port_string);
//        tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);
//        tcp::resolver::iterator end;
//        tcp::socket socket(io_service);
//        boost::system::error_code error = boost::asio::error::host_not_found;
//        while (error && endpoint_iterator != end) {
//            socket.close();
//            socket.connect(*endpoint_iterator++, error);
//        }
//
//        if (error)
//            return __LINE__;
//
//        std::cout << "connected to " << argv[1] << std::endl;
//
//        char buf[1024];
//        std::ifstream source_file(argv[3], std::ios_base::binary | std::ios_base::ate);
//        if (!source_file) {
//            std::cout << "failed to open " << argv[2] << std::endl;
//            return __LINE__;
//        }
//
//        size_t file_size = source_file.tellg();
//        source_file.seekg(0);
//
//
//        // first send file name and file size to server
//        boost::asio::streambuf request;
//        std::ostream request_stream(&request);
//        request_stream << fs::path(argv[3]).filename().c_str() << "\n\n" << file_size << "\n\n";
//        boost::asio::write(socket, request);
//        std::cout << "start sending file content" << std::endl;
//        std::cout << "file size : " << file_size << std::endl;
//
//        uintmax_t sended = 0;
//
//        while (true) {
//            if (!source_file.eof()) {
//                source_file.read(buf, sizeof(buf));
//                if (source_file.gcount() <= 0) {
//                    std::cout << "read file error " << std::endl;
//                    return __LINE__;
//                }
//
//                std::cout << "sending to the socket " << std::endl;
//                boost::asio::write(socket, boost::asio::buffer(buf, source_file.gcount()), boost::asio::transfer_all(),
//                                   error);
//
//                std::cout << "send : " << buf << std::endl;
//                std::cout << "send : " << source_file.gcount() << std::endl;
//                if (error) {
//                    std::cout << "send error:" << error << std::endl;
//                    return __LINE__;
//                }
//            } else {
//                break;
//            }
//        }
//        std::cout << "send file " << argv[3] << " completed successfully.\n";
//
////        boost::asio::write(socket, boost::asio::buffer("<EOF>", source_file.gcount()), boost::asio::transfer_all(), error);
//    }
//
//    catch (std::exception &e) {
//        std::cerr << e.what() << std::endl;
//    }
    return 0;
}
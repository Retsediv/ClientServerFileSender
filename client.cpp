#include <iostream>
#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <boost/filesystem.hpp>
#include <fstream>

using boost::asio::ip::tcp;
namespace fs = boost::filesystem;

int main(int argc, char *argv[]) {
    if (argc != 4) {
        std::cerr << "Usage: " << argv[0] << " <id> <port> <file path>" << std::endl;
        std::cerr << "sample: " << argv[0] << " 127.0.0.1 1234 ./from/1.txt" << std::endl;
        return __LINE__;
    }

    try {
        std::string server_ip_or_host = argv[1];
        std::string port_string = argv[2];

        boost::asio::io_service io_service;
        tcp::resolver resolver(io_service);

        tcp::resolver::query query(server_ip_or_host, port_string);
        tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);
        tcp::resolver::iterator end;
        tcp::socket socket(io_service);
        boost::system::error_code error = boost::asio::error::host_not_found;
        while (error && endpoint_iterator != end) {
            socket.close();
            socket.connect(*endpoint_iterator++, error);
        }

        if (error)
            return __LINE__;

        std::cout << "connected to " << argv[1] << std::endl;

        char buf[1024];
        std::ifstream source_file(argv[3], std::ios_base::binary | std::ios_base::ate);
        if (!source_file) {
            std::cout << "failed to open " << argv[2] << std::endl;
            return __LINE__;
        }

        size_t file_size = source_file.tellg();
        source_file.seekg(0);


        // first send file name and file size to server
        boost::asio::streambuf request;
        std::ostream request_stream(&request);
        request_stream << fs::path(argv[3]).filename().c_str() << "\n\n" << file_size << "\n\n";
        boost::asio::write(socket, request);
        std::cout << "start sending file content" << std::endl;
        std::cout << "file size : " << file_size << std::endl;

        uintmax_t sended = 0;

        while(true) {
            if (!source_file.eof()) {
                source_file.read(buf, sizeof(buf));
                if (source_file.gcount() <= 0) {
                    std::cout << "read file error " << std::endl;
                    return __LINE__;
                }

                std::cout << "sending to the socket "  << std::endl;
                boost::asio::write(socket, boost::asio::buffer(buf, source_file.gcount()), boost::asio::transfer_all(),
                                   error);

                std::cout << "send : " << buf << std::endl;
                std::cout << "send : " << source_file.gcount() << std::endl;
                if (error) {
                    std::cout << "send error:" << error << std::endl;
                    return __LINE__;
                }
            } else {
                break;
            }
        }
        std::cout << "send file " << argv[3] << " completed successfully.\n";

//        boost::asio::write(socket, boost::asio::buffer("<EOF>", source_file.gcount()), boost::asio::transfer_all(), error);
    }

    catch (std::exception &e) {
        std::cerr << e.what() << std::endl;
    }
    return 0;
}
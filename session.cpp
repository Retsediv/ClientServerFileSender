#include <cstdlib>
#include <iostream>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <fstream>
#include <boost/filesystem.hpp>

using boost::asio::ip::tcp;
namespace fs = boost::filesystem;

class session {
public:
    session(boost::asio::io_context &io_context, const fs::path &destination) : socket_(io_context), destination_(destination) {}

    tcp::socket &socket() { return socket_; }

    void proceed() {
        handle_read();
    }

private:
    tcp::socket socket_;
    enum {
        max_length = 1024
    };

    char data_[max_length];

    boost::array<char, 1024> buf_;
    fs::path destination_;

    std::string file_path_;
    size_t file_size_ = 0;

    void handle_read() {
        get_file_properties();

        try {
            boost::asio::streambuf request_buf;
            std::istream request_stream(&request_buf);

            std::cout << "Getting file from client." << std::endl;
            boost::system::error_code error;

            fs::path outfile_dest = destination_ / fs::path{file_path_};
            std::cout << "file_path : " << outfile_dest.c_str() << std::endl;


            std::ofstream output_file(outfile_dest.c_str(), std::ios_base::binary);
            if (!output_file) {
                std::cout << "failed to open " << file_path_ << std::endl;
                exit(-1);
            }

            // write extra bytes to file
            do {
                request_stream.read(buf_.c_array(), (std::streamsize) buf_.size());
                std::cout << __FUNCTION__ << " write " << request_stream.gcount() << " bytes.\n";
                output_file.write(buf_.c_array(), request_stream.gcount());
            } while (request_stream.gcount() > 0);

            for (;;) {
                size_t len = socket_.read_some(boost::asio::buffer(buf_), error);
                if (len > 0)
                    output_file.write(buf_.c_array(), (std::streamsize) len);
                if (output_file.tellp() == (std::fstream::pos_type) (std::streamsize) file_size_)
                    break; // file was received
                if (error) {
                    std::cout << error << std::endl;
                    break;
                }
            }
            std::cout << "received " << output_file.tellp() << " bytes.\n";
        }
        catch (std::exception &e) {
            std::cerr << e.what() << std::endl;
        }

        //////////////////////////////////////
        //////////////////////////////////////
        //////////////////////////////////////

//        boost::asio::async_write(socket_, boost::asio::buffer("Everything is okay!", 22),
//                                 boost::bind(&session::handle_write, this, boost::asio::placeholders::error));

        std::cout << "after read " << std::endl;
    }

    void send_response(const std::string &res, uintmax_t size){
        boost::asio::async_write(socket_, boost::asio::buffer(res, size),
                                 boost::bind(&session::handle_write, this, boost::asio::placeholders::error));
    }

    void handle_write(const boost::system::error_code &error) {
        if (!error) {
//            socket_.async_read_some(boost::asio::buffer(data_, max_length),
//                                    boost::bind(&session::handle_read, this,
//                                                boost::asio::placeholders::error,
//                                                boost::asio::placeholders::bytes_transferred));
        } else {
            delete this;
        }
    }

    void get_file_properties(){
        boost::asio::streambuf request_buf;
        boost::asio::read_until(socket_, request_buf, "\n\n");
        std::cout << "request size:" << request_buf.size() << "\n";
        std::istream request_stream(&request_buf);

        request_stream >> file_path_;
        request_stream >> file_size_;
        request_stream.read(buf_.c_array(), 2); // eat the "\n\n"

    }
};

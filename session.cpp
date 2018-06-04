#include "session.h"

using boost::asio::ip::tcp;
namespace fs = boost::filesystem;

void session::handle_read() {
    get_file_properties();

    try {
        boost::asio::streambuf request_buf;
        std::istream request_stream(&request_buf);
        boost::system::error_code error;

        fs::path outfile_dest = destination_ / fs::path{file_path_};

        // TODO: fix this shit
//        if(fs::is_regular_file(outfile_dest)){
//            std::cout << "such file is already exists: " << outfile_dest << std::endl;
//        }

        std::ofstream output_file(outfile_dest.c_str(), std::ios_base::binary);
        if (!output_file) {
            send_response("failed to create destination file");
            std::cout << "failed to create destination file" << file_path_ << std::endl;
            return;
        }

        // write extra bytes to file
        do {
            request_stream.read(buf_.c_array(), (std::streamsize) buf_.size());
            std::cout << "write extra bytes to file" << __FUNCTION__ << " write " << request_stream.gcount() << " bytes.\n";
            output_file.write(buf_.c_array(), request_stream.gcount());
        } while (request_stream.gcount() > 0);

        while(true) {
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

        send_response("File was successfully received and saved");
    }
    catch (std::exception &e) {
        std::cerr << e.what() << std::endl;
    }
}


void session::get_file_properties() {
    boost::asio::streambuf request_buf;
    boost::asio::read_until(socket_, request_buf, "\n\n");
    std::istream request_stream(&request_buf);

    request_stream >> file_path_;
    request_stream >> file_size_;
    request_stream.read(buf_.c_array(), 2); // eat the "\n\n"
}


void session::send_response(const std::string &res) {
    boost::asio::streambuf request;
    std::ostream request_stream(&request);
    request_stream << res << "\n\n";
    boost::asio::write(socket_, request);

//    delete this;
}
#include "session.h"
#include "../helpers.h"

using boost::asio::ip::tcp;
namespace fs = boost::filesystem;

void session::handle_read() {
    get_file_properties();

    // getting and saving the file
    try {
        boost::asio::streambuf request_buf;
        std::istream request_stream(&request_buf);
        boost::system::error_code error;

        fs::path outfile_dest = destination_ / fs::path{file_path_};

        if (file_exists(outfile_dest.c_str())) {
            send_response("Such file is already exists");
            return;
        }

        std::ofstream output_file(outfile_dest.c_str(), std::ios_base::binary);

        if (!output_file) {
            send_response("failed to create destination file");
            return;
        }

        // write extra bytes to file
        do {
            request_stream.read(buf_.c_array(), (std::streamsize) buf_.size());
            output_file.write(buf_.c_array(), request_stream.gcount());
        } while (request_stream.gcount() > 0);

        while (true) {
            size_t bytes_transferred = socket_.read_some(boost::asio::buffer(buf_));

            if (bytes_transferred > 0)
                output_file.write(buf_.c_array(), (std::streamsize) bytes_transferred);

            if (output_file.tellp() == (std::fstream::pos_type) (std::streamsize) file_size_)
                break; // file was received

            if (error) {
                std::cerr << "Error while operating with file: " << error << std::endl;
                send_response("Error while operating with file");
                return;
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

    std::cout << "Sending response: " << res << std::endl;

    boost::asio::write(socket_, request);
}
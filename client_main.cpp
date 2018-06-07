#include <boost/program_options.hpp>
#include "client/client.h"
#include "helpers.h"

using std::string;

int main(int argc, char *argv[]) {
    // Program options configuration
    namespace po = boost::program_options;

    po::options_description desc(" Usage: ./client --host <ip or host> -p <port> -f <file to send> \n Example: ./client --host 127.0.0.1 -p 5000 -f ./great_file.txt\n\nOptions");
    desc.add_options()
            ("help,h", "Help me! or 'Houston, we have a problem!'")
            ("version,v", "Version of the program")
            ("host,i", po::value<string>()->default_value("127.0.0.1"), "Host address to connect")
            ("port,p", po::value<string>()->default_value("5000"), "Port to connect")
            ("file,f", po::value<string>()->default_value("./test.txt"), "Path to the file which will be transported");

    po::variables_map vm;
    po::store(po::command_line_parser(argc, argv).options(desc).run(), vm);
    po::notify(vm);

    // Validate values
    string host = vm["host"].as<string>();
    string port = vm["port"].as<string>();
    string file_path = vm["file"].as<string>();

    // Print help and version of the program
    if (vm.count("help")) {
        std::cout << desc << std::endl;
        return 0;
    }

    if (vm.count("version")) {
        std::cout << "Version: 1.0" << std::endl;
        return 0;
    }


    // validate input file
    if (!file_exists(file_path.c_str())) {
        std::cerr << "Failed to open file(check if it exists): " << file_path << std::endl;
        return EXIT_FAILURE;
    }

    if (!is_regular_file(file_path.c_str())) {
        std::cout << "It's not a regular file: " << file_path << std::endl;
        return EXIT_FAILURE;
    }

    // start client
    boost::asio::io_context io_context;
    client c(io_context, host, port, file_path);
    io_context.run();

    return 0;
}

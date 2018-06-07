#include <iostream>
#include <boost/program_options.hpp>

#include "server/server.h"
#include "helpers.h"

using std::string;

int main(int argc, char *argv[]) {
    // Program options configuration
    namespace po = boost::program_options;

    po::options_description desc(" Usage: ./server -p <port> -f <destination folder> \n Example: ./server -p 1234 -f ./my_great_folder\n\nOptions");
    desc.add_options()
            ("help,h", "Help me! or 'Houston, we have a problem!'")
            ("version,v", "Version of the program")
            ("port,p", po::value<string>()->default_value("5000"), "Port, where server will be bind")
            ("folder,f", po::value<string>()->default_value("./where"), "Path to the directory, where files will be stored");

    po::variables_map vm;
    po::store(po::command_line_parser(argc, argv).options(desc).run(), vm);
    po::notify(vm);

    // Validate values
    string port = vm["port"].as<string>();
    string folder = vm["folder"].as<string>();

    // Print help and version of the program
    if (vm.count("help")) {
        std::cout << desc << std::endl;
        return 0;
    }

    if (vm.count("version")) {
        std::cout << "Version: 1.0" << std::endl;
        return 0;
    }

    // validate path to the folder
    if (!file_exists(folder.c_str())) {
        std::cerr << "You path is incorrect! Such folder is not exists!" << std::endl;
        return EXIT_FAILURE;
    }

    if (!is_directory(folder.c_str())) {
        std::cerr << "You path is incorrect! It's not a directory" << std::endl;
        return EXIT_FAILURE;
    }

    // number of threads for server
    const size_t NUM_THREADS = 4;

    // init and run the server
    server s{port, folder, NUM_THREADS};

    s.run();

    return 0;
}
# Client-server application which allows sending files from client to server

### Requirements
 - Boost library (1.66 version), otherwise you may have problems with io_context
 - Cmake (minimum version: 3.10)
 
### Compiling
```bash
$ mkdir Build
$ cd Build
$ cmake -DCMAKE_CXX_COMPILER=/usr/bin/g++  ../
$ make
```

## Usage

### Run the server
```bash
### ./client <port> <destination folder>
$ ./server 5000 ./dest
```

### Run the client
```bash
### ./client <address> <port> <file to send>
$ /client 127.0.0.1 5000 ./file.txt
```




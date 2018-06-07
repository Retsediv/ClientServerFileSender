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
### ./server -p <port> -f <destination folder>
$ ./server -p 1234 -f ./dest
```
For more details(help) check:
```bash
$ ./server -h
```

### Run the client
```bash
### ./client --host <ip or host> -p <port> -f <file to send>
$ ./client --host 127.0.0.1 -p 5000 -f ./file.txt
```

For more details(help) check:
```bash
$ ./cilent -h
```



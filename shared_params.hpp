#ifndef SHARED_PARAMS_HPP
#define SHARED_PARAMS_HPP

//Temporary constat value for max client connections
//TODO: add max client connections selection block (or something)
#define MAX_CLIENTS 100

#define MAX_MESSAGE_LENGTH 4096

#define SERVER_HOSTNAME "localhost"

//Temporary constant value for port
//TODO: add port input section
#define SERVER_PORT "49153"

//POSIX-compatible constant value for infinite waiting time
#define INFTIM -1

//Constant value for quantity of polling file descriptors
//in client module (stdin and client_socket)
#define CLIENT_POLL_FDS_COUNT 2

#endif // SHARED_PARAMS_HPP

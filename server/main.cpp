#include <arpa/inet.h>
#include <iostream>
#include <netdb.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "../shared_params.h"

using namespace std;

//TODO: uncomment argc, argv to use input parameters
int main(/*int argc, char *argv[]*/)
{

    //TODO: add usage output section

    //Temporary constant value for port
    //TODO: add port input section

    int server_socket;

    if ((server_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1) {
        cerr << "socket error." << endl;
        return server_socket;
    }

    int sockopt_val = 1; //socket_option_value

    if (setsockopt(server_socket,
                   SOL_SOCKET,
                   SO_REUSEADDR | SO_REUSEPORT,
                   &sockopt_val,
                   sizeof(sockopt_val)) == -1) {
        cerr << "setsockopt reuseaddr error." << endl;
        exit(EXIT_FAILURE);
    }

    addrinfo addrinfo_reqs = {
        .ai_family = AF_INET,
        .ai_socktype = SOCK_STREAM,
        .ai_protocol = IPPROTO_TCP
    }; //address_info_requirements

    int status;
    addrinfo *server_addrinfo;

    if ((status = getaddrinfo(NULL,
                              SERVER_PORT,
                              &addrinfo_reqs,
                              &server_addrinfo)) != 0) {
        cerr << "getaddrinfo error: " << gai_strerror(status) << endl;
        return status;
    }

    if ((status = bind(server_socket,
                       server_addrinfo->ai_addr,
                       server_addrinfo->ai_addrlen)) == -1) {
        cerr << "bind error" << endl;
        return status;
    }

    //Temporary constat value for max client connections
    //TODO: add max client connections selection block (or something)
    if ((status = listen(server_socket, MAX_CLIENTS)) == -1) {
        cerr << "listen error" << endl;
        return -1;
    }

    char server_ip_address[INET_ADDRSTRLEN];
    if (inet_ntop(AF_INET,
                  &((sockaddr_in *)(server_addrinfo->ai_addr))->sin_addr,
                  server_ip_address,
                  INET_ADDRSTRLEN) == NULL) {
        cerr << "inet_ntop error" << endl;
        return NULL;
    }

    cout << "server " << server_ip_address << " started on port " << SERVER_PORT << "." << endl;

    //TODO: temporary declared as int (should be int[max_clients])
    int client_socket;
    client_socket = accept(server_socket,
                           server_addrinfo->ai_addr,
                           &server_addrinfo->ai_addrlen);
    char client_message[MAX_MESSAGE_LENGTH];

    while (read(client_socket, &client_message, (MAX_MESSAGE_LENGTH * sizeof(char))) != -1)
    {
        cout << "client message recieved: " << client_message << endl;

        //TODO: do we quit if message was sent with error?
        if (write(client_socket, &client_message, MAX_MESSAGE_LENGTH * sizeof(char)) == -1) {
            cerr << "message sending error" << endl;
            return -1;
        }
        else {
            cout << "message sent successfully" << endl;
        }

        if (strcmp(client_message, "quit") == 0) {
            close(client_socket);
            cout << "quit message recieved, client quits" << endl;
            break;
        }
        cout.flush();
    }

    return 0;
}

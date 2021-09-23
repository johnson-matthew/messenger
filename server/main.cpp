#include <iostream>
#include <netdb.h>
#include <netinet/in.h>
#include <string>
#include <sys/socket.h>
#include <unistd.h>

using namespace std;

//TODO: uncomment argc, argv to use input parameters
int main(/*int argc, char *argv[]*/)
{

    //TODO: add usage output section

    //Temporary constant value for port
    //TODO: add port input section
    string server_port = "49153";

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

    struct addrinfo addrinfo_reqs = {0}; //address_info_requirements
    addrinfo_reqs.ai_family = AF_INET;
    addrinfo_reqs.ai_socktype = SOCK_STREAM;
    addrinfo_reqs.ai_flags = AI_PASSIVE;

    int status;
    struct addrinfo *server_addrinfo;

    if ((status = getaddrinfo(NULL,
                              server_port.c_str(),
                              &addrinfo_reqs,
                              &server_addrinfo)) != 0) {
        cerr << "getaddrinfo error: " << gai_strerror(status) << endl;
        return status;
    }

    if (bind(server_socket, server_addrinfo->ai_addr, server_addrinfo->ai_addrlen) == -1) {
        cerr << "bind error" << endl;
        return -1;
    }

    //Temporary constat value for max client connections
    //TODO: add max client connections selection block (or something)
    const int max_clients = 100;
    if (listen(server_socket, max_clients) == -1) {
        cerr << "listen error" << endl;
        return -1;
    }

    cout << "Server " << server_addrinfo->ai_addr->sa_data << " started to listen on port " << server_port << "." << endl;

    //TODO: temporary declared as int (should be int[max_clients])
    int client_socket;
    client_socket = accept(server_socket, server_addrinfo->ai_addr, &server_addrinfo->ai_addrlen);
    char client_message[256];

    while (read(client_socket, client_message, 256) != -1)
    {
        cout << "client message recieved: " << client_message << endl;

        //TODO: do we quit if message was sent with error?
        if (write(client_socket, client_message, 256) == -1) {
            cerr << "message sending error" << endl;
            return -1;
        }
        else {
            cout << "message sent successfully" << endl;
        }

        if (client_message == "quit") {
            close(client_socket);
            cout << "quit message recieved, client quits" << endl;
            break;
        }
        cout.flush();
    }

    return 0;
}

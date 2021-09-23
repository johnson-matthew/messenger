#include <iostream>
#include <netdb.h>
#include <netinet/in.h>
#include <string>
#include <sys/socket.h>

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

    //TODO: are these areas of visibility so nessecary? code readability may suffer saving 4 bytes of int.
    {
        int sockopt_val = 1; //socket_option_value

        if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &sockopt_val, sizeof(sockopt_val)) == -1) {
           cerr << "setsockopt reuseaddr error." << endl;
            exit(EXIT_FAILURE);
           }
    }

    struct sockaddr_in my_addr;
    my_addr.sin_family = AF_INET;
    my_addr.sin_port = stoi(server_port);

    //TODO: are these areas of visibility so nessecary? code readability may suffer saving ?? bytes of in_addr.
    {
        struct in_addr temp_internet_address = {
            .s_addr = INADDR_ANY
        };

        my_addr.sin_addr = temp_internet_address;
    }

    //TODO: bind, listen, accept

    //Temporary constat value for max client connections
    //TODO: add max client connections selection block (or something)
    const int max_clients = 100;

    int client_socket[max_clients] = {0};

    return 0;
}

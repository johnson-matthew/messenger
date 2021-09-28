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

    //Temporary constant values for hostname and port
    //TODO: add hostname and port input section

    struct addrinfo addrinfo_reqs = {
        .ai_family = AF_INET,
        .ai_socktype = SOCK_STREAM,
        .ai_protocol = IPPROTO_TCP
    }; //address_info_requirements

    struct addrinfo *server_addrinfo;

    int status;

    if ((status = getaddrinfo(SERVER_HOSTNAME,
                              SERVER_PORT,
                              &addrinfo_reqs,
                              &server_addrinfo)) != 0) {
        cerr << "getaddrinfo error: " << gai_strerror(status) << endl;
        return status;
    }

    int client_socket;

    if ((client_socket = socket(server_addrinfo->ai_family,
                                server_addrinfo->ai_socktype,
                                server_addrinfo->ai_protocol)) == -1) {
        cerr << "socket error" << endl;
        return client_socket;
    }

    if ((status = connect(client_socket,
                          server_addrinfo->ai_addr,
                          server_addrinfo->ai_addrlen)) == -1) {
        cerr << "connect error" << endl;
        return status;
    }

    //"Magic" escape-sequence to clear console
    // \e means escape code
    // ESC[2J clears screen from top (2) to bottom (J);
    // ESC[H moves cursor to home position
    cout << "\e[2J\e[H";

    cout << "connection with host " << SERVER_HOSTNAME <<
            " on port " << SERVER_PORT << " established." << endl;

    while (true) {
        char message[MAX_MESSAGE_LENGTH];
        cout << "please, enter your message and press \"Enter\": ";
        cin >> message;

        //TODO: do we quit if message was sent with error?
        if (write(client_socket, message, MAX_MESSAGE_LENGTH * sizeof(char)) == -1) {
            cerr << "message sending error" << endl;
        }
        else {
            cout << "message sent successfully" << endl;
        }

        if (strcmp(message, "quit") == 0) {
            cout << "client quits" << endl;
            close(client_socket);
            break;
        }

        //TODO: also do we quit if message wasn't recieved?
        if (read(client_socket, message, MAX_MESSAGE_LENGTH * sizeof(char)) == -1) {
            cerr << "message recieving error" << endl;
        }
        else {
            cout << "server message recieved: " << message << endl;
        }

    }

    return 0;
}

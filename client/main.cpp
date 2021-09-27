#include <iostream>
#include <netdb.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

using namespace std;

//TODO: uncomment argc, argv to use input parameters
int main(/*int argc, char *argv[]*/)
{

    //TODO: add usage output section

    //Temporary constant values for hostname and port
    //TODO: add hostname and port input section
    char server_hostname[] = "localhost";
    char server_port[] = "49153";

    int status;

    struct addrinfo addrinfo_reqs = {
        .ai_family = AF_INET,
        .ai_socktype = SOCK_STREAM,
        .ai_protocol = IPPROTO_TCP
    }; //address_info_requirements

    struct addrinfo *server_addrinfo;

    if ((status = getaddrinfo(server_hostname,
                              server_port,
                              &addrinfo_reqs,
                              &server_addrinfo)) != 0) {
        cerr << "getaddrinfo error: " << gai_strerror(status) << endl;
        return status;
    }

    int client_socket;

    if ((client_socket = socket(server_addrinfo->ai_family,
                                server_addrinfo->ai_socktype,
                                server_addrinfo->ai_protocol)) == -1) {
        cerr << "socket error." << endl;
        return client_socket;
    }

    if ((status = connect(client_socket,
                          server_addrinfo->ai_addr,
                          server_addrinfo->ai_addrlen)) == -1) {
        cerr << "connect error." << endl;
        return status;
    }

    cout << "Connection with host " << server_hostname << " on port " << server_port << " established successfully." << endl;

    while (true) {
        char client_message[4096];
        cout << "please, enter your message and press \"Enter\": ";
        cin >> client_message;

        if (strcmp(client_message, "quit") == 0) {
            if (write(client_socket, client_message, 4096 * sizeof(char)) == -1) {
                cerr << "quit message sending error" << endl;
                return -1;
            }
            else {
                close(client_socket);
                cout << "quit message sent successfully, client quits" << endl;
                break;
            }
        }

        //TODO: do we quit if message was sent with error?
        if (write(client_socket, client_message, 4096 * sizeof(char)) == -1) {
            cerr << "message sending error" << endl;
            return -1;
        }
        else {
            cout << "message sent successfully" << endl;
        }

        cout.flush();

        //TODO: also do we quit if message wasn't recieved?
        if (read(client_socket, client_message, 4096 * sizeof(char)) == -1) {
            cerr << "message recieving error" << endl;
            return -1;
        }

        cout << "server message recieved: " << client_message << endl;
    }

    return 0;
}

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

    //Temporary constant values for hostname and port
    //TODO: add hostname and port input section
    string server_hostname = "localhost";
    string server_port = "49153";

    int status;

    struct addrinfo addrinfo_reqs = {0}; //address_info_requirements
    addrinfo_reqs.ai_family = AF_INET;
    addrinfo_reqs.ai_socktype = SOCK_STREAM;
    addrinfo_reqs.ai_protocol = IPPROTO_TCP;

    struct addrinfo *server_addrinfo;

    if ((status = getaddrinfo(server_hostname.c_str(),
                              server_port.c_str(),
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

    cout << "Connection with host " << server_hostname << " on port " << server_port << "established successfully." << endl;

    while (true) {
        string client_message;
        cout << "please, enter your message and press \"Enter\": ";
        cin >> client_message;

        if (client_message == "quit") {
            if (write(client_socket, client_message.c_str(), client_message.size()) == -1) {
                cerr << "quit message sending error" << endl;
                return -1;
            }
            else {
                close(client_socket);
                cout << "quit message sent successfully, client quits" << endl;
            }
        }

        //TODO: do we quit if message was sent with error?
        if (write(client_socket, client_message.c_str(), client_message.size()) == -1) {
            cerr << "message sending error" << endl;
            return -1;
        }
        else {
            cout << "message sent successfully" << endl;
        }

        cout.flush();
        string server_message = "";

        //TODO: also do we quit if message wasn't recieved?
        if (read(client_socket, &server_message, server_message.size()) == -1) {
            cerr << "message recieving error" << endl;
            return -1;
        }

        cout << "server message recieved: " << server_message << endl;
    }

    return 0;
}

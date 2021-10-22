#include <iostream>
#include <netdb.h>
#include <poll.h>
#include <pthread.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "../shared_params.hpp"

using namespace std;

//TODO: uncomment argc, argv to use input parameters
int main(/*int argc, char *argv[]*/)
{

    //TODO: add usage output section

    //Temporary constant values for hostname and port
    //TODO: add hostname and port input section

    addrinfo addrinfo_reqs = {
        .ai_flags = 0,
        .ai_family = AF_INET,
        .ai_socktype = SOCK_STREAM,
        .ai_protocol = IPPROTO_TCP,
        .ai_addrlen = 0,
        .ai_addr = nullptr,
        .ai_canonname = nullptr,
        .ai_next = nullptr
    }; //address_info_requirements

    addrinfo *server_addrinfo;

    int status = getaddrinfo(SERVER_HOSTNAME,
                             SERVER_PORT,
                             &addrinfo_reqs,
                             &server_addrinfo);

    if (status != 0) {
        cerr << "getaddrinfo() error: " << gai_strerror(status) << endl;
        return status;
    }

    int client_socket = socket(server_addrinfo->ai_family,
                               server_addrinfo->ai_socktype,
                               server_addrinfo->ai_protocol);

    if (client_socket == -1) {
        cerr << "socket() error: " << strerror(errno) << endl;
        return errno;
    }

    if (connect(client_socket,
                server_addrinfo->ai_addr,
                server_addrinfo->ai_addrlen) == -1) {
        cerr << "connect() error: " << strerror(errno) << endl;
        return errno;
    }

    //"Magic" escape-sequence to clear console
    // \e means escape code
    // ESC[2J clears screen from top (2) to bottom (J);
    // ESC[H moves cursor to home position
    cout << "\e[2J\e[H";

    cout << "connection with host " << SERVER_HOSTNAME <<
            " on port " << SERVER_PORT << " established." << endl;

    pollfd pollsd_set[CLIENT_POLL_FDS_COUNT] = {
        {
            .fd = client_socket,
            .events = POLLIN,
            .revents = 0
        },

        {
            .fd = STDIN_FILENO,
            .events = POLLIN,
            .revents = 0
        }
    }; // polling_socket_descriptors_set

    for (;;) {

        char message[MAX_MESSAGE_LENGTH] = {0};
        int poll_retval = poll(pollsd_set, CLIENT_POLL_FDS_COUNT, INFTIM);

        if (poll_retval == -1) {
            cerr << "poll() error: " << strerror(errno) << endl;
            continue;
            //TODO: return?
            //return errno;
        }
        else if (poll_retval == 0) {
            cerr << "timeout reached: no data recieved after infinite time." << endl;
        }
        else {
            for (int i = 0; i < CLIENT_POLL_FDS_COUNT; i++) {

                if (pollsd_set[i].revents & POLLIN) {

                    if (pollsd_set[i].fd == STDIN_FILENO) {
                        cin.getline(message, MAX_MESSAGE_LENGTH * sizeof(char));

                        if (send(client_socket, message, MAX_MESSAGE_LENGTH * sizeof(char), 0) == -1) {
                            cerr << "send() error: " << strerror(errno) << endl;
                        }
                        continue;
                    }
                    else if (pollsd_set[i].fd == client_socket) {
                        int bytes_recieved = recv(pollsd_set[i].fd, message, MAX_MESSAGE_LENGTH * sizeof(char), 0);

                        if (bytes_recieved == -1) {
                            cerr << "recv() error: " << strerror(errno) << endl;
                        }
                        else {
                            cout << message << endl;
                        }
                        continue;
                    }
                }
            }
        }
    }

/*    pthread_t incoming_message_handling_thread;

    int retval = pthread_create(&incoming_message_handling_thread, NULL, &handle_incoming_message, NULL);

    if(retval != 0) {
        cerr << "pthread_create() error: handle_incoming_message." << endl;
        return EXIT_FAILURE;
    }
    else {
        cout << "handle_incoming_message thread created successfully." << endl;
    }

//    retval = pthread_detach(incoming_message_handling_thread);

    if(retval != 0) {
        cerr << "pthread_detach() error: handle_incoming_message." << endl;
        return EXIT_FAILURE;
    }
    else {
        cout << "handle_incoming_message thread detached successfully." << endl;
    }
*/
    /*while (true) {
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
    }*/
//TODO: poll sets errno, use it
    return 0;
}

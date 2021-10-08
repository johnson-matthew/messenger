#include <iostream>
#include <netdb.h>
#include <poll.h>
#include <pthread.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "../shared_params.hpp"

using namespace std;

char message[MAX_MESSAGE_LENGTH];
int client_socket;

void* handle_incoming_message(void *args) {
    pollfd pollsd = {0}; // polling_socket_descriptor
    pollsd.fd = client_socket;
    pollsd.events = POLLIN;

    for(;;) {
        int poll_retval = poll(&pollsd, 1, -1);

        if (poll_retval == -1) {
            cerr << "poll() error: " << strerror(errno) << endl;
            continue;
            //TODO: return?
            //return errno;
        }

        if (pollsd.revents & POLLIN) {
            int bytes_recieved = recv(pollsd.fd, message, MAX_MESSAGE_LENGTH * sizeof(char), 0);
            cout << "here" << endl;

            if (bytes_recieved == 0) {
                //Server disconnected
                close(pollsd.fd);
                cout << "server closed connection." << endl;
                return nullptr;
            }
            else if (bytes_recieved == -1) {
                cerr << "recv() error: " << strerror(errno) << endl;
            }
            else {
                printf("%s\n", message);
            }
        }
    }
}

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

    client_socket = socket(server_addrinfo->ai_family,
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
/*
    fd_set rfds;

    for (;;) {
        FD_ZERO(&rfds);
        FD_SET(0, &rfds);
        int maxfd = 0;
        int retval;
        FD_SET(client_socket, &rfds);
        if(maxfd < client_socket) {
            maxfd = client_socket;
        }
        timeval tv;
        tv.tv_sec = 5;
        tv.tv_usec = 0;
//        int bytes_recieved = recv(pollsd.fd, message, MAX_MESSAGE_LENGTH * sizeof(char), 0);
        cout << "here" << endl;
        int bytes_recieved = select(maxfd+1, &rfds, NULL, NULL, &tv);
        if(retval == -1){
            printf("select Error, client program exit\n");
            break;
        }else if(retval == 0){
            printf("The client does not have any input information, and the server does not have any information coming. waiting...\n");
            continue;
        }else{

            if(FD_ISSET(client_socket,&rfds)){
                bytes_recieved = recv(client_socket, message, MAX_MESSAGE_LENGTH * sizeof(char), 0);
                cout << message << endl;
            }
            When the user enters the information, he begins to process the information and send it.
            if(FD_ISSET(0, &rfds)){
                fgets(message, MAX_MESSAGE_LENGTH * sizeof(char), stdin);
                send(client_socket, message, MAX_MESSAGE_LENGTH * sizeof(char),0); //Send out

            }
        }
    }
*/


    pthread_t incoming_message_handling_thread;

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

    while (true) {
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
    }

    return 0;
}

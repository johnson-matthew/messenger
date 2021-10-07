#include <arpa/inet.h>
#include <errno.h>
#include <iostream>
#include <list>
#include <netdb.h>
#include <netinet/in.h>
#include <poll.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "../shared_params.hpp"

using namespace std;

//TODO: global variables! it's a blasphemy!))

int listen_socket;
addrinfo *server_addrinfo;
list<int> client_sockets_list;


//TODO: think out on thread exit conditions
void* get_client_socket(void *args) {
    for (;;) {
        int client_socket = accept(listen_socket,
                                   server_addrinfo->ai_addr,
                                   &server_addrinfo->ai_addrlen);

        if (client_socket == -1) {
            cerr << "accept() error: " << strerror(errno) << endl;
        }
        else {
            client_sockets_list.push_back(client_socket);
            char client_ip_address[INET_ADDRSTRLEN];
            if (inet_ntop(AF_INET,
                          &((sockaddr_in *)(server_addrinfo->ai_addr))->sin_addr,
                          client_ip_address,
                          INET_ADDRSTRLEN) == NULL) {
                cerr << "inet_ntop() error: " <<strerror(errno) << endl;
            }
            // TODO: output a message informing that client is connected
            cout << "server accepted client from " << client_ip_address << " successfully." << endl;
        }
    }
    pthread_exit(EXIT_SUCCESS);
}

//TODO: uncomment argc, argv to use input parameters
int main(/*int argc, char *argv[]*/)
{

    //TODO: add usage output section

    //Temporary constant value for port
    //TODO: add port input section

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

    int getaddrinfo_status = getaddrinfo(NULL,
                             SERVER_PORT,
                             &addrinfo_reqs,
                             &server_addrinfo);

    if (getaddrinfo_status != 0) {
        cerr << "getaddrinfo() error: " << gai_strerror(getaddrinfo_status) << endl;
        return getaddrinfo_status;
    }

    listen_socket = socket(server_addrinfo->ai_family,
                           server_addrinfo->ai_socktype,
                           server_addrinfo->ai_protocol);

    if (listen_socket == -1) {
        cerr << "socket() error: " << strerror(errno) << endl;
        return errno;
    }

    //set SO_REUSEADDR and SO_REUSEPORT flags on a listen_socket to "true" value (1)
    int listen_socket_flag = (int)true;

    if (setsockopt(listen_socket,
                   SOL_SOCKET,
                   SO_REUSEADDR | SO_REUSEPORT,
                   &listen_socket_flag,
                   sizeof(listen_socket_flag)) == -1) {
        cerr << "setsockopt() error: " << strerror(errno) << endl;
        return errno;
    }

    if (bind(listen_socket,
             server_addrinfo->ai_addr,
             server_addrinfo->ai_addrlen) == -1) {
        close(listen_socket);
        freeaddrinfo(server_addrinfo);
        cerr << "bind() error: " << strerror(errno) << endl;
        return errno;
    }

    //Temporary constat value for max client connections
    //TODO: add max client connections selection block (or something)
    if (listen(listen_socket, MAX_CLIENTS) == -1) {
        cerr << "listen() error: " << strerror(errno) << endl;
        return errno;
    }

    char server_ip_address[INET_ADDRSTRLEN];
    if (inet_ntop(AF_INET,
                  &((sockaddr_in *)(server_addrinfo->ai_addr))->sin_addr,
                  server_ip_address,
                  INET_ADDRSTRLEN) == NULL) {
        cerr << "inet_ntop() error: " <<strerror(errno) << endl;
        return errno;
    }

    cout << "server " << server_ip_address << " started on port " << SERVER_PORT << "." << endl;

    pthread_t get_client_socket_thread;
    int retval = pthread_create(&get_client_socket_thread, NULL, &get_client_socket, NULL);
    //get_client_socket_thread.detach();

    cout << "get_client_socket_thread detached successfully." << endl;

    pollfd pollsd_set[MAX_CLIENTS]; // polling_socket_descriptors_set
    pollsd_set[0].fd = listen_socket;
    pollsd_set[0].events = POLLIN;
    int pollsd_count = 1; // polling_socket_descriptors_count

    for(;;) {
        int poll_count = poll(pollsd_set, pollsd_count, -1);

        if (poll_count == -1) {
            cerr << "poll() error: " << strerror(errno) << endl;
            return errno;
        }
/*
        for (int i = 0; i < pollsd_count; i++) {

            if (pollsd_set[i].revents & POLLIN) {

                if (pollsd_set[i].fd == listen_socket) {

                    int client_socket = accept(listen_socket,
                                               server_addrinfo->ai_addr,
                                               &server_addrinfo->ai_addrlen);

                    if (client_socket == -1) {
                        cerr << "accept() error: " << strerror(errno) << endl;
                    }
                    else {
                        pollsd_set[pollsd_count].fd = client_socket;
                        pollsd_set[pollsd_count].events = POLLIN;
                        pollsd_count++;


                        char client_ip_address[INET_ADDRSTRLEN];
                        if (inet_ntop(AF_INET,
                                      &((sockaddr_in *)(server_addrinfo->ai_addr))->sin_addr,
                                      client_ip_address,
                                      INET_ADDRSTRLEN) == NULL) {
                            cerr << "inet_ntop() error: " <<strerror(errno) << endl;
                            return errno;
                        }

                        cout << "server " << server_ip_address << " started on port " << SERVER_PORT << "." << endl;


                    }

                }

            }

        }
*/
    }


    //TODO: temporary declared as int (should be int[max_clients])


    char message[MAX_MESSAGE_LENGTH];
    /*while (read(client_socket, &message, (MAX_MESSAGE_LENGTH * sizeof(char))) != -1)
    {
        cout << "client message recieved: " << message << endl;

        //TODO: do we quit if message was sent with error?
        if (write(client_socket, &message, MAX_MESSAGE_LENGTH * sizeof(char)) == -1) {
            cerr << "message sending error" << endl;
            return -1;
        }
        else {
            cout << "message sent successfully" << endl;
        }

        if (strcmp(message, "quit") == 0) {
            close(client_socket);
            cout << "quit message recieved, client quits" << endl;
            break;
        }
        cout.flush();
    }*/

    return 0;
}

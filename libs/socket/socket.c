//
// Created by Vitor Hugo on 02/04/2022.
//

#include "socket.h"


int create_socket_client(char *socket_path) {

    int uds = 0;
    struct sockaddr_un channel;		/* Unix Domain socket */

    if ( (uds = socket(AF_UNIX, SOCK_STREAM, 0)) < 0) {
        perror("socket error");
        exit(EXIT_FAILURE);
    }

    memset(&channel, 0, sizeof(channel));
    channel.sun_family = AF_UNIX;
    strncpy(channel.sun_path, socket_path, sizeof(channel.sun_path) - 1);

    if (connect(uds, (struct sockaddr*)&channel, sizeof(channel)) < 0) {
        perror("connect error");
        exit(EXIT_FAILURE);
    }

    return uds;
}

int create_socket_server(char *socket_path) {

    int listenfd;

    char buf[PIPE_SZ];
    // buffer for outgoing file
    struct sockaddr_un channel_srv;

    // Creating the server socket
    if ( (listenfd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
        perror("socket error");
        exit(-1);
    }

    unlink(socket_path);

    memset(&channel_srv, 0, sizeof(channel_srv));

    channel_srv.sun_family = AF_UNIX;
    strncpy(channel_srv.sun_path,
            socket_path,
            sizeof(channel_srv.sun_path) - 1);

    // Binding the server socket to its name
    if (bind(listenfd,
             (struct sockaddr*)&channel_srv,
                     sizeof(channel_srv)) == -1) {
        perror("bind error");
        exit(-1);
    }

    // Configuring the listen queue
    if (listen(listenfd, LISTENQ) == -1) {
        perror("listen error");
        exit(-1);
    }

    return listenfd;
}
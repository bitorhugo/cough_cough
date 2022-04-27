//
// Created by Vitor Hugo on 02/04/2022.
//

#include "socket.h"


int create_socket_client(char *socket_path) {

    int uds = 0;
    struct sockaddr_un channel;		/* Unix Domain socket */

    if ((uds = socket(AF_UNIX, SOCK_STREAM, 0)) < 0) {
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

    struct sockaddr_un channel_srv;


    if ((listenfd = socket(AF_UNIX, SOCK_STREAM, 0)) < 0) {
        perror("socket error");
        exit(EXIT_FAILURE);
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
        exit(EXIT_FAILURE);
    }

    // Configuring the listen queue
    if (listen(listenfd, LISTENQ) == -1) {
        perror("listen error");
        exit(EXIT_FAILURE);
    }

    return listenfd;
}

void from_socket_to_file (int listen_fd, int fd_out) {
    int connfd = 0;
    size_t bytes_read = 0;
    char buffer[PIPE_SZ];

    while (1) {

        if ((connfd = accept(listen_fd, NULL, NULL)) < 0) {
            perror("accept:");
        }

        while ((bytes_read = readn(connfd, buffer, sizeof(buffer))) > 0) {
            writen(fd_out, buffer, bytes_read);
        }

    }

}
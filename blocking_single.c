#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>

#include <unistd.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <arpa/inet.h>

#include "config.h"
#include "http_handler.h"
#include "logging.h"

int main(int argc, const char *argv[])
{
    int sock_listen;
    int rc = EXIT_SUCCESS;
    struct sockaddr_in address, peer_address;
    socklen_t peer_address_len;

    memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_port = htons(PORT);
    address.sin_addr.s_addr = htonl(INADDR_ANY);

    sock_listen = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_listen < 0) {
        perror("socket");
        rc = EXIT_FAILURE;
        goto exit_rc;
    }

    int yes = 1;
    if (setsockopt(sock_listen, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
        perror("setsockopt");
        rc = EXIT_FAILURE;
        goto exit_socket;
    }


    if (bind(sock_listen, (struct sockaddr *)&address, sizeof(address))) {
        perror("bind");
        rc = EXIT_FAILURE;
        goto exit_socket;
    }

    if (listen(sock_listen, BACKLOG)) {
        perror("listen");
        rc = EXIT_FAILURE;
        goto exit_socket;
    }

    while (1) {
        int sock_client;
        sock_client = accept(sock_listen, (struct sockaddr *)&peer_address, &peer_address_len);

        debug("Client accept\n");

        http_handler_loop(sock_client);

        close(sock_client);
    }

exit_socket:
    close(sock_listen);
exit_rc:
    exit(rc);
}

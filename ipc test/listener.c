#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char** argv)
{
    int sock, msgsock, rval;
    struct sockaddr_un server;
    char buf[1024];

    if (argc < 2) {
        printf("usage:%s <pathname>", argv[0]);
        exit(1);
    }

    sock = socket(PF_LOCAL, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("opening stream socket");
        exit(1);
    }
    server.sun_family = PF_LOCAL;
    strcpy(server.sun_path, argv[1]);

    if (connect(sock, (struct sockaddr *) &server, sizeof(struct sockaddr_un)) < 0) {
        close(sock);
        perror("connecting stream socket");
        exit(1);
    }


    
do {
            bzero(buf, sizeof(buf));
            if ((rval = read(sock, buf, 1024)) < 0)
                perror("reading stream message");
            else if (rval == 0)
                printf("Ending connection\n");
            else
                printf("%s", buf);
                fflush(stdout);
        } while (rval > 0);
        close(sock);
    
}

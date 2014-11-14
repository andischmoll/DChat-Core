
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>


int main()
{
  int msgsock, sock;
  struct sockaddr_un server;

  unlink("dchat");

  sock = socket(PF_LOCAL, SOCK_STREAM, 0);
  if (sock < 0){
    perror("opening stream socket");
    exit(1);
  }
  server.sun_family = PF_LOCAL;
  strcpy(server.sun_path, "dchat");
  if (bind(sock, (struct sockaddr *) &server, sizeof(struct sockaddr_un))) {
    perror("binding stream socket");
    exit(1);
  }
  printf("Socket has name '%s'\n", server.sun_path);

  printf("Connected to socket %d", sock);
  fflush(stdout);

  listen(sock, 5);
  msgsock = accept(sock, 0, 0);
  printf("Connected to socket %d", msgsock);
  fflush(stdout);
} 
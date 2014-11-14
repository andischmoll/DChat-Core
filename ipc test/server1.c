#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define NAME "dchat"

int main()
{
  int sock, msgsock, rval;
  struct sockaddr_un server;
  char buf[1024];

  unlink(NAME);

  sock = socket(AF_UNIX, SOCK_STREAM, 0);
  if (sock < 0){
    perror("opening stream socket");
    exit(1);
  }
  server.sun_family = AF_UNIX;
  strcpy(server.sun_path, NAME);
  if (bind(sock, (struct sockaddr *) &server, sizeof(struct sockaddr_un))) {
    perror("binding stream socket");
    exit(1);
  }
  printf("Socket has name %s\n", server.sun_path);

  printf("Connected to socket %d", sock);
  fflush(stdout);

  listen(sock, 5);
  msgsock = accept(sock, 0, 0);
  printf("Connected to socket %d", msgsock);
  fflush(stdout);



  while(1){
    if (write(msgsock, "DATA", sizeof("DATA")) < 0)
        perror("writing on stream socket");
    sleep(3);
  }



  listen(sock, 5);
  for (;;) {
    msgsock = accept(sock, 0, 0);
    if (msgsock == -1)
      perror("accept");
    else do {
      bzero(buf, sizeof(buf));
      if ((rval = read(msgsock, buf, 1024)) < 0)
        perror("reading stream message");
      else if (rval == 0)
        printf("Ending connection\n");
      else
        printf("-->%s\n", buf);
    } while (rval > 0);
    close(msgsock);
  }
  close(sock);
  unlink(NAME);
} 
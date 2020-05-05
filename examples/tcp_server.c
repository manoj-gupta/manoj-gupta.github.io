#include <stdio.h>
#include <sys/socket.h>
#include <netinet/ip.h> // superset of netinet/in.h
#include <string.h>
#include <time.h>
#include <unistd.h>

#define MAXBUFSIZE 1024

int main(int argc, int *argv[])
{
    int listenfd = 0, connfd = 0;
    struct sockaddr_in saddr;
    char buff[MAXBUFSIZE+1]; // extra 1 byte for new line
    time_t ticks; // sends the time

    // open tcp stream socket
    listenfd = socket(AF_INET, SOCK_STREAM, 0);
        // AF_INET - Address family (IPv4 Internet protocols)
        // SOCK_STREAM - TCP socket
        // protocol - 0 means use default protocol for the address family

    memset(&saddr, 0, sizeof(saddr));
    memset(buff, 0, sizeof(buff));

    saddr.sin_family = AF_INET;                // address family: AF_INET
    saddr.sin_port = htons(5000);              // port in network byte order
    saddr.sin_addr.s_addr = htonl(INADDR_ANY);   // IP interface address in network byte order
        // INADDR_ANY - socket will be bound to all local interfaces.

    // bind the socket to a local interface address
    bind(listenfd, (struct sockaddr *)&saddr, sizeof(saddr));

    // mark the socket referred to by listenfd as a passive socket
    // i.e. as a socket that will be used to accept incoming connection requests using accept(2).
    listen(listenfd, 10);
        // Second argument (backlog) specifies maximum length 
        // to which the queue of pending connections for sockfd may grow.

    while (1) {
        // accept() system call is used with connection-based socket types
        //   It extracts the first connection request on the queue of pending
        //   connections for the listening socket, sockfd,
        //   creates a new connected socket, and
        //   returns a new file descriptor referring to that socket.
        connfd = accept(listenfd, (struct sockaddr *) NULL, NULL);
            // arguments:
            // listenfd: socket created with socket
            // addr: pointer to a sockaddr structure to be filled in with the
            //       address of the peer socket. NULL - nothing is filled in
            // addrlen: caller must initialize it to contain the size (in bytes)
            //       of the structure pointed to by addr. NULL if addr is NULL
            //

        printf("Got connection!\n");

        ticks = time(NULL);
        snprintf(buff, sizeof(buff), "%.24s\r\n", ctime(&ticks));

        // write() writes up to count bytes from the buffer starting at buf to
        //        the file referred to by the file descriptor fd.
        write(connfd, buff, strlen(buff));

        // close() deallocate the file descriptor indicated by connfd.
        close(connfd);

        sleep(1);
    }
    return 0;
}

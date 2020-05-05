#include <stdio.h>
#include <sys/socket.h>
#include <netinet/ip.h> // superset of netinet/in.h
#include <arpa/inet.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    int sockfd = 0, n = 0, ret = 0;
    char rxbuff[1024];
    struct sockaddr_in saddr;

    if (argc != 2) {
        printf("\nUsage: %s <server ip address>\n", argv[0]);
        return 1;
    }

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        printf("\nError: Could not create socket\n");
        return 1;
    }

    memset(rxbuff, 0, sizeof(rxbuff));
    memset(&saddr, 0, sizeof(saddr));

    saddr.sin_family = AF_INET;                // address family: AF_INET
    saddr.sin_port = htons(5000);              // port in network byte order
    // inet_pton: convert IPv4 and IPv6 addresses from text to binary form
    if (inet_pton(AF_INET, argv[1], &saddr.sin_addr) <= 0) {
        printf("\nError: inet_pton failed\n");
        return 1;
    }

    if ((ret = connect(sockfd, (struct sockaddr *)&saddr, sizeof(saddr))) < 0) {
        printf("\nError: connect failed ret:0x%x\n", ret);
        return 1;
    }

    while ((n = read(sockfd, rxbuff, sizeof(rxbuff) - 1)) > 0) {
        rxbuff[n] = 0;
        if (fputs(rxbuff, stdout) == EOF) {
            printf("\nError: fputs error\n");
        }
    }

    if (n < 0) {
        printf("\nRead error\n");
    }

    return 0;
}

#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include "../lib/communication.h"

const int SIZE_DOUBLE = 8;
const int FLAGS = 0;

void setup_socket(struct sockaddr_in *client, char *ip_daddr){
    client->sin_family = AF_INET;
    inet_pton(AF_INET, ip_daddr, &(client->sin_addr));
    memset(client->sin_zero,0x0,SIZE_DOUBLE);
}

int recv_pack(int sock, char *buf, int packet_size){
    int fromlen = sizeof(from), lenpacket = 0;
    int rv = select(sock+1, &readfds, NULL, NULL, NULL);

    if(rv == -1){
        printf("\nselect: %s\n", strerror(errno));
        return rv;
    }
    else if(rv == 0){
        printf("recv_pack: Timeout occurred! No data after 10.5 seconds.\n");
        return rv;
    }
    else{
        if(FD_ISSET(sock, &readfds)){
            lenpacket = (int) recvfrom(sock, buf, (size_t) packet_size, FLAGS, (struct sockaddr *) &from, (socklen_t *) &fromlen);
            if(lenpacket < 0){
                printf("recv failed: %s", strerror(errno));
            }
            return lenpacket;
        }
    }
}

int send_pack(int sock, char *buf, int packet_size, struct sockaddr_in destination){
    int nsend;
    int rv = select(sock + 1, NULL, &writefds, NULL, NULL);

    if(rv == -1){
        printf("\nselect: %s\n", strerror(errno));
        return rv;
    }
    else if (rv == 0){
        printf("send_pack: Timeout occurred!  No data after 10.5 seconds.\n");
        return rv;
    }
    else {
        if(FD_ISSET(sock, &writefds)){
            if ((nsend = sendto(sock, buf, (size_t) packet_size, FLAGS, (struct sockaddr *) &destination, sizeof(destination)) < 0)) {
                printf("\n send failed %s\n", strerror(errno));
            }
            return nsend;
        }
    }
}

int create_socket(){
    int on = 1;
    int sock = socket(AF_INET,SOCK_RAW, IPPROTO_ICMP);
    if(sock < 0){
        printf("\nsocket: %s\n", strerror(errno));
        exit(1);
    }
    if (setsockopt (sock, IPPROTO_IP, IP_HDRINCL, (const char*)&on,sizeof (on)) == -1) {
        printf("\nsetsockopt: %s\n", strerror(errno));
        exit(0);
    }
    return sock;
}
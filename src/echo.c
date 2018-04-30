#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <errno.h>
#include <string.h>
#include <ifaddrs.h>
#include "../lib/communication.h"
#include "../lib/protocol.h"


int main(int argc, char **argv){

    int nsend = 0, nreceived = 0;
    int len_received;
    int sock_read = 0, sock_write = 0;
    int packet_size;
    int received_data_size;
    char *ip_saddr;
    char *ip_daddr = argv[1];
    ip_saddr = get_ip_interface();
    sock_read = create_socket();
    sock_write = create_socket();
    setup_socket(&remote, ip_daddr);

    FD_ZERO(&readfds);
    FD_ZERO(&writefds);

    FD_SET(sock_read, &readfds);
    FD_SET(sock_write, &writefds);

    packet_size = sizeof (header_ip) + sizeof (header_icmp) + PAYLOAD_SIZE;
    char *send_packet = (char *) malloc((size_t) packet_size);
    header_ip *ip_hdr = (header_ip *)send_packet;
    header_icmp *icmp_hdr = (header_icmp *)(send_packet + sizeof (header_ip));

    char *recv_packet = (char *) malloc((size_t) packet_size);

    setup_ipv4hdr(ip_hdr, (unsigned short) packet_size, ip_saddr, ip_daddr);

    while (TRUE){
        setup_icmphdr(icmp_hdr, ECHO_REQUEST, (unsigned short) nsend, ID);
        icmp_hdr->checksum = 0;
        icmp_hdr->checksum = checksum((unsigned short *)icmp_hdr, sizeof(header_icmp) + PAYLOAD_SIZE);
        if(send_pack(sock_write, send_packet, packet_size, remote) < 0 ){
            printf("Error send_pack: %s",strerror(errno));
        }
            
        memset(recv_packet,0,packet_size);
        len_received = recv_pack(sock_read,recv_packet,packet_size);
        if(len_received < 0 ){
            printf("Error recv_pack: %s",strerror(errno));
        }
        received_data_size = len_received - sizeof(header_ip)- sizeof(header_icmp);
        header_ip *ip_rec = (header_ip *)recv_packet;
        header_icmp *icmp_rec = (header_icmp *)(recv_packet + sizeof (header_ip));
        if(icmp_rec->type == ECHO_REPLY){
            unpack(recv_packet, from, len_received);
        }
        else if(icmp_rec->type == ECHO_REQUEST){
            ip_rec->source_address.s_addr = inet_addr(get_ip_interface());
            ip_rec->destination_address = from.sin_addr;
            icmp_rec->type = ECHO_REPLY;
            icmp_rec->checksum = 0;
            icmp_rec->checksum = checksum((unsigned short *)icmp_rec, sizeof(header_icmp) + received_data_size);
            nsend = send_pack(sock_write, recv_packet, len_received, from);
        }
        ++nsend;
        sleep(1);
    }

    close(sock_read);
    close(sock_write);
    printf("\n Ping Encerrado \n");
    return 0;
}
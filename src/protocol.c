#include <arpa/inet.h>
#include <ifaddrs.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/time.h>
#include "../lib/protocol.h"

const int TRUE = 1;
const int ECHO_REQUEST = 8;
const int ECHO_REPLY = 0;
const int ID = 4545;
const int VERSION = 4;
const int IHL = 5;
const int PROTOCOL = 1;
const int TTL = 225;
const int SERVICE = 0;
const int OFFSET = 0;
const int TOTAL_SIZE_HEADERS = 28;
const int MICROSECONDS = 1000000;
const int THOUSAND = 1000;
const int PAYLOAD_SIZE = 50;
const int SIZE_BUFFER = 4096;
const int FLAG = 0;

short checksum(unsigned short *addr, int count){
    /* Compute Internet Checksum for "count" bytes
    *         beginning at location "addr".
    */
    long sum = 0;
    short checksum;

    while(count > 1) {
        /*  This is the inner loop */
        sum += *addr++;
        count -= 2;
    }
    /*  Add left-over byte, if any */
    if(count > 0) {
        sum += *(unsigned char *) addr;
    }
    /*  Fold 32-bit sum to 16 bits */
    while (sum>>16) {
        sum = (sum & 0xffff) + (sum >> 16);
    }
    checksum = (short) ~sum;

    return checksum;
}

void setup_ipv4hdr(header_ip *ipv4_hdr, unsigned short len, char *ip_saddr, char *ip_daddr){
    len = (unsigned short) htons((uint16_t) len);
    ipv4_hdr->version = VERSION;
    ipv4_hdr->ihl = IHL;
    ipv4_hdr->type_service = SERVICE;
    ipv4_hdr->total_lenght = len;
    ipv4_hdr->id = (unsigned short) rand();
    ipv4_hdr->frag_offset = OFFSET;
    ipv4_hdr->flag = FLAG;
    ipv4_hdr->time_live = TTL;
    ipv4_hdr->protocol = PROTOCOL;
    ipv4_hdr->source_address.s_addr = inet_addr(ip_saddr);
    ipv4_hdr->destination_address.s_addr = inet_addr(ip_daddr);
}

void setup_icmphdr(header_icmp *icmp_hdr, unsigned char type, unsigned short sequence, unsigned short identifier){
    struct timeval *tval;
    icmp_hdr->type = type;
    icmp_hdr->code = 0;
    icmp_hdr->checksum = 0;
    icmp_hdr->hd_idseq.sequence = sequence;
    icmp_hdr->hd_idseq.identifier = identifier;
    tval = (struct timeval*)icmp_hdr->data;
    gettimeofday(tval, NULL);
}

int unpack(char *recvpacket, struct sockaddr_in from, int len){
    header_ip *ip_hdr;
    header_icmp *icmp_hdr;
    ip_hdr = (header_ip *) recvpacket;
    icmp_hdr = (header_icmp *) (recvpacket + sizeof(header_ip));
    struct timeval tvrecv;
    struct timeval *tvsend;
    double time;
    gettimeofday(&tvrecv, NULL);

    if (len < TOTAL_SIZE_HEADERS){
        printf("packets\'s length is less than header ip + header icmp\n");
        return -1;
    }
    if(icmp_hdr->type == ECHO_REPLY && icmp_hdr->hd_idseq.identifier == ID){
        tvsend = (struct timeval*)icmp_hdr->data;
        time = timeval_subtract(tvsend,&tvrecv);
        printf("%d byte from %s: icmp_seq=%u ttl=%d rtt=%.3lf ms\n",
               len, inet_ntoa(from.sin_addr), icmp_hdr->hd_idseq.sequence, ip_hdr->time_live, time);
        return 1;
    }
    else
        return -1;
}

double timeval_subtract(struct timeval *out, struct timeval *in){
    double result;
    if(out->tv_sec < in->tv_sec){
        result = MICROSECONDS - out->tv_usec;
        result += in->tv_usec;
        return result/THOUSAND;
    }
    else if(out->tv_sec == in->tv_sec){
        result = in->tv_usec - out->tv_usec;
        return result/THOUSAND;
    }
    return -1;
}

char *get_ip_interface(){
    char *ip_saddr;
    struct ifaddrs *addrs = NULL;
    getifaddrs(&addrs);
    struct ifaddrs *tmp = addrs;

    while (tmp){
        if (tmp->ifa_addr && tmp->ifa_addr->sa_family == AF_INET) {
            struct sockaddr_in *pAddr = (struct sockaddr_in *)tmp->ifa_addr;
            ip_saddr = inet_ntoa(pAddr->sin_addr);
        }
        tmp = tmp->ifa_next;
    }
    freeifaddrs(addrs);
    return ip_saddr;
}

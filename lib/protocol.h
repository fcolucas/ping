#ifndef PING_PROTOCOL_H
#define PING_PROTOCOL_H

#include <netinet/in.h>

#define LENGTH_IP 4
#define LENGTH_IHL 4
#define LENGTH_FLAG 3
#define LENGTH_FRAG_OFFSET 13

const int TOTAL_SIZE_HEADERS;
const int MICROSECONDS;
const int THOUSAND;
const int PAYLOAD_SIZE;
const int SIZE_BUFFER;
const int FLAG;
const int ECHO_REQUEST;
const int ECHO_REPLY;
const int ID;
const int VERSION;
const int IHL;
const int PROTOCOL;
const int TTL;
const int SERVICE;
const int OFFSET;
const int TRUE;

unsigned long daddr;
unsigned long saddr;

typedef struct header_ip{
#if BYTE_ORDER == LITTLE_ENDIAN
    unsigned char	ihl:LENGTH_IHL,
            version:LENGTH_IP;
#endif
#if BYTE_ORDER == BIG_ENDIAN
    unsigned char	version: LENGTH_IP,
            ihl:LENGTH_IHL;
#endif
    unsigned char type_service;
    unsigned short total_lenght;
    unsigned  short id;
    unsigned short flag: LENGTH_FLAG,
            frag_offset: LENGTH_FRAG_OFFSET;
    unsigned char time_live;
    unsigned char protocol;
    unsigned short checksum;
    struct in_addr source_address;
    struct in_addr destination_address;
} header_ip;

typedef struct header_icmp{
    unsigned char type;
    unsigned char code;
    unsigned short checksum;
    struct hd_idseq {
        unsigned short identifier;
        unsigned short sequence;
    } hd_idseq;
    struct timeval data[1];
} header_icmp;

/*The following function receives the memory address of the header structure (param addr) and its size summed 
 * with the message size (param count) and calculates the 16-bit complement of the sum of the
 * complement of all 16-bit words in the header. For the purposes of calculating the checksum, the
 * value of the checksum field is zero. */
short checksum(unsigned short *addr, int count);

/* The setup_ipv4hdr function receives a pointer to a header_ip (header_ip * ipv4_hdr) structure. 
*The total size of the package, including headers and data (unsigned short len). 
*A pointer to the source ip address (char * ip_saddr) and another to the destination address (char * ip_daddr). 
*This function will populate the fields of the ip header to define the destination and size of the data sent.*/
void setup_ipv4hdr(header_ip *ipv4_hdr, unsigned short len, char *ip_saddr, char *ip_daddr);

/*The setup_icmphdr function receives a pointer to a header_icmp (header_icmp * icmp_hdr) structure. 
*The header type (unsigned character type). The sequence (unsigned short sequence) and an identifier (unsigned short identifier).
*This function defines the proper echo type of the icmp header, as well as an identifier for its tracking*/
void setup_icmphdr(header_icmp *icmp_hdr, unsigned char type, unsigned short sequence, unsigned short identifier);

/*The unpack function receives a pointer to an array that contains data from the icmp (char * recvpacket) package, 
*receives a struct sockaddr_in that contains the data from whom it sent (struct sockaddr_in from) and the size of the 
*received packet. Returns -1 if the packet is not a response and / or if it does not have the correct identifier, 
*and 1 if the operation succeeds.*/
int unpack(char *recv_packet, struct sockaddr_in from, int len);

/*The timeval_subtract function receives a pointer to a system output struct (struct timeval * out) 
*and a pointer to a struct timeval * in. The function returns the time difference between the 
*output and the arrival in milliseconds*/
double timeval_subtract(struct timeval *out, struct timeval *in);

/*The git_ip_interface function returns a * char pointer 
*to the IP address of the machine's network interface.*/
char *get_ip_interface();

#endif //PING_PROTOCOL_H

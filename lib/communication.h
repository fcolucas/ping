#ifndef PING_COMMUNICATION_H
#define PING_COMMUNICATION_H

const int SIZE_DOUBLE;
const int FLAGS;

struct sockaddr_in remote;
struct sockaddr_in from;
fd_set readfds;
fd_set writefds;

/*The setup_socket function receives a pointer to a struct sockaddr_in 
*that will be configured (struct sockaddr_in * client) and a pointer to 
*the address to which the socket will connect (char * ip_daddr).*/
void setup_socket(struct sockaddr_in *client,char *ip_daddr);

/*The function has as parameters a socket descriptor (param sock),
 * a buffer (param *buf) and the size of the receive packet (param
 * packet_size), receives a message buffer and returns its size (len_packet).
 * */
int recv_pack(int sock, char *buf, int packet_size);

/*The function has as parameters a socket descriptor (param sock), a buffer (param *
 * buf), the size of the packet (param packet_size) and the structure for who will be
 * sent (param destination), sends a message buffer and returns successful if buffer was sent.
 * */
int send_pack(int sock, char *buf, int packet_size, struct sockaddr_in destination);

/*The following function configures a raw socket for communication.
 *
 * Raw socket - if you use IPPROTO_ICMP, then kernel
 * will fill in the correct ICMP header checksum, if IPPROTO_RAW, then it wont.
 *
 * setsockopt - We shall provide IP headers.
 * */
int create_socket();

#endif //PING_COMMUNICATION_H

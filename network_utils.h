/*header file  for netwrk_utils*/
#ifndef NETWORK_UTILS_H
#define NETWORK_UTILS_H

#include <stdint.h>
#include "arp_packet.h"
#include <net/if_arp.h>
#include <netinet/if_ether.h> //includes this for struct ether_arp


/*struct ether_arp{
	struct arphdr ea_hdr; //ARP header
	uint8_t arp_sha[6];   //sender hardware address (MAC)
	uint8_t arp_spa[4];   //sender protocol address (IP)
	uint8_t arp_tha[6];   //Target Hardware address (MAC)
	uint8_t arp_tpa[4];   //Target protocol address (IP) 
};*/

//Utility function prototypes for networking
void print_mac_address(const uint8_t *mac);
void print_ip_address(const uint8_t *ip);

int get_mac_address(const char *interface, uint8_t *mac);
int get_ip_address(const char *interface, uint8_t *ip);

ARPPacket *create_arp_request(const uint8_t *src_mac,
			     const uint8_t *src_ip,
			     const uint8_t *target_ip);

int send_arp_request(int sockfd, const ARPPacket *packet, 
		     		 const char *interface);

int receive_arp_request(int sockfd, uint8_t *sender_mac, 
			uint8_t *sender_ip, const char *interface);

int receive_arp_response(int sockfd, uint8_t *sender_mac);

#endif //NETWORK_UTILS_H

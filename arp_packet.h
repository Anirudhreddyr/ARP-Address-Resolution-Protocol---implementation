/* arp_pacet.h */

#ifndef ARP_PACKET_H
#define ARP_PACKET_H

#include <stdint.h>

typedef struct {
	uint16_t hardware_type; //Ethernet = 1
	uint16_t protocol_type;	//IPv4 = 0X0800
	uint8_t hardware_size;	//MAC address size = 6
	uint8_t protocol_size;	//IPv4 address size = 4
	uint16_t opcode;	//Request = 1 Replay = 2	
	uint8_t sender_mac[6];
	uint8_t sender_ip[4];
	uint8_t target_mac[6];
	uint8_t target_ip[4];
} ARPPacket;

ARPPacket *create_arp_request(const uint8_t *src_mac, const uint8_t *src_ip, const uint8_t *target_ip); 

//Functions prototypes
/*void construct_arp_request(ARPPacket *packet, const uint8_t *src_mac,
					      const uint8_t *src_ip,
			                      const uint8_t *dest_ip);
*/

void send_arp_packet(const ARPPacket *packet, int sockfd);

#endif // ARP_PACKET_H

/*  Implement ARP related logic (eg., ARP request creation)*/
#include "arp_packet.h"
#include <string.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>


ARPPacket *create_arp_request(const uint8_t *src_mac, const uint8_t *src_ip, const uint8_t *target_ip){

	ARPPacket *packet = (ARPPacket *) malloc(sizeof(ARPPacket));
	if (!packet) {
		perror("Memory allocation failed");
		return NULL;
	}
	
	packet->hardware_type = htons(1); // Ethernet
	packet->protocol_type = htons(0X0800); // IPv4
	packet->hardware_size = 6;
	packet->protocol_size = 4;
	packet->opcode = htons(1); //Request
	memcpy(packet->sender_mac, src_mac, 6);
	memcpy(packet->sender_ip, src_ip, 4);
	memset(packet->target_mac, 0, 6);	//Unknown MAC
	memcpy(packet->target_ip, target_ip, 4);

	return packet;
}

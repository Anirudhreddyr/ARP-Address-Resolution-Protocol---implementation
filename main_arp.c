#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/if_ether.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <stdint.h>
#include "arp_packet.h"
#include "network_utils.h"


#define INTERFACE "eth0" 

int main(){
	int sockfd;
	uint8_t src_mac[6];
	uint8_t src_ip[4];
	uint8_t target_ip[4] = {192,168,222,1}; //Replace with our targeted IP

	//step1 : creating raw socket
	sockfd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ARP));
	if(sockfd < 0){
		perror("Socket creation failed");
		close(sockfd);
		return EXIT_FAILURE;
	}

	//step2 : Getting source MAC and IP address
	if(get_mac_address(INTERFACE, src_mac) < 0) {
		perror("Failed get mac address");
		close(sockfd);
		return EXIT_FAILURE;
	}

	//IP address
	if(get_ip_address(INTERFACE, src_ip) < 0 ) {
		perror("Failed get IP address");
		close(sockfd);
		return EXIT_FAILURE;
	}

	//step3 : Construct the ARP request packet 
	ARPPacket *packet = create_arp_request(src_mac, src_ip, target_ip);
	if (!packet){
		fprintf(stderr, "Failed to create ARP request\n");
		close(sockfd);
		return EXIT_FAILURE;
	}
	

	//step4 : send the arp request
	if(send_arp_request(sockfd, packet, INTERFACE) < 0) {
		perror("Failed to send ARP Request");
		close(sockfd);
		return EXIT_FAILURE;
	}

	printf("ARP request sent successfully ");

	//step5 : Recive arp replay
	uint8_t sender_mac[6];
	uint8_t sender_ip[4];
	if(receive_arp_request(sockfd, sender_mac, sender_ip, INTERFACE) < 0) {
		perror("Failed to recive ARP Replay");
		close(sockfd);
		return EXIT_FAILURE;
	
	// Print rececived ARP relpy information
	printf("Received ARP Relpy from MAC : %02X:%02X:%02X:%02X:%02X:%02X\n",
		sender_mac[0], sender_mac[1], sender_mac[2],
		sender_mac[3], sender_mac[4], sender_mac[5]);
	printf("Sender IP: %d.%d.%d.%d\n", sender_ip[0],
		sender_ip[1], sender_ip[2], sender_ip[3]);

	close(sockfd);
	return EXIT_SUCCESS;

	}
}

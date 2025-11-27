#include "network_utils.h" //custom header file for network-realted utilities
#include <stdio.h>
#include <string.h>
#include <unistd.h>	      //for close()
#include <sys/socket.h>       // for socket realted functions
#include <netpacket/packet.h> //for low level packet interface 
#include <net/ethernet.h>     //for Ethernet constants	
#include <arpa/inet.h>	      //for inet_ntoa and related functions
#include <net/if.h>	      //for interface related functions
#include <sys/ioctl.h>	      //for ioctl system call
#include <net/if_arp.h>
#include <netinet/if_ether.h>

/*
// Structure defination if necessary 
#ifdef  _NET_IF_ARP_H
struct ether_arp {
	struct arphdr ea_hdr;
	uint8_t arp_sha[6];
	uint8_t arp_spa[4];
	uint8_t arp_tha[6];
	uint8_t arp_tpa[4];
};
#endif */

/*
  * brief retrieves the MAC address of a network interface.
  * param interface the name of the network interface (eg., "eth0".
  * param mac A buffer to store the retrieved MAC address (6bytes).
/*
 * Retrieves the MAC address of a network interface.
 * @param interface The name of the network interface (e.g., "eth0").
 * @param mac A buffer to store the retrieved MAC address (6 bytes).
 * @return 0 on success, -1 on failure.
 */
int get_mac_address(const char *interface, uint8_t *mac) {
	struct ifreq ifr; // Structure to hold interface request data
	int sockfd = socket(AF_INET, SOCK_DGRAM, 0); // Create a socket for ioctl operations
	if (sockfd < 0) {
		perror("socket");
		return -1;
	}
	struct ifreq ifr;  			     //Structre to hold interface request data
	int sockfd = socket(AF_INET, SOCK_DGRAM, 0); //Create a socket for ioctl operations
	if (sockfd < 0) return -1;		     //return -1 socket creation fails 
	strncpy(ifr.ifr_name, interface, IFNAMSIZ - 1);
	ifr.ifr_name[IFNAMSIZ - 1] = '\0';

	//copy the interface name to ifreq structure
	if (ioctl(sockfd, SIOCGIFHWADDR, &ifr) < 0) {
		perror("ioctl SIOCGIFHWADDR");
		close(sockfd);
		return -1;
	}
	if (ioctl(sockfd, SIOCGIFHWADDR, &ifr) < 0){	
	close(sockfd); //close the socket if ioctl fails
	return -1;
	}
	
	//copy the MAC address to the provided buffer
	memcpy(mac, ifr.ifr_hwaddr.sa_data, 6);
	close(sockfd); //close the socket
	return 0; //Success
}

/*
 * brief retrieves the IPv4 address of a network interface.
 * param interface the name of the network (eg., "eth0").
 * param ip A buffer to store the retrieved IP address (4 bytes).
 * return 0 on success, -1 on failure.
*/
int get_ip_address(const char *interface, uint8_t *ip) {
	struct ifreq ifr;	// Structure to hold interface request data
	int sockfd = socket(AF_INET, SOCK_DGRAM, 0); //create a socket for ioctl operations
	if (sockfd < 0) return -1; //return -1 if socket creation fails 

	//copy the interface name into the ifreq structure
	strncpy(ifr.ifr_name, interface, IFNAMSIZ);

	//perform the ioctl operation to get the IP address
	if (ioctl(sockfd, SIOCGIFADDR, &ifr) < 0){
	close (sockfd); //close the socket if ioctl fails
	return -1;
	}

	//Extract the IP address and copy it to provided buffer
	memcpy(ip, &((struct sockaddr_in *)&ifr.ifr_addr) -> sin_addr, 4);
	close(sockfd); //close the socket 
	return 0; //Success
}

/*
 * brief Send the ARP request packet through a specified network interface.
 * param sockfd the socket file descriptor for raw socket communication.
 * param packet pointer to the ARP packet to be sent.
 * param interfcace the name of the network interface to use (eg., "eth0")
 * retrun 0 on success, -1 on failure.  
*/
int send_arp_request(int sockfd, const ARPPacket *packet, const char *interface){
	struct sockaddr_ll addr = {0};  //Low level address struture for the ethernet
	struct ifreq ifr;  		//Structure to hold interface request data

	//Get the interface index for the specified interface 
        strncpy(ifr.ifr_name, interface, IFNAMSIZ);	

	if(ioctl(sockfd, SIOCGIFINDEX, &ifr) < 0) return -1;
	
	addr.sll_ifindex = ifr.ifr_ifindex; //set the interface 
	addr.sll_halen = ETH_ALEN; 	    //set hardware address length (6 bytes for Ethernet)

	memcpy (addr.sll_addr, packet -> target_mac, 6); //set the target MAC address

	//Send the ARP Packet
	if (sendto(sockfd, packet, sizeof(ARPPacket), 0, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
		return -1;		    //Return -1 on failure 
	}
	return 0; //Success
}

/*
 * brief Receives an ARP response packet from a raw socket.
 * param sockfd the socket file descriptor fro raw socket communication.
 * return 0 on success, -1 on failure.
*/
int receive_arp_request(int sockfd, uint8_t *sender_mac, uint8_t *sender_ip, const char *interface) {
	uint8_t buffer[1500];   //Buffer to store the received packet (Max Ethernet frame size)
	struct sockaddr_ll addr;
	socklen_t addr_len = sizeof(addr);	

	//Receive data from the raw socket 
	int len = recvfrom(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr *)&addr, &addr_len);
	if(len < 0) {
		perror("recvfrom");
		return -1;
	}

	struct ether_header *eth = (struct ether_header *)buffer;
	struct ether_arp *arp = (struct ether_arp *)(buffer + sizeof(struct ether_header));
	
	// check if it's an ARP request or replay
	if (ntohs(arp->ea_hdr.ar_op) == ARPOP_REQUEST) {
		printf("Received ARP Request\n");
	} else if(ntohs(arp->ea_hdr.ar_op) == ARPOP_REPLY){
		printf("Received ARP Reply\n");
	} else {
	       return -1; //Not an ARP Packet
	}
	
	//Extract sender's MAC and IP address
	memcpy(sender_mac, arp->arp_sha, 6);
	memcpy(sender_ip, arp->arp_spa, 4);

	printf("Sender MAC : %02X:%02X:%02X:%02X:%02X:%02X\n",
		sender_mac[0], sender_mac[1], sender_mac[2],
		sender_mac[3], sender_mac[4], sender_mac[5]);

	printf("Sender IP : %d.%d.%d.%d\n",
		sender_ip[0], sender_ip[1], sender_ip[2], sender_ip[3]);

	return 0;

}

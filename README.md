# ARP Protocol Implementation

A Linux-based implementation of the Address Resolution Protocol (ARP) that enables discovering the MAC address of a host on a Local Area Network (LAN) given its IP address.

## Overview

This project demonstrates a low-level network programming implementation of the ARP protocol using raw sockets. It creates ARP request packets, sends them over a network interface, and receives ARP replies to map IP addresses to MAC addresses.

## Features

- **ARP Request Generation**: Creates properly formatted ARP request packets with Ethernet and IPv4 headers
- **MAC Address Retrieval**: Extracts the MAC address of a specified network interface
- **IP Address Retrieval**: Retrieves the IPv4 address of a network interface
- **Raw Socket Communication**: Uses AF_PACKET sockets for direct Ethernet layer communication
- **ARP Reply Handling**: Parses received ARP responses and extracts sender information

## Project Structure

```
arp_packet.h          - ARP packet data structure and function declarations
arp_packet.c          - ARP request packet creation logic
network_utils.h       - Network utility function declarations
network_utils.c       - Implementation of MAC/IP address retrieval and packet send/receive
main_arp.c            - Main application demonstrating ARP request and response handling
```

## Components

### arp_packet.h / arp_packet.c

Defines and implements ARP packet creation:

- **ARPPacket Structure**: Contains all fields required by the ARP protocol
  - Hardware type (Ethernet = 1)
  - Protocol type (IPv4 = 0x0800)
  - Hardware address size (6 bytes for MAC)
  - Protocol address size (4 bytes for IPv4)
  - Operation code (1 = Request, 2 = Reply)
  - Sender/Target MAC and IP addresses

- **create_arp_request()**: Allocates and initializes an ARP request packet with source MAC/IP and target IP

### network_utils.h / network_utils.c

Provides low-level network operations:

- **get_mac_address()**: Retrieves the MAC address of a network interface using SIOCGIFHWADDR ioctl
- **get_ip_address()**: Retrieves the IPv4 address of a network interface using SIOCGIFADDR ioctl
- **send_arp_request()**: Sends the ARP packet through a raw socket to the specified interface
- **receive_arp_request()**: Receives and parses ARP packets from the socket, extracting sender information

### main_arp.c

Main application that orchestrates the ARP request/response workflow:

1. Creates a raw socket for ARP communication
2. Retrieves local MAC and IP addresses
3. Constructs an ARP request for a target IP address
4. Sends the ARP request packet
5. Receives and displays the ARP reply with the sender's MAC and IP

## Requirements

- **Linux Operating System**: Uses Linux-specific system calls and socket APIs
- **Network Interface**: Default configured for `eth0` (modify in `main_arp.c` if needed)
- **Root Privileges**: Raw socket operations typically require elevated permissions
- **C Compiler**: GCC or Clang with standard C libraries

## Building

```bash
gcc -o arp_tool main_arp.c arp_packet.c network_utils.c
```

## Usage

1. **Modify target IP** (optional):
   - Edit `main_arp.c` line with `target_ip` to change the target address
   - Default: 192.168.222.1

2. **Run with elevated privileges**:
   ```bash
   sudo ./arp_tool
   ```

3. **Expected Output**:
   ```
   ARP request sent successfully
   Received ARP Reply
   Sender MAC : XX:XX:XX:XX:XX:XX
   Sender IP: 192.168.222.1
   ```

## Network Flow

```
┌─────────────────────────────────────────┐
│ 1. Create a raw socket (AF_PACKET)      │
├─────────────────────────────────────────┤
│ 2. Get local MAC and IP from eth0       │
├─────────────────────────────────────────┤
│ 3. Construct ARP request packet         │
├─────────────────────────────────────────┤
│ 4. Send ARP request to the target IP    │
├─────────────────────────────────────────┤
│ 5. Listen for ARP reply on the socket   │
├─────────────────────────────────────────┤
│ 6. Parse reply and extract sender info  │
└─────────────────────────────────────────┘
```

## Key Functions

| Function | Purpose |
|----------|---------|
| `create_arp_request()` | Create ARP request packet with source/target information |
| `get_mac_address()` | Retrieve MAC address of a network interface |
| `get_ip_address()` | Retrieve IP address of a network interface |
| `send_arp_request()` | Send ARP packet through raw socket |
| `receive_arp_request()` | Receive and parse ARP response |

## Technical Details

### ARP Packet Format

```
Header Fields:
┌──────────────────────────────────┐
│ Hardware Type (2 bytes)          │ = 1 (Ethernet)
├──────────────────────────────────┤
│ Protocol Type (2 bytes)          │ = 0x0800 (IPv4)
├──────────────────────────────────┤
│ Hardware Size (1 byte)           │ = 6 (MAC)
├──────────────────────────────────┤
│ Protocol Size (1 byte)           │ = 4 (IPv4)
├──────────────────────────────────┤
│ Operation (2 bytes)              │ = 1 (Request) / 2 (Reply)
├──────────────────────────────────┤
│ Sender MAC (6 bytes)             │
├──────────────────────────────────┤
│ Sender IP (4 bytes)              │
├──────────────────────────────────┤
│ Target MAC (6 bytes)             │
├──────────────────────────────────┤
│ Target IP (4 bytes)              │
└──────────────────────────────────┘
```

### Socket Operations

- **Socket Type**: `AF_PACKET, SOCK_RAW, htons(ETH_P_ARP)`
- **Interface Operations**: Uses `ioctl()` with SIOCGIFHWADDR and SIOCGIFADDR
- **Data Transmission**: Uses `sendto()` and `recvfrom()` for raw packet I/O

## Learning Objectives

This project demonstrates:

- Raw socket programming in Linux
- Low-level network packet manipulation
- System call usage (ioctl, socket, sendto, recvfrom)
- ARP protocol implementation details
- MAC and IP address handling
- Ethernet and IPv4 header structures
- Network interface configuration

## Limitations & Future Improvements

- Currently targets a single IP address per execution
- No timeout mechanism for waiting for ARP replies
- Limited error handling and validation
- Hardcoded interface name (`eth0`)
- No support for other network protocols beyond Ethernet/IPv4

### Possible Enhancements

- Add a timeout for ARP reply reception
- Support for multiple simultaneous ARP requests
- ARP cache implementation
- Support for ARP probing and gratuitous ARP
- Statistics and packet analysis
- Configurable interface and IP via command-line arguments

## Troubleshooting

| Issue | Solution |
|-------|----------|
| "Socket creation failed" | Ensure running with `sudo` |
| "Failed get mac address" | Verify interface name exists (`ifconfig`) |
| "Failed get IP address" | Ensure interface has an IP configured |
| "Failed to recive ARP Replay" | Check if target is reachable on the network |

## References

- [RFC 826 - An Ethernet Address Resolution Protocol](https://tools.ietf.org/html/rfc826)
- [Linux raw sockets documentation](https://man7.org/linux/man-pages/man7/raw.7.html)
- [Ethernet Frame Format](https://en.wikipedia.org/wiki/Ethernet_frame)

## Author

Created as a learning resource for Linux network programming and ARP protocol implementation.

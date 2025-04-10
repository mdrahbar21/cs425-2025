#include <iostream>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <cstdint> 

#define SERVER_PORT 12345
#define CLIENT_PORT 54321

// Size of the packet: IP header + TCP header
#define PACKET_SIZE (sizeof(struct iphdr) + sizeof(struct tcphdr))

// Utility function to print TCP flags (for debugging)
void print_tcp_flags(struct tcphdr *tcp)
{
    std::cout << "[+] TCP Flags:"
              << " SYN: " << tcp->syn
              << " ACK: " << tcp->ack
              << " FIN: " << tcp->fin
              << " RST: " << tcp->rst
              << " PSH: " << tcp->psh
              << " SEQ: " << ntohl(tcp->seq) << std::endl;
}

/*
 * Function: send_syn
 * ------------------
 * Constructs and sends an initial SYN packet.
 * The packet contains:
 *   - IP header with source and destination addresses set to 127.0.0.1.
 *   - TCP header with source port = CLIENT_PORT,
 *     destination port = SERVER_PORT, sequence number = 200, and SYN flag set.
 */
void send_syn(int sock, struct sockaddr_in &dest_addr)
{
    char packet[PACKET_SIZE];
    memset(packet, 0, PACKET_SIZE);

    // Set pointers for the IP and TCP headers inside the packet buffer
    struct iphdr *ip = (struct iphdr *)packet;
    struct tcphdr *tcp = (struct tcphdr *)(packet + sizeof(struct iphdr));

    // Fill in IP header fields
    ip->ihl = 5;
    ip->version = 4;
    ip->tos = 0;
    ip->tot_len = htons(PACKET_SIZE);
    ip->id = htons(11111); // Identifier (arbitrary)
    ip->frag_off = 0;
    ip->ttl = 64;
    ip->protocol = IPPROTO_TCP;
    // For simplicity, using loopback addresses for both source and destination
    ip->saddr = inet_addr("127.0.0.1");
    ip->daddr = inet_addr("127.0.0.1");

    // Fill in TCP header fields for the SYN packet
    tcp->source = htons(CLIENT_PORT);
    tcp->dest = htons(SERVER_PORT);
    tcp->seq = htonl(200); // Initial sequence number as per assignment
    tcp->ack_seq = 0;
    tcp->doff = 5; // Data offset: size of TCP header
    tcp->syn = 1;  // SYN flag set for connection initiation
    tcp->ack = 0;
    tcp->fin = 0;
    tcp->rst = 0;
    tcp->psh = 0;
    tcp->window = htons(8192);
    tcp->check = 0; // Set to 0; kernel may compute checksum for raw packets

    // Send the SYN packet using sendto()
    if (sendto(sock, packet, PACKET_SIZE, 0,
               (struct sockaddr *)&dest_addr, sizeof(dest_addr)) < 0)
    {
        perror("sendto() failed for SYN");
        exit(EXIT_FAILURE);
    }
    else
    {
        std::cout << "[+] SENT SYN packet with SEQ=200" << std::endl;
    }
}

/*
 * Function: receive_syn_ack
 * -------------------------
 * Waits for the server's SYN-ACK response.
 * The expected SYN-ACK packet should have:
 *   - SYN flag = 1 and ACK flag = 1.
 *   - An acknowledgement number equal to 201 (200 + 1).
 *   - A sequence number of 400.
 * Returns: true if a valid SYN-ACK is received.
 */

bool receive_syn_ack(int sock, uint32_t expected_ack_num, uint32_t expected_seq_num) {
    char buffer[65536];
    struct sockaddr_in source_addr;
    socklen_t addr_len = sizeof(source_addr);

    while (true) {
        int data_size = recvfrom(sock, buffer, sizeof(buffer), 0,
                                 (struct sockaddr *)&source_addr, &addr_len);
        if (data_size < 0) {
            perror("recvfrom() failed");
            continue;
        }

        struct iphdr *ip = (struct iphdr *)buffer;
        unsigned int ip_header_len = ip->ihl * 4;
        struct tcphdr *tcp = (struct tcphdr *)(buffer + ip_header_len);

        if (ntohs(tcp->dest) != CLIENT_PORT) continue;
        if (ntohs(tcp->source) != SERVER_PORT) continue;

        if (tcp->syn == 1 && tcp->ack == 1) {
            print_tcp_flags(tcp);
            if (ntohl(tcp->ack_seq) == expected_ack_num &&
                ntohl(tcp->seq) == expected_seq_num) {
                std::cout << "[+] RECEIVED SYN-ACK (SEQ=400, ACK=201)" << std::endl;
                return true;
            }
        }
    }
    return false;
}

/*
 * Function: send_final_ack
 * ------------------------
 * Constructs and sends the final ACK packet to complete the handshake.
 * The ACK packet contains:
 *   - IP header (with loopback addresses).
 *   - TCP header with source port = CLIENT_PORT, destination port = SERVER_PORT,
 *     sequence number = 600, and the ACK flag set.
 *   - The acknowledgment number is set to the server's sequence + 1 (i.e., 401).
 */
void send_final_ack(int sock, struct sockaddr_in &dest_addr)
{
    char packet[PACKET_SIZE];
    memset(packet, 0, PACKET_SIZE);

    // Set up IP and TCP header pointers
    struct iphdr *ip = (struct iphdr *)packet;
    struct tcphdr *tcp = (struct tcphdr *)(packet + sizeof(struct iphdr));

    // Fill in IP header fields (source and destination remain the loopback address)
    ip->ihl = 5;
    ip->version = 4;
    ip->tos = 0;
    ip->tot_len = htons(PACKET_SIZE);
    ip->id = htons(22222); // Arbitrary identifier for this packet
    ip->frag_off = 0;
    ip->ttl = 64;
    ip->protocol = IPPROTO_TCP;
    ip->saddr = inet_addr("127.0.0.1");
    ip->daddr = inet_addr("127.0.0.1");

    // Fill in TCP header fields for the ACK packet
    tcp->source = htons(CLIENT_PORT);
    tcp->dest = htons(SERVER_PORT);
    tcp->seq = htonl(600);     // Final ACK packet sequence number as per assignment
    tcp->ack_seq = htonl(401); // Acknowledging server's sequence (400 + 1)
    tcp->doff = 5;
    tcp->ack = 1; // ACK flag set
    tcp->syn = 0;
    tcp->fin = 0;
    tcp->rst = 0;
    tcp->psh = 0;
    tcp->window = htons(8192);
    tcp->check = 0; 

    // Send the final ACK packet
    if (sendto(sock, packet, PACKET_SIZE, 0,
               (struct sockaddr *)&dest_addr, sizeof(dest_addr)) < 0)
    {
        perror("sendto() failed for ACK");
        exit(EXIT_FAILURE);
    }
    else
    {
        std::cout << "[+] SENT final ACK packet with SEQ=600" << std::endl;
    }
}

int main()
{
    std::cout << "[+] Client starting TCP handshake..." << std::endl;

    // Create a raw socket with IPPROTO_TCP; this requires root privileges.
    int sock = socket(AF_INET, SOCK_RAW, IPPROTO_TCP);
    if (sock < 0)
    {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Enable IP header inclusion: tell the kernel that headers are provided in the packet
    int one = 1;
    if (setsockopt(sock, IPPROTO_IP, IP_HDRINCL, &one, sizeof(one)) < 0)
    {
        perror("setsockopt() failed");
        exit(EXIT_FAILURE);
    }

    // Set up destination address (loopback)
    struct sockaddr_in dest_addr;
    memset(&dest_addr, 0, sizeof(dest_addr));
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(SERVER_PORT);
    dest_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    // Step 1: Send SYN Packet 
    send_syn(sock, dest_addr);

    // Step 2: Wait and process SYN-ACK from server
    // Expected: ACK number 201 (for our SYN with 200) and SEQ number 400.
    if (!receive_syn_ack(sock, 201u, 400u)) {
        std::cerr << "[-] Failed to receive correct SYN-ACK" << std::endl;
        close(sock);
        exit(EXIT_FAILURE);
    }

    // Step 3: Send final ACK
    send_final_ack(sock, dest_addr);

    std::cout << "[+] Handshake complete." << std::endl;
    close(sock);
    return 0;
}

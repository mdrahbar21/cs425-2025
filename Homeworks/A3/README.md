# TCP Handshake Client
This project implements the client side of a simplified TCP three-way handshake using raw sockets in C++.

## Overview
The client performs the following steps:
1. **SYN Packet:** Constructs and sends an SYN packet with a sequence number of **200**.
2. **SYN-ACK Reception:** Listens for the server’s SYN-ACK response. The expected response has:
   - **Sequence number:** 400
   - **Acknowledgment number:** 201 (200 + 1)
3. **Final ACK:** Sends the final ACK with a sequence number of **600** (and the acknowledgment number set to 401, which is the server’s sequence + 1).

This handshake sequence is designed to work with the provided server code (server.cpp) that is part of the assignment.

## Files
- **client.cpp:** Client source code with detailed comments explaining each step.
- **server.cpp:** Provided server code (already compiled via the Makefile).
- **Makefile:** Build script for compiling both client and server.
  
## Build Instructions

1. **Clone the repository:**
   ```bash
   git clone https://github.com/privacy-iitk/cs425-2025.git

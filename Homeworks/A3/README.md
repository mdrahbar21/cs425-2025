# Assignment 3 – TCP Handshake using Raw Sockets

## Course: CS425: Computer Networks  

**Instructor**: Prof. Adithya Vadapalli  

## 📁 Contents of Submission

The ZIP file should be named:

**A3_210052_210601_210661.zip**

After extracting the ZIP, you will find the following structure:

```
A3_210052_210601_210661/
│   ├── server.cpp      # Provided server code
│   ├── client.cpp      # Implemented client
│   ├── Makefile        # To build both server and client
│   └── README.md       # This README
```

---

## ⚙️ Requirements

- Operating System: **Linux** (required for raw sockets)
- Privileges: **Root** (raw sockets require superuser access)
- Compiler: **g++** with C++17 support
- Loopback interface: Code uses `127.0.0.1` (localhost)

---

## 🛠️ Build Instructions

After extracting the ZIP file, open a terminal and navigate to the A3 directory:

```bash
cd A3_210052_210601_210661/
```

Compile the code using the provided `Makefile`:

```bash
make
```

You should now see two executable files created:

- `server`
- `client`

---

## 🚀 Running the Code

Since raw sockets require elevated privileges, you **must use `sudo`** to run both the server and the client.

### Step 1: Run the Server

Open **Terminal 1** and run:

```bash
cd A3_210052_210601_210661/
sudo ./server
```

Expected output:

```
[+] Server listening on port 12345...
```

The server will wait for an incoming SYN packet from the client.

---

### Step 2: Run the Client

Open **Terminal 2** and run:

```bash
cd A3_210052_210601_210661/
sudo ./client
```

Expected output:

```
[+] Client starting TCP handshake...
[+] SENT SYN packet with SEQ=200
[+] TCP Flags: SYN: 1 ACK: 1 ...
[+] RECEIVED SYN-ACK (SEQ=400, ACK=201)
[+] SENT final ACK packet with SEQ=600
[+] Handshake complete.
```

---

## 📡 TCP Handshake Protocol

This assignment demonstrates a simplified TCP 3-way handshake using **raw sockets**, with **manually crafted packets**:

1. **SYN (Client → Server)**  
   - Sequence Number: 200  
   - Flags: SYN=1

2. **SYN-ACK (Server → Client)**  
   - Sequence Number: 400  
   - Acknowledgment: 201  
   - Flags: SYN=1, ACK=1

3. **ACK (Client → Server)**  
   - Sequence Number: 600  
   - Acknowledgment: 401  
   - Flags: ACK=1

Each packet is built manually using C++ structures for IP and TCP headers.

---

## 🧪 Expected Output (Server Terminal)

```
[+] Server listening on port 12345...
[+] TCP Flags: SYN: 1 ACK: 0 FIN: 0 RST: 0 PSH: 0 SEQ: 200
[+] Received SYN from 127.0.0.1
[+] Sent SYN-ACK
[+] TCP Flags: SYN: 0 ACK: 1 FIN: 0 RST: 0 PSH: 0 SEQ: 600
[+] Received ACK, handshake complete.
```

---

## 🧯 Troubleshooting

| Issue | Solution |
|-------|----------|
| `Permission denied` | Use `sudo` to run client/server |
| Server not receiving packets | Check if server is running before client |
| No output on client | Make sure both are using `127.0.0.1`, check firewall |
| Raw socket errors | Temporarily disable firewall: `sudo ufw disable` (only for local test) |

---

## 📌 Notes

- This implementation is hardcoded for `127.0.0.1` (localhost) and **port 12345** as defined in `server.cpp`.
- TCP checksum is left as zero; the OS kernel usually fills this in when sending via raw sockets.
- Only a subset of TCP/IP fields are set for simplicity.
- This code **does not use the kernel TCP stack** for connection state. All handshake logic is done manually.

---

## 👨‍💻 Authors

- Adit Gupta (210052)
- Md Rahbar (210601)
- Neerav Sethi (210661)

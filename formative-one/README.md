# Blockchain-Based Attendance Tracking System

A simple blockchain-based attendance tracking system implemented in ANSI C. The system uses SHA-256 hashing and ECDSA digital signatures to ensure attendance records are secure, immutable, and verifiable.

## Features

* Blockchain attendance ledger
* SHA-256 block hashing
* ECDSA digital signatures
* Student registry validation
* Chain integrity verification
* Tamper detection
* CLI-based interaction

## Project Structure

```text
.
├── main.c
├── blockchain.c
├── blockchain.h
├── students.txt
└── README.md
```

## Requirements

* GCC Compiler
* OpenSSL 3.x

## Compilation

```bash
gcc -Wall -Wextra -pedantic -ansi *.c -o attendance -lssl -lcrypto
```

## Running the Application

```bash
./attendance
```

## students.txt Format

```text
ALU001,John Doe,BLK101
ALU002,Jane Smith,BLK101
ALU003,Amara Diallo,BLK101
```

## Functionalities

* Load student registry
* Mark attendance
* Validate student IDs
* View attendance records
* Validate blockchain integrity
* Detect tampering

## Security

* SHA-256 ensures data integrity
* ECDSA signatures authenticate attendance records
* Chain validation detects unauthorized modifications

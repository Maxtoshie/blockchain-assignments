# Blockchain Attendance System with Transaction Models and Mining

## Overview
This system extends a blockchain-based attendance system with token transactions and mining simulation, implementing both UTXO and Account-based transaction models.

## Features
- Mark attendance (PRESENT=10 tokens, LATE=5 tokens, ABSENT=0)
- Pending pool for unconfirmed attendance
- Proof-of-work mining with configurable difficulty (1-4)
- Three mining methods: Solo, Pool, Cloud
- Two transaction models: UTXO and Account-based
- Token transfer between students
- Transaction history (Account model)
- UTXO set tracking (UTXO model)

## Compilation

```bash
make clean
make
./blockchain_attendance

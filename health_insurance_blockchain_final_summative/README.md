# ALU Health Insurance Blockchain System

A blockchain-based health insurance management system built in C for the African Leadership University (ALU). This system provides a secure, immutable, and transparent platform for managing health insurance policies, claims, and payments using blockchain technology.

## Table of Contents

- [Overview](#overview)
- [Features](#features)
- [System Architecture](#system-architecture)
- [Installation](#installation)
- [Usage](#usage)
- [Commands](#commands)
- [Testing](#testing)
- [Technical Details](#technical-details)
- [Contributing](#contributing)
- [License](#license)

## Overview

This system implements a decentralized health insurance platform using blockchain technology. It features:

- Blockchain-based ledger for immutable record keeping
- Account-based transaction model for balance management
- Proof-of-Work mining for block creation
- ECDSA cryptography for digital signatures
- Automatic fraud detection with manual review workflow
- Reinsurance pool for high-value claims
- Disk persistence for state management
- Interactive CLI for system operations

## Features

### Core Blockchain Features
- Block structure with Merkle root
- Proof-of-Work mining (solo and pool)
- Automatic difficulty retargeting (every 10 blocks)
- Blockchain verification and tamper detection
- SHA-256 hashing and ECDSA signatures

### Insurance Operations
- Member registration
- Policy enrollment, renewal, and expiry
- Premium payments with automatic reinsurance (5%)
- Claim submission with fraud detection
- Claim approval, rejection, and settlement
- Reinsurance pool for claims > 1000 AHT

### Security Features
- ECDSA digital signatures
- Account nonces for replay protection
- Fraud detection heuristics:
  - High-frequency claims (>3 in 24 hours)
  - Abnormal claim amounts (>2x historical average)
  - Duplicate transaction detection
- Manual fraud review workflow
- Input validation for all operations

### Additional Features
- UTXO and Account-based transaction models
- Mempool with fee-based prioritization
- Disk persistence (binary format)
- Automatic difficulty adjustment
- Mining rewards distribution

## System Architecture

```
+---------------------------------------------------------+
|                    CLI Interface                        |
+---------------------------------------------------------+
|  +----------+  +----------+  +----------------------+  |
|  |  Mempool |  |  Mining  |  |   Insurance Engine   |  |
|  +----------+  +----------+  +----------------------+  |
|  | Pending  |  | PoW      |  | Policies             |  |
|  | Priority |  | Solo     |  | Claims               |  |
|  | Fraud    |  | Pool     |  | Reinsurance          |  |
|  +----------+  +----------+  +----------------------+  |
+---------------------------------------------------------+
|                  Blockchain Core                        |
|  +--------------------------------------------------+  |
|  |  Block 0  |  Block 1  |  Block 2  |  Block N   |  |
|  |  Genesis  |  Hash: X  |  Hash: Y  |  Hash: Z   |  |
|  |  Hash: A  |  Prev: A  |  Prev: X  |  Prev: Y   |  |
|  +--------------------------------------------------+  |
+---------------------------------------------------------+
|              Account & Transaction Models               |
|  +--------------+  +--------------------------------+  |
|  |    Account   |  |    Transaction Types           |  |
|  |  Balance     |  |  Policy Enrollment             |  |
|  |  Nonce       |  |  Premium Payment               |  |
|  |  Address     |  |  Claim Submission              |  |
|  +--------------+  |  Claim Settlement              |  |
+---------------------+--------------------------------+  |
```

## Installation

### Prerequisites

- GCC compiler (version 9.0 or higher)
- OpenSSL (version 3.0 or higher)
- Make build system
- Linux/Unix environment (or WSL for Windows)

### Build from Source

```bash
# Clone the repository
git clone https://github.com/yourusername/health-insurance-blockchain.git
cd health-insurance-blockchain

# Build the project
make clean
make

# The executable will be created as 'health_insurance'
```

### Directory Structure

```
health_insurance_blockchain/
├── data/              # Persistent blockchain data
├── include/           # Header files
│   ├── account.h
│   ├── blockchain.h
│   ├── block.h
│   ├── cli.h
│   ├── cryptography.h
│   ├── fraud.h
│   ├── insurance.h
│   ├── mempool.h
│   ├── mining.h
│   ├── persistence.h
│   ├── transaction.h
│   └── utils.h
├── src/               # Source files
│   ├── account.c
│   ├── blockchain.c
│   ├── block.c
│   ├── cli.c
│   ├── cryptography.c
│   ├── fraud.c
│   ├── insurance.c
│   ├── main.c
│   ├── mempool.c
│   ├── mining.c
│   ├── persistence.c
│   ├── transaction.c
│   └── utils.c
├── tests/             # Test files
│   └── test_scenario.txt
├── Makefile           # Build configuration
└── README.md          # This file
```

## Usage

### Running the Program

```bash
# Run the application
./health_insurance

# Or using make
make run
```

### Quick Start Example

Here's a complete workflow to test the system:

```bash
# 1. Register a member
Choose operation: 1
Enter member address: alice
Enter member name: Alice
# Output: Member registered: Alice (alice)

# 2. Enroll a policy
Choose operation: 2
Enter member address: alice
Enter coverage plan: Premium
Enter premium amount: 100
# Output: Policy enrolled: POL-1-1781772589

# 3. Pay premium
Choose operation: 3
Enter member address: alice
Enter policy ID: POL-1-1781772589
Enter premium amount: 100
# Output: Premium paid and reinsurance contribution

# 4. Mine a block
Choose operation: 6
Enter miner address: miner1
# Output: Block mined with difficulty X

# 5. Submit a claim
Choose operation: 4
Enter policy ID: POL-1-1781772589
Enter provider address: provider1
Enter claim amount: 500
# Output: Claim submitted and passed fraud checks

# 6. Approve and settle claim
Choose operation: 13 (Approve)
Enter claim ID: CLM-1-1781772589
Enter amount: 500

Choose operation: 15 (Settle)
Enter claim ID: CLM-1-1781772589
Enter amount: 500

# 7. View blockchain
Choose operation: 7
# Shows all blocks with transactions

# 8. Exit
Choose operation: 0
# State is automatically saved
```

## Commands

### Main Menu Options

| # | Category | Operation | Description |
|---|----------|-----------|-------------|
| 1 | Membership | Register New Member | Register a new user in the system |
| 2 | Policy | Enroll Insurance Policy | Create a new insurance policy |
| 3 | Insurance | Pay Premium | Pay premium with automatic reinsurance |
| 4 | Insurance | Submit Claim | Submit a healthcare claim |
| 5 | Mempool | View Transactions | View pending transactions |
| 6 | Mining | Mine Block | Mine a block with Proof-of-Work |
| 7 | Ledger | View Blockchain | Display the complete blockchain |
| 8 | Audit | Verify Chain | Validate chain integrity |
| 9 | Audit | Fraud Review | Review suspicious transactions |
| 10 | Balances | Check Wallets | View account balances |
| 11 | Policy | Renew Policy | Renew an expiring policy |
| 12 | Policy | View Policy | Check policy status |
| 13 | Claim | Approve Claim | Approve a pending claim |
| 14 | Claim | Reject Claim | Reject a claim with reason |
| 15 | Claim | Settle Claim | Settle an approved claim |
| 0 | Exit | Save & Exit | Save state and terminate |

## Testing

### Run Automated Tests

```bash
# Run the test scenario
make test

# Or manually
./health_insurance < tests/test_scenario.txt
```

### Sample Test Scenario

Create `tests/test_scenario.txt`:

```
1
alice
Alice
2
alice
Premium
100
3
alice
POL-1-1781772589
100
6
miner1
4
POL-1-1781772589
provider1
500
13
CLM-1-1781772589
500
15
CLM-1-1781772589
500
7
10
0
```

### Manual Testing

```bash
# Test fraud detection - high frequency claims
# Submit 4 claims in quick succession
./health_insurance
# ... submit 4 claims within 24 hours
# Should flag as SUSPICIOUS

# Test reinsurance - high value claim
# Submit claim > 1000 AHT
# Should split between insurance and reinsurance pools

# Test policy expiry
# Wait for policy to expire (or manually set date)
# Claim should be rejected
```

## Technical Details

### Blockchain Implementation

- Block Size: Maximum 100 transactions per block
- Mining Difficulty: Starts at 2, adjusts every 10 blocks
- Block Reward: 10 AHT (configurable)
- Merkle Tree: SHA-256 binary tree for transaction verification

### Transaction Types

| Type | Description |
|------|-------------|
| POLICY_ENROLLMENT | New insurance policy |
| PREMIUM_PAYMENT | Premium payment |
| REINSURANCE_CONTRIBUTION | 5% reinsurance transfer |
| CLAIM_SUBMISSION | Healthcare claim |
| CLAIM_APPROVAL | Claim approval |
| CLAIM_REJECTION | Claim rejection |
| CLAIM_SETTLEMENT | Claim settlement |
| TOKEN_TRANSFER | AHT token transfer |
| MINING_REWARD | Block mining reward |
| POOL_REWARD | Pool mining reward |

### Security Features

- ECDSA: secp256k1 curve for digital signatures
- Account Nonces: Replay attack protection
- Fraud Detection: Automated suspicious pattern detection
- Manual Review: Human oversight for flagged transactions
- Tamper Detection: Blockchain verification on load

### Persistence

- Format: Binary (.bin) for efficiency
- Save Trigger: After each mined block and on exit
- Load Trigger: On program startup
- Verification: Blockchain re-validated on load

## Contributing

1. Fork the repository
2. Create your feature branch (`git checkout -b feature/amazing`)
3. Commit changes (`git commit -m 'Add amazing feature'`)
4. Push to branch (`git push origin feature/amazing`)
5. Open a Pull Request

### Coding Standards

- Follow ANSI C standards
- Use the Betty style guide
- Write inline comments for complex logic
- Update tests for new features

## License

This project is developed as part of the Software Engineering – Low-Level Specialization at the African Leadership University (ALU).

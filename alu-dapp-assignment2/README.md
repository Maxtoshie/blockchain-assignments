---

# ALU Digital Asset dApp – Assignment 2

A browser-based dApp that connects to the **ALUAssetRegistry (ERC-721)** and **ALULogoToken (ERC-20)** smart contracts, allowing users to register, verify, and manage ownership of the ALU logo through a web interface.

## Features

* Connect a MetaMask wallet.
* Register the ALU logo by hashing it with SHA-256 and storing it on-chain.
* Verify a logo by uploading a file or entering its hash (no wallet required).
* View ALUT balances and ownership percentages.
* Distribute ALUT tokens (contract owner only).

## Architecture

The frontend is built with **HTML, CSS, JavaScript, and ethers.js**. It connects to MetaMask for transactions and uses a read-only provider for public verification.

**Flow:** Browser → ethers.js → MetaMask/JSON-RPC → Hardhat → Smart Contracts

## Registered Logo Hash

```
0xe03128e7ed668d74c334ed965edf89d0e1b3da112f6aeb6233bfd191ad829348
```

## Versions

| Tool         | Version               |
| ------------ | --------------------- |
| Node.js      | v20                   |
| Hardhat      | ^2.28.6               |
| ethers.js    | v6                    |
| OpenZeppelin | ^5.6.1                |
| Frontend     | HTML, CSS, JavaScript |

## Installation

```bash
npm install
```

## Run the Project

```bash
# Start Hardhat
npx hardhat node

# Deploy contracts
npm run deploy:local

# Start frontend
npm run serve
```

Open **[http://localhost:8080](http://localhost:8080)**.

## Using the dApp

1. Connect MetaMask.
2. Register a logo by uploading an image.
3. Verify a logo using a file or hash.
4. View token balances and ownership.
5. Distribute ALUT tokens (owner only).

## Testing

```bash
npm test
```

The project includes **13 automated tests**:

* 8 smart contract tests.
* 5 frontend integration tests.

## Known Limitations

* Supports MetaMask-compatible wallets only.
* Example holder addresses are for demonstration.
* Contract addresses are regenerated after each local deployment.

## Project Structure

```
contracts/
frontend/
scripts/
test/
alu-logo.png
hardhat.config.js
README.md
Project_Report.pdf
package.json
```

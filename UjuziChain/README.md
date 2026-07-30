# UjuziChain

A decentralized academic and professional credential verification platform built on Ethereum.

UjuziChain enables accredited institutions to issue tamper-proof digital credentials that students own and employers can verify instantly.

## Tech Stack

- Solidity
- Hardhat
- React (Vite)
- ethers.js
- MetaMask
- Ethereum (Localhost & Sepolia)

## Project Structure

```text
UjuziChain/
├── contracts/      # Smart contracts
├── frontend/       # React frontend
├── scripts/        # Deployment scripts
├── test/           # Hardhat test suite
├── docs/           # Project documentation
├── hardhat.config.js
├── package.json
└── README.md
```

## Getting Started

Clone the repository:

```bash
git clone https://github.com/<your-username>/UjuziChain.git
cd UjuziChain
```

Install dependencies:

```bash
npm install
```

Compile the smart contract:

```bash
npx hardhat compile
```

Run the tests:

```bash
npx hardhat test
```

Start the frontend:

```bash
cd frontend
npm install
npm run dev
```

## License

This project is licensed under the MIT License.

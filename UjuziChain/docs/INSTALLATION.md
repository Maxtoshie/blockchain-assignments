# Installation Guide

## Prerequisites

- Node.js 18+ and npm
- MetaMask browser extension
- A Sepolia testnet RPC URL (from [Alchemy](https://www.alchemy.com/) or [Infura](https://infura.io/))
- Sepolia test ETH (from a faucet, e.g. [sepoliafaucet.com](https://sepoliafaucet.com/))

## 1. Clone and install root dependencies

```bash
git clone <your-repo-url>
cd UjuziChain
npm install
```

This installs Hardhat, OpenZeppelin contracts, and the testing toolchain.

## 2. Configure environment variables

```bash
cp .env.example .env
```

Fill in `.env` with:

```
SEPOLIA_RPC_URL=https://eth-sepolia.g.alchemy.com/v2/YOUR_API_KEY
PRIVATE_KEY=your_wallet_private_key_here
ETHERSCAN_API_KEY=your_etherscan_api_key_here
```

**Never commit your real `.env` file.** It's already excluded via `.gitignore`.

## 3. Compile and test the contract

```bash
npx hardhat compile
npx hardhat test
```

All tests in `test/UjuziChain.test.js` should pass, covering institution
registration, credential issuance, revocation, verification, unauthorized
access attempts, and edge cases.

## 4. Install and run the frontend

```bash
cd frontend
npm install
npm run dev
```

The app runs at `http://localhost:5173` by default.

## 5. Connect MetaMask

- Switch MetaMask to the **Sepolia** test network.
- Make sure your account holds some Sepolia test ETH.
- Click **Connect Wallet** on the app.

At this point the frontend will work for read operations (verification)
against whatever address is set in `frontend/src/utils/contractConfig.js`.
For write operations (issuing/revoking/registering), you'll need to deploy
your own instance first — see [DEPLOYMENT.md](DEPLOYMENT.md).

# Deployment Guide

This guide walks through deploying `UjuziChain.sol` to the Sepolia testnet
and wiring the frontend up to the deployed contract.

## 1. Prerequisites

- `.env` configured as described in [INSTALLATION.md](INSTALLATION.md)
- Your deployer wallet funded with Sepolia test ETH

## 2. Deploy the contract

```bash
npx hardhat run scripts/deploy.js --network sepolia
```

Expected output:

```
Deploying UjuziChain...
Deploying with account: 0xYourDeployerAddress
Account balance: 0.4821 ETH
UjuziChain deployed to: 0xYourDeployedContractAddress
Deployer (admin) address: 0xYourDeployerAddress
```

**Copy the deployed contract address.** The account that deploys the
contract automatically becomes the platform Admin (via OpenZeppelin's
`Ownable`).

## 3. Verify the contract on Etherscan

```bash
CONTRACT_ADDRESS=0xYourDeployedContractAddress npx hardhat run scripts/verify.js --network sepolia
```

This publishes the contract source on Sepolia Etherscan so anyone —
including employers verifying credentials — can inspect the code directly.

## 4. Point the frontend at your deployed contract

Open `frontend/src/utils/contractConfig.js` and update:

```js
export const CONTRACT_ADDRESS = "0xYourDeployedContractAddress";
```

## 5. Register your first institution

As the admin account, either:

- Use the **Admin Dashboard** in the running frontend, or
- Call it directly via Hardhat console:

```bash
npx hardhat console --network sepolia
```

```js
const UjuziChain = await ethers.getContractFactory("UjuziChain");
const contract = UjuziChain.attach("0xYourDeployedContractAddress");
await contract.registerInstitution("0xInstitutionWalletAddress", "University of Nairobi");
```

## 6. Deploy the frontend (optional, for a public demo)

```bash
cd frontend
npm run build
```

Deploy the resulting `dist/` folder to any static host (Vercel, Netlify,
GitHub Pages). Since verification is a public read call, employers can use
a deployed frontend without ever installing MetaMask — the app falls back
to a public Sepolia RPC endpoint for read-only queries.

## Troubleshooting

| Issue | Likely cause |
|---|---|
| `insufficient funds for gas` | Deployer wallet needs more Sepolia test ETH |
| `HH502` compiler download error | Network/firewall blocking `binaries.soliditylang.org` |
| Contract calls revert unexpectedly | Confirm MetaMask is on Sepolia, not Mainnet or another testnet |
| Frontend shows no credentials | Confirm `CONTRACT_ADDRESS` in `contractConfig.js` matches your deployment |

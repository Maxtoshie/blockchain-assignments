const hre = require("hardhat");

async function main() {
  console.log("Deploying UjuziChain...");

  const [deployer] = await hre.ethers.getSigners();
  console.log("Deploying with account:", deployer.address);

  const balance = await hre.ethers.provider.getBalance(deployer.address);
  console.log("Account balance:", hre.ethers.formatEther(balance), "ETH");

  const UjuziChain = await hre.ethers.getContractFactory("UjuziChain");
  const ujuziChain = await UjuziChain.deploy();
  await ujuziChain.waitForDeployment();

  const address = await ujuziChain.getAddress();
  console.log("UjuziChain deployed to:", address);
  console.log("Deployer (admin) address:", deployer.address);

  console.log("\nNext steps:");
  console.log("1. Save this contract address into frontend/src/utils/contractConfig.js");
  console.log("2. Verify on Etherscan with: npx hardhat run scripts/verify.js --network sepolia");
  console.log("3. Register your first institution using registerInstitution(address, name)");
}

main().catch((error) => {
  console.error(error);
  process.exitCode = 1;
});
